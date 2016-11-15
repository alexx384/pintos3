#include <stdio.h>
#include "tests/threads/tests.h"
#include "threads/thread.h"
#include "threads/synch.h"
#include "narrow-bridge.h"
#include "devices/timer.h"

#define N 3
#define F 5

static int parts_of_eat=F;
static struct semaphore sema_sleep;
static struct semaphore sema_mother;

static void chick_is_eat (void);

void
birds (void)
{
	int i, a;

	sema_init(&sema_sleep, 0);
	sema_init(&sema_mother, 0);
	thread_set_priority(PRI_DEFAULT+10);

	for(i=0; i<N; i++)
	{
    	char name[16];

    	snprintf (name, sizeof name, "Chick %d", i);
    	thread_create (name, PRI_DEFAULT, chick_is_eat, NULL);
	}

	while(true)
	{

		sema_down(&sema_mother);

		for(a=N; a>0; a--)
			sema_up(&sema_sleep);

		parts_of_eat=F;
	}
}

void chick_is_eat(void)
{
	struct thread *cur=thread_current();


	while(true)
	{
		if(parts_of_eat <= 0 && list_size(&sema_sleep.waiters) < N-1)	sema_down(&sema_sleep);
		else if(parts_of_eat <= 0)
		{
			msg("Eat is end. Mother wake up");
			sema_up(&sema_mother);
			sema_down(&sema_sleep);
		}
		msg("%s is eat and start to rest. Parts of eat = %d",cur->name, parts_of_eat);
		parts_of_eat--;

		timer_sleep(10);
	}
}