#ifndef USERPROG_SYSCALL_H
#define USERPROG_SYSCALL_H

#include "userprog/process.h"

/* Work with threads */
void add_to_list(int pid);
int if_elem_in_list(int pid);
void global_size(int i);
/* Exit */
void exit(int value);

void syscall_init (void);

#endif /* userprog/syscall.h */
