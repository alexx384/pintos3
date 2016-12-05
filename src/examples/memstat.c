#include <stdio.h>
#include <syscall.h>

int
main (int argc, char **argv)
{ 

  printf("All memory = %d b\n", ram_size ());
  printf("Free memory = %d b\n", ram_free_size ());

  return EXIT_SUCCESS;
}
