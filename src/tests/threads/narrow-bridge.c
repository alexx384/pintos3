
/* File for 'narrow_bridge' task implementation.  
   Only output required. 
   SPbSTU, IBKS, 2016 */

#include <stdio.h>
#include "tests/threads/tests.h"
#include "threads/thread.h"
#include "threads/synch.h"
#include "narrow-bridge.h"

static struct semaphore sema_norm_left_side;
static struct semaphore sema_norm_right_side;
static struct semaphore sema_emer_left_side;
static struct semaphore sema_emer_right_side;
static struct semaphore sema_ready;
static int count_of_cars=0;
static int repare=0;

int try_up_car(int direct);
void wake_up_car(enum car_priority prio, enum car_direction dir);
void down_car(enum car_priority prio, enum car_direction dir);

// Called before test
void narrow_bridge_init(unsigned int num_vehicles_left, unsigned int num_vehicles_right,
        unsigned int num_emergency_left, unsigned int num_emergency_right)
{
	// Not implemented
	sema_init(&sema_norm_left_side, 0);			
	sema_init(&sema_norm_right_side, 0);			
	sema_init(&sema_emer_left_side, 0);			
	sema_init(&sema_emer_right_side, 0);
	sema_init(&sema_ready, 0);		//For ready car
	count_of_cars=num_vehicles_left + num_vehicles_right +
				  num_emergency_left + num_emergency_right;
}

void arrive_bridge(enum car_priority prio, enum car_direction dir)
{
	int antidir=(dir-1)*(-1);

	count_of_cars--;
	if(count_of_cars == 0)
	{
		if(prio == 1)
		{
			if(dir == 0)
			{
				if(list_size(&sema_emer_left_side.waiters) != 0)
				{
					wake_up_car(1, dir);
					return;
				}
				if(list_size(&sema_emer_right_side.waiters) > 1)
				{
					wake_up_car(1, antidir);
					wake_up_car(1, antidir);
					down_car(prio, dir);
					return;
				}
				if(try_up_car(dir)==2)
				{
					wake_up_car(0, dir);
					return;
				}	
			}else{
				if(list_size(&sema_emer_right_side.waiters) != 0)
				{
					wake_up_car(1, dir);
					return;
				}
				if(list_size(&sema_emer_left_side.waiters) > 1)
				{
					wake_up_car(1, antidir);
					wake_up_car(1, antidir);
					down_car(prio, dir);
					return;
				}
				if(try_up_car(dir)==2)
				{
					wake_up_car(0, dir);
					return;
				}
			}
			
			if(list_empty(&sema_norm_left_side.waiters) && list_empty(&sema_norm_right_side.waiters))	return;
			wake_up_car(0, antidir);
			wake_up_car(0, antidir);
			down_car(prio, dir);
		}else{
			if(dir == 0)
			{
				if(list_size(&sema_emer_right_side.waiters) > 1)
				{
					wake_up_car(1, antidir);
					wake_up_car(1, antidir);
					down_car(prio, dir);
					return;
				}

				if(list_size(&sema_emer_right_side.waiters) != 0)
				{
					wake_up_car(1, antidir);
					return;
				}
				if(list_size(&sema_norm_left_side.waiters) != 0)
				{
					wake_up_car(0, dir);
					return;
				}	
			}else{
				if(list_size(&sema_emer_left_side.waiters) > 1)
				{
					wake_up_car(1, antidir);
					wake_up_car(1, antidir);
					down_car(prio, dir);
					return;
				}

				if(list_size(&sema_emer_left_side.waiters) != 0)
				{
					wake_up_car(1, antidir);
					return;
				}
				
				if(list_size(&sema_norm_right_side.waiters) != 0)
				{
					wake_up_car(0, dir);
					return;
				}
			}

			if(list_empty(&sema_norm_left_side.waiters) && list_empty(&sema_norm_right_side.waiters))	return;
			wake_up_car(0, antidir);
			wake_up_car(0, antidir);
			down_car(prio, dir);

		}
	}

	// Not implemented
	down_car(prio, dir);
}

void exit_bridge(enum car_priority prio, enum car_direction dir)
{
	// Not implemented
	static int end_car=0;
	int antidir=(dir-1)*(-1);
	end_car++;
	
	if(end_car == 1 &&
	 (!list_empty(&sema_emer_left_side.waiters) ||
	 !list_empty(&sema_emer_right_side.waiters) ||
	 !list_empty(&sema_norm_left_side.waiters)	||
	 !list_empty(&sema_norm_right_side.waiters)))	sema_down(&sema_ready);		//if there is one car, then we are waiting second car 
	else				sema_up(&sema_ready);

	end_car--;
	if(end_car == -1)	return;

	if(antidir == 0)									//Change direction every time
	{
		if(list_size (&sema_emer_left_side.waiters) > 1 && end_car==1)
		{		wake_up_car(1, antidir);	end_car=7;	return;}
		if(list_size (&sema_emer_left_side.waiters) > 0 && end_car==0)
		{		wake_up_car(1, antidir);	return;}

		if(end_car == 3)
		{		wake_up_car(0, dir);	end_car=0;	return;}
		if(end_car == 4)
		{		wake_up_car(0, antidir);	end_car=0;	return;}
		if(end_car == 5)
		{		wake_up_car(1, dir);	end_car=0;	return;}
		if(end_car == 6)
		{		wake_up_car(1, antidir);	end_car=0;	return;}

		if(list_size (&sema_emer_left_side.waiters) == 1 && end_car==1 && !list_empty(&sema_norm_left_side.waiters))
		{		wake_up_car(1, antidir);	end_car=5;	return;}


		if(list_size (&sema_emer_right_side.waiters) > 1 && end_car==1)
		{		wake_up_car(1, dir);	end_car=6;	return;	}
		if(list_size (&sema_emer_right_side.waiters) > 0 && end_car==0)
		{		wake_up_car(1, dir);	return;	}

		if(list_size (&sema_emer_left_side.waiters) == 1 && end_car==1)
		{		wake_up_car(1, antidir);	end_car=0;	return;}

		if(list_size (&sema_emer_right_side.waiters) == 1 && end_car==1 && !list_empty(&sema_norm_right_side.waiters))
		{		wake_up_car(1, dir);	end_car=4;	return;}
		if(list_size (&sema_emer_right_side.waiters) == 1 && end_car==1)
		{		wake_up_car(1, dir);	end_car=0;	return;}

		if(list_size (&sema_norm_left_side.waiters) > 1 && end_car==1)
		{		wake_up_car(0, antidir);	end_car=5;	return;}
		if(list_size (&sema_norm_right_side.waiters) > 1 && end_car==1)
		{		wake_up_car(0, dir);	end_car=4;	return;}

		if(!list_empty(&sema_norm_left_side.waiters))
		{		wake_up_car(0, antidir);	end_car=0;	return;}
		wake_up_car(0, dir);	end_car=0;	return;

	}else{
		//msg("emer_right = %d\temer_left = %d\tend_car = %d",list_size (&sema_emer_right_side.waiters),
		//list_size (&sema_emer_left_side.waiters), end_car);
		if(list_size (&sema_emer_right_side.waiters) > 1 && end_car==1)
		{		wake_up_car(1, antidir);	end_car=7;	return;	}
		if(list_size (&sema_emer_right_side.waiters) > 0 && end_car==0)
		{		wake_up_car(1, antidir);	return;	}

		if (end_car == 3)
		{		wake_up_car(0, antidir);	end_car=0;	return;}
		if (end_car == 4)
		{		wake_up_car(0, dir);	end_car=0;	return;}
		if (end_car == 5)
		{		wake_up_car(1, dir);	end_car=0;	return;}
		if (end_car == 6)
		{		wake_up_car(1, antidir);	end_car=0;	return;}

		if(list_size (&sema_emer_right_side.waiters) == 1 && end_car==1 && !list_empty(&sema_norm_right_side.waiters))
		{		wake_up_car(1, antidir);	end_car=4;	return;}

		if(list_size (&sema_emer_left_side.waiters) > 1 && end_car==1)
		{		wake_up_car(1, dir);	end_car=6;	return;}
		if(list_size (&sema_emer_left_side.waiters) > 0 && end_car==0)
		{		wake_up_car(1, dir);	return;}
		if(list_size (&sema_emer_left_side.waiters) > 0 && end_car==1)
		{		wake_up_car(1, dir);	end_car=5;	return;}

		if(list_size (&sema_emer_left_side.waiters) == 1 && end_car==1 && !list_empty(&sema_norm_left_side.waiters))
		{		wake_up_car(1, dir);	end_car=3;	return;}

		if(list_size (&sema_norm_right_side.waiters) > 1 && end_car==1)
		{		wake_up_car(0, antidir);	end_car=4;	return;}
		if(list_size (&sema_norm_left_side.waiters) > 1 && end_car==1)
		{		wake_up_car(0, dir);	end_car=5;	return;}

		if(!list_empty(&sema_norm_right_side.waiters))
		{		wake_up_car(0, antidir);	end_car=0;	return;}
		wake_up_car(0, dir);	end_car=0;	return;
	}
}


void wake_up_car(enum car_priority prio, enum car_direction dir)
{
	if(prio == 1)
	{
		if(dir == 0)
		{
			sema_up(&sema_emer_left_side);
			return;
		}else
		{
			sema_up(&sema_emer_right_side);
			return;
		}
	}else{
		if(dir == 0)
		{
			sema_up(&sema_norm_left_side);
			return;
		}else
		{
			sema_up(&sema_norm_right_side);
			return;
		}
	}
}

void down_car(enum car_priority prio, enum car_direction dir)
{
	if(prio == 1)
	{
		if(dir == 0)
		{
			sema_down(&sema_emer_left_side);
		}else
		{
			sema_down(&sema_emer_right_side);
		}
	}else{
		if(dir == 0)
			sema_down(&sema_norm_left_side);
		else
		{
			repare++;
			sema_down(&sema_norm_right_side);
		}
	}
}

int try_up_car(int direct)
{
	if(direct == 0)
	{
		if(!list_empty(&sema_emer_left_side.waiters))
			return 1;
		if(!list_empty(&sema_norm_left_side.waiters))
			return 2;
		
		return 0;
	}else{
		if(!list_empty(&sema_emer_right_side.waiters))
			return 1;
		if(!list_empty(&sema_norm_right_side.waiters))
			return 2;

		return 0;
	}	
}