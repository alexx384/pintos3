#include "userprog/syscall.h"
#include <stdio.h>
#include <syscall-nr.h>
#include "threads/interrupt.h"
#include "threads/thread.h"
#include "threads/vaddr.h"
#include "threads/init.h"
#include "userprog/process.h"

static void syscall_handler (struct intr_frame *);

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

	switch (sys_num)
	{
		case SYS_HALT:
		{
			shutdown_power_off();
		}break;
		case SYS_EXIT:
		{
			set_exit_status(((size_t*) f->esp)[1]);			
			thread_exit();// вызов exit() должен завершать программу
		}break;
		case SYS_EXEC:
		{	
			return_address=process_execute(args[0]);
			return return_address;	
		}break;
		case SYS_WAIT:
		{
			process_wait(args[0]);
			return;
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
	if (is_kernel_vaddr (esp))
	{
		printf("Error unknown stack addres!\n");
		thread_exit ();		
	}	
}