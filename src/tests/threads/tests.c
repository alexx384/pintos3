#include "tests/threads/tests.h"
#include <debug.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "threads/malloc.h"

enum test_args_type
  {
    test_no_args,
    test_args_4uint
  };

struct test 
  {
    const char *name;
    void* function;
    enum test_args_type args_type;
  };


static const struct test tests[] = 
  {
    {"alarm-single", test_alarm_single, test_no_args},
    {"alarm-multiple", test_alarm_multiple, test_no_args},
    {"alarm-simultaneous", test_alarm_simultaneous, test_no_args},
    {"alarm-priority", test_alarm_priority, test_no_args},
    {"alarm-zero", test_alarm_zero, test_no_args},
    {"alarm-negative", test_alarm_negative, test_no_args},
    {"priority-change", test_priority_change, test_no_args},
    {"priority-donate-one", test_priority_donate_one, test_no_args},
    {"priority-donate-multiple", test_priority_donate_multiple, test_no_args},
    {"priority-donate-multiple2", test_priority_donate_multiple2, test_no_args},
    {"priority-donate-nest", test_priority_donate_nest, test_no_args},
    {"priority-donate-sema", test_priority_donate_sema, test_no_args},
    {"priority-donate-lower", test_priority_donate_lower, test_no_args},
    {"priority-donate-chain", test_priority_donate_chain, test_no_args},
    {"priority-fifo", test_priority_fifo, test_no_args},
    {"priority-preempt", test_priority_preempt, test_no_args},
    {"priority-sema", test_priority_sema, test_no_args},
    {"priority-condvar", test_priority_condvar, test_no_args},
    {"mlfqs-load-1", test_mlfqs_load_1, test_no_args},
    {"mlfqs-load-60", test_mlfqs_load_60, test_no_args},
    {"mlfqs-load-avg", test_mlfqs_load_avg, test_no_args},
    {"mlfqs-recent-1", test_mlfqs_recent_1, test_no_args},
    {"mlfqs-fair-2", test_mlfqs_fair_2, test_no_args},
    {"mlfqs-fair-20", test_mlfqs_fair_20, test_no_args},
    {"mlfqs-nice-2", test_mlfqs_nice_2, test_no_args},
    {"mlfqs-nice-10", test_mlfqs_nice_10, test_no_args},
    {"mlfqs-block", test_mlfqs_block, test_no_args},
    {"narrow-bridge", narrow_bridge, test_args_4uint},
    {"birds", birds, test_no_args}
  };

static const char *test_name;

void call_test(const struct test* t, const char* name_args);

/* Runs the test named NAME. */
void
run_test (const char *name) 
{
  const struct test *t;

  for (t = tests; t < tests + sizeof tests / sizeof *tests; t++)
  {
    // Tests without args
    if (!strcmp (name, t->name))
      {
        test_name = name;
        msg ("begin");
        call_test(t, name);
        msg ("end");
        return;
      }

    // Tests with args
    if (strstr(name, " ") && !strncmp(name, t->name, strstr(name, " ") - name ) )
    {
       size_t sz = strstr(name, " ") - name;
       char* nm = malloc(sz + 1);
       memcpy(nm, name, sz);
       nm[sz] = 0;

       test_name = nm;
       msg ("begin");
       call_test(t, name);
       msg ("end");

       free(nm);
       return;
    }
  }

  PANIC ("no test named \"%s\"", name);
}

/* Prints FORMAT as if with printf(),
   prefixing the output by the name of the test
   and following it with a new-line character. */
void
msg (const char *format, ...) 
{
  va_list args;
  
  printf ("(%s) ", test_name);
  va_start (args, format);
  vprintf (format, args);
  va_end (args);
  putchar ('\n');
}

/* Prints failure message FORMAT as if with printf(),
   prefixing the output by the name of the test and FAIL:
   and following it with a new-line character,
   and then panics the kernel. */
void
fail (const char *format, ...) 
{
  va_list args;
  
  printf ("(%s) FAIL: ", test_name);
  va_start (args, format);
  vprintf (format, args);
  va_end (args);
  putchar ('\n');

  PANIC ("test failed");
}

/* Prints a message indicating the current test passed. */
void
pass (void) 
{
  printf ("(%s) PASS\n", test_name);
}


void call_test(const struct test* t, const char* name_args)
{
   if (t->args_type == test_no_args)
   {
      test_func* p = (test_func*) t->function;
      p();
   }
   else if (t->args_type == test_args_4uint && strstr(name_args, " "))
   {
      const char* cargs = strstr(name_args, " ") + 1;
      test_func_args_4uint* p = (test_func_args_4uint *) t->function;
      unsigned int params[4] = { 0 };
      char *token, *save_ptr, *args;
      int cnt = 0;
      size_t sz = sizeof(char) * (strlen(cargs) + 1);

      args = (char*) malloc(sz);
      memcpy(args, cargs, sz);

      for (token = strtok_r (args, " ", &save_ptr); token != NULL; token = strtok_r (NULL, " ", &save_ptr))
      {
          if (cnt < 4)
          {
              params[cnt] = (unsigned int) atoi(token);
              cnt++;
          }
      }

      free(args);

      if (cnt < 4)
      {
          PANIC("invalid arguments count");
          return;
      }

      p(params[0], params[1], params[2], params[3]);
   }
   else
   {
       PANIC("invalid arguments");
   }
}
