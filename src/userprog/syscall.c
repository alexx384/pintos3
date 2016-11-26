#include "userprog/syscall.h"
#include <stdio.h>
#include <syscall-nr.h>
#include "threads/interrupt.h"
#include "threads/thread.h"
#include "threads/vaddr.h"
#include "threads/init.h"
#include "threads/synch.h"
#include "userprog/process.h"

static void syscall_handler (struct intr_frame *);
static void get_user (const uint8_t *uaddr);
static bool put_user (uint8_t *udst, uint8_t byte);
static void exec_once(void);
static void add_to_list(int pid);
static void if_elem_in_list(int pid);
static void exit(int value);

static struct semaphore sema_for_execute={0};
static int once=1;

struct pid_list
{
	int *list;
	int count;
};

static struct pid_list list_for_pid;

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

	//printf("sys_num = %d\n",sys_num);
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
			get_user(args[0]);
			return_address=process_execute(args[0]);
			f->eax=return_address;

			sema_up(&sema_for_execute);
			return;	
		}break;
		case SYS_WAIT:
		{
			if_elem_in_list(args[0]);
			return_address=process_wait(args[0]);
			f->eax=return_address;
			add_to_list(args[0]);
			return return_address;
		}break;
		case SYS_WRITE:
		{
			putbuf( ((const char**) f->esp)[2], ((size_t*) f->esp)[3]);
			return;
		}break;
	}

	printf ("system call!\n");
	thread_exit ();
}

void validate_user_pointer(const void *esp)
{
	if (!is_user_vaddr (esp))
	{
		printf("Error unknown stack addres!\n");
		thread_exit ();		
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

  if(result == -1)
  {
  	exit(-1);
  }
  return result;
}

/* Writes BYTE to user address UDST.
   UDST must be below PHYS_BASE.
   Returns true if successful, false if a segfault occurred. */
static bool
put_user (uint8_t *udst, uint8_t byte)
{
  int error_code;
  asm ("movl $1f, %0; movb %b2, %1; 1:"
       : "=&a" (error_code), "=m" (*udst) : "q" (byte));
  return error_code != -1;
}

static void 
exec_once()
{
	sema_init(&sema_for_execute, 1);
	list_for_pid.count=1;
}

static void 
add_to_list(int pid)
{
	if(list_for_pid.count == 1)
	{
		list_for_pid.list=malloc(sizeof(int));
		list_for_pid.list[0]=pid;
	}else{
		list_for_pid.list=realloc(list_for_pid.list, sizeof(int)*list_for_pid.count);
		list_for_pid.list[list_for_pid.count]=pid;
	}
	++list_for_pid.count;
}

static void 
if_elem_in_list(int pid)
{
	int i;
	for(i=0; i < (list_for_pid.count+1); ++i)
	{
		if(list_for_pid.list[i] == pid)
			exit(-1);
	}
}

static void 
exit(int value)
{
	set_exit_status(value);			
	thread_exit();	
}