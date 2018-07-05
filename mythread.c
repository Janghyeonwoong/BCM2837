#include <ucontext.h>
#include <signal.h>
#include <sys/time.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "mythread.h"

static enum mythread_scheduling_policy sched_policy = MYTHREAD_RR;

ucontext_t main_thread;

// Thread control block
struct tcb {
  // TODO: Fill your own code
  ucontext_t *context;
 struct tcb *next; // i will use linked list for searching;
 struct tcb *prev;
  int tid;
  int finished; // 0이면 끝나지 않음, 1이면 끝남.
};

struct tcbmanage{
  struct tcb * head;
  struct tcb * tail;
}tcblist;

int lock = 0;    // i will use lock system
static int stacked_tcb = 0;
static struct tcb* tcbs[1024];
static int n_tcbs = 0;
static int current_thread_id = 0;
static int next_thread_id = 0;
static struct itimerval time_quantum;
static struct sigaction ticker;

static int nextTcb() {
	int count = 0;
  if (sched_policy == MYTHREAD_RR) {
/* we should search active context */
//		printf("error?\n");
//		(tcbs[current_thread_id]->prev)->next = tcbs[current_thread_id] -> next;
		if(tcbs[current_thread_id]->next == NULL)
		{
			next_thread_id = 0;
			//printf("once !!! \n");
			return 0;
		}
		else
		{
			next_thread_id = tcbs[current_thread_id] -> next -> tid;
			return next_thread_id;
			
		}
	// TODO: Fill your own code
  } else if (sched_policy == MYTHREAD_FIFO) {
		for(int i =current_thread_id + 1; i< stacked_tcb; i++)
		{
		
			if(tcbs[i]->finished == 0)
				return i;
			
		}

		return 0; /* there are not thread */

    // TODO: Fill your own code
  }
}

static void tick(int sig) {
  // TODO: Implement your own code
//	printf("tick!\n");
	int temp;
		if( lock == 0 )
	{
		if(n_tcbs == 1)
		{
//			printf("Do you see that????\n");
			return;
		}
		next_thread_id = nextTcb();
//		printf("current id = %d\n", current_thread_id);
//		printf("current-> = %d\n", next_thread_id );
		temp = next_thread_id;
		next_thread_id = current_thread_id;
		current_thread_id = temp;
// change value

		swapcontext( tcbs[next_thread_id] -> context ,
		 tcbs[current_thread_id] -> context);
	}
	else if(lock == 1 )
	{
		printf("synchronization");
	}
  // HINT: 이 함수에서 nextTcb() 함수를 통해 scheduling 하고, 해당 schedule 함수를 실행.
}

void mythread_init(enum mythread_scheduling_policy policy)
{
	sched_policy = policy;
	/* make a main thread */
	tcbs[0] = (struct tcb *)malloc(sizeof(struct tcb));
	tcbs[0]->context = &main_thread;
	tcbs[0]->tid = 0;
	tcbs[0]->finished = 0;
	tcbs[0]->next = NULL;
	tcbs[0]->prev = NULL;
	tcblist.head = tcbs[0];
	tcblist.tail = tcbs[0];
	n_tcbs = 1;

	stacked_tcb++;
  	// TODO: Implement your own code
	if(policy == MYTHREAD_RR) // if we use RR , should set the timer
{
        /* Install timer_handler as the signal handler for SIGVTALRM. */
        memset (&ticker, 0, sizeof (ticker));
        ticker.sa_handler = &tick;
        sigaction (SIGVTALRM, &ticker, NULL);

        /* Configure the timer to expire after 1 sec... */
        time_quantum.it_value.tv_sec = 1;
        time_quantum.it_value.tv_usec =0;

        /* ... and every 100 usec after that. */
        time_quantum.it_interval.tv_sec = 0;
        time_quantum.it_interval.tv_usec = 100;

        /* Start a virtual timer. It counts down whenever this process is executing. */
        setitimer (ITIMER_VIRTUAL, &time_quantum, NULL);

}  

  // HINT: 이 함수에서 tick 에 대해서 timer signal를...
}

int mythread_create(void (*stub)(void*), void* args)
{
	// Thread ID
	lock = 1; // Use lock for synchronization

	n_tcbs++;
//	stacked_tcb++;
	tcbs[stacked_tcb] = (struct tcb *) malloc( sizeof(struct tcb) );
	tcbs[stacked_tcb]->context = (ucontext_t *)malloc(sizeof(ucontext_t));

	getcontext(tcbs[stacked_tcb]->context);
	tcbs[stacked_tcb] -> tid = stacked_tcb;
	tcbs[stacked_tcb] -> context->uc_stack.ss_sp = (char *)malloc(4096);
	tcbs[stacked_tcb] -> context->uc_stack.ss_size = 4096;
	(tcblist.tail) -> next = tcbs[stacked_tcb];
	tcbs[stacked_tcb]-> prev = tcblist.tail;
	tcblist.tail = tcbs[stacked_tcb];
	tcbs[stacked_tcb] -> next = NULL;
//	printf("Tail's pid = %d\n", (tcblist.tail) -> tid);
	makecontext(tcbs[stacked_tcb] -> context , (void (*)(void)) mythread_execute ,
				   	3 , stub , args, tcbs[stacked_tcb] -> tid);
	stacked_tcb++;	
	lock = 0; // release lock;

  // TODO: Implement your own code

  return tcbs[stacked_tcb - 1]->tid;
}
void mythread_execute(void(*stub)(void*) ,void *args, int tid)
{
	int next;
	current_thread_id = tid;
	(stub)(args);
	
	lock = 1; // Use lock for synchronization
	n_tcbs--;
	tcbs[tid]->finished = 1;
	if(sched_policy == MYTHREAD_RR)
	{
//	  printf("asdf\n");
	  next = nextTcb();
//	  printf("next = %d\n",next);
	  /* link relocate */
	  if(tcbs[current_thread_id]->next != NULL)
	  {
	 	 (tcbs[current_thread_id]->prev)->next = tcbs[current_thread_id] -> next;
	 	 (tcbs[current_thread_id]->next)->prev = tcbs[current_thread_id] -> prev;
	  }
	  else
	  {
	 	 (tcbs[current_thread_id]->prev)->next = tcbs[current_thread_id] -> next;

	  }
	  current_thread_id = next_thread_id;
	 
	  lock = 0;


//	  printf("next = %d tid = %d\n",next,tid);
	  
	  swapcontext(tcbs[tid]->context, tcbs[next]->context );
	
	}
	else if(sched_policy == MYTHREAD_FIFO)
	{
		current_thread_id = 0;
		swapcontext(tcbs[tid]->context , &main_thread);  
		/* first is main_thread */
	}


}

int mythread_search(int tid)
{
	for(int i=0; i <stacked_tcb; i++)
	{
		if(tcbs[i] -> tid == tid)
			return 1; 
			/* if find tid, return 1; */
	}

	return 0;
	/* if you can't find tid return 0; */

}

void mythread_join(int tid)
{
	int next;
	int master_tid;
	master_tid = current_thread_id;
//	printf("master tid = %d\n",master_tid);	
	if(mythread_search(tid) == 0 )
	{
		printf("there isn't tcbs that have a tid\n");
		return;
	}


  // TODO: Implement your own code
if(sched_policy == MYTHREAD_RR)
	{
		while(1)
		{
			if(tcbs[tid]->finished == 1)
					break;
		}
//		printf("waiting for %d\n", tid);
	}
else
{
	while(1)
	{
		if(tcbs[tid]->finished == 1)
				break;
		current_thread_id = master_tid;
		next = nextTcb();
			
		swapcontext(tcbs[master_tid] -> context, tcbs[next]->context);

	}
//		printf("waiting for %d\n", tid);


}



}

