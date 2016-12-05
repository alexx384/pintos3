#include "userprog/syscall.h"
#include <stdio.h>
#include <syscall-nr.h>
#include "threads/interrupt.h"
#include "threads/thread.h"
#include "threads/vaddr.h"
#include "threads/init.h"
#include "threads/synch.h"
#include "threads/malloc.h"
#include "devices/shutdown.h"
#include "devices/input.h"
#include "filesys/filesys.h"
#include "filesys/file.h"
#include "filesys/directory.h"
#include "filesys/inode.h"

struct dir_entry 
  {
    block_sector_t inode_sector;        /* Sector number of header. */
    char name[NAME_MAX + 1];            /* Null terminated file name. */
    bool in_use;                        /* In use or free? */
  };

struct dir 
  {
    struct inode *inode;                /* Backing store. */
    off_t pos;                          /* Current position. */
  };  

static void syscall_handler (struct intr_frame *);
static void get_user (const uint8_t *uaddr);
static void exec_once(void);
static void validate_user_pointer(const void *esp);
/* Work with filesys */
static int add_file_to_list(struct file *opened_file);
static struct file *get_file_by_fd(const int fd);
static void close_files(struct file *opened);
static void delete_open_file_data(struct open_file **opened);

static struct semaphore sema_for_execute={0};
static int once=1;

void
syscall_init (void) 
{
  intr_register_int (0x30, 3, INTR_ON, syscall_handler, "syscall");
}

static void
syscall_handler (struct intr_frame *f UNUSED) 
{
	int sys_num = *(int*) f->esp;
	int *args = (int*) f->esp + 1;
	int return_address;


	validate_user_pointer((const void*) f->esp);

	if(once)
		exec_once();

	switch (sys_num)
	{
		case SYS_HALT:
		{
			shutdown_power_off();
		}break;
		case SYS_EXIT:
		{
			exit(((size_t*) f->esp)[1]);			
		}break;
		case SYS_EXEC:
		{
			sema_down(&sema_for_execute);

			validate_user_pointer((const void*) args[0]);
			get_user((const uint8_t*) args[0]);
			return_address=process_execute((const char*) args[0]);
			f->eax=return_address;
			add_to_list(return_address);

			sema_up(&sema_for_execute);
			return;	
		}break;
		case SYS_WAIT:
		{
			if(if_elem_in_list(args[0]) >= 1)
			{
				return_address=process_wait(args[0]);
				f->eax=return_address;
				
				add_to_list(args[0]);
			}else f->eax=-1;

			return;
		}break;
		case SYS_CREATE:
		{
			filesys_init (0);
			if ((int*) args[0] == NULL)	exit(-1);
			
			f->eax = filesys_create ((const char*) args[0], args[1]);
			return;
		}break;
		case SYS_REMOVE:
		{
			if ((int*) args[0] == NULL)	exit(-1);
			filesys_remove ((const char*) args[0]);
			return;
		}break;
		case SYS_OPEN:
		{
			if ((int*) args[0] == NULL)	exit(-1);
				
			struct file *opened_file = opened_file = filesys_open ((const char*) args[0]);
				

			if (opened_file == NULL)
				f->eax=-1;
			else
				f->eax = add_file_to_list(opened_file);

			return;
		}break;
		case SYS_FILESIZE:
		{
			if ((int*) args[0] == NULL)	exit(-1);

			struct file *opened = get_file_by_fd((const int) args[0]);
			f->eax = file_length(opened);
			return;
		}break;
		case SYS_READ:
		{
			if ((int*) args[0] == NULL)	exit(-1);

			if (args[0] == 0)
			{
				input_init ();
				
				input_getc ();
				return;	
			}

			validate_user_pointer((const void*) args[1]);
			get_user((const uint8_t*) args[1]);

			struct file *opened = get_file_by_fd((const int) args[0]);

			if (opened != NULL)
			{
				f->eax = file_read(opened, (void*) args[1], args[2]);	
			}else 	f->eax=-1;
			return;
		}break;
		case SYS_CLOSE:
		{
			if ((int*) args[0] == NULL)	exit(-1);

			struct file *opened = get_file_by_fd((const int) args[0]);

			if (opened != NULL)		close_files(opened);
			else 					f->eax=-1;	
			return;
		}break;
		case SYS_WRITE:
		{
			if ((int*) args[0] == NULL)	exit(-1);

			if (args[0] == 1)
			{
				putbuf( ((const char**) f->esp)[2], ((size_t*) f->esp)[3]);

				return;	
			}

			struct file *opened = get_file_by_fd((const int) args[0]);

			if (opened != NULL)
			{
				f->eax = file_write (opened, (const void*) args[1], args[2]);
			}		
			else 					f->eax=-1;
			return;	
		}break;
		case SYS_RAM_SIZE:
		{			
  			f->eax = init_ram_pages * PGSIZE;
			return;
		}break;
		case SYS_RAM_FREE_SIZE:
		{			
			uint8_t *free_start = ptov (1024 * 1024);
  			uint8_t *free_end = ptov (init_ram_pages * PGSIZE);
  			f->eax = free_end - free_start;
			return;
		}break;
		case SYS_DISK_SPACE:
		{
			struct dir_entry e;
  			size_t ofs;
  			struct dir *dir = dir_open_root ();
  			int i = 0;

			for (ofs = 0; inode_read_at (dir->inode, &e, sizeof e, ofs) == sizeof e;
    		    ofs += sizeof e) 
    			if (e.in_use) 
        		{
        			printf("%s\n",e.name );
        			++i;
      			}
      		printf("\nTotal count of files %d\n",i );	
			return;
		}break;
	}

	printf ("system call!\n");
	thread_exit ();
}
static void 
validate_user_pointer(const void *esp)
{
	if (!is_user_vaddr (esp))
	{
		exit(-1);		
	}	
}

/* Reads a byte at user virtual address UADDR.
   UADDR must be below PHYS_BASE.
   Returns the byte value if successful, -1 if a segfault
   occurred. */
static void
get_user (const uint8_t *uaddr)
{
  int result;
  asm ("movl $1f, %0; movzbl %1, %0; 1:"
       : "=&a" (result) : "m" (*uaddr));
}	

static void 
exec_once()
{
	sema_init(&sema_for_execute, 1);
}

int
add_file_to_list(struct file *opened_file)
{
	struct thread *cur = thread_current();
	struct open_file *open_file = malloc(sizeof(struct open_file));

	open_file->file = opened_file;

	if (cur->files == NULL)
	{
		open_file->next = NULL;
		open_file->before = NULL;
	}else{
		cur->files->next = open_file;
		open_file->before = cur->files;
	} 						
	
	cur->files = open_file;

	open_file->fd = cur->count_open_file;
	
	++(cur->count_open_file);
	return open_file->fd;
}

struct file *
get_file_by_fd(const int fd)
{
	struct thread *cur = thread_current();
	struct open_file *open_file;

	if (cur->files == NULL)
		return 0;

	for (open_file = cur->files; open_file != NULL; open_file = open_file->before)
	{
		if (open_file->fd == fd)
		return open_file->file;
	}
	return 0;	
}

void
close_files(struct file *opened)
{
	struct thread *cur = thread_current();
	struct open_file *open_file;

	if (cur->files == NULL)
		return;

	for (open_file = cur->files; open_file != NULL; open_file = open_file->before)
	{
		if (open_file->file == opened)
		{	
			file_close (opened);
			delete_open_file_data(&open_file);
			if (open_file == NULL){	cur->files = open_file;	return;}
				
		}
	}
	return;
}

void
delete_open_file_data(struct open_file **opened)
{
	struct open_file *dump = (*opened)->next;
	if (dump != NULL)
	{
		dump->before = (*opened)->before;
	}

	free((*opened));
	(*opened) = dump;
}

void 
add_to_list(int pid)
{
	struct pid_list *list_for_pid = &(thread_current()->child_thread_list);
	int in_list;

	if (list_for_pid->count == 0)
	{
		list_for_pid->list=malloc(sizeof(int));
		list_for_pid->value=malloc(sizeof(int));
		list_for_pid->list[0]=pid;
		list_for_pid->value[0]=0;
	}else{
		in_list=if_elem_in_list(pid);

		if (in_list == 0)
		{
			list_for_pid->list=realloc(list_for_pid->list, sizeof(int)*(list_for_pid->count));
			list_for_pid->value=realloc(list_for_pid->value, sizeof(int)*(list_for_pid->count));
			list_for_pid->list[list_for_pid->count]=pid;
			list_for_pid->value[list_for_pid->count]=0;
		}
		if (in_list >= 1)
		{
			++(list_for_pid->value[in_list-1]);
		 	return;
		}
		if (in_list == -1)	return;	
		
	}
	++(list_for_pid->count);
}

int 
if_elem_in_list(int pid)
{
	int i;
	struct pid_list *list_for_pid = &(thread_current()->child_thread_list);

	for(i=0; i < list_for_pid->count; ++i)
	{
		if(list_for_pid->list[i] == pid)
		{
			if(list_for_pid->value[i] == 0)
				return i+1;
			else
				return -1;
		}
	}
	return 0;
}

void 
exit(int value)
{
	set_exit_status(value);			
	thread_exit();	
}