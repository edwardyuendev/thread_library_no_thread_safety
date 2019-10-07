// User Mode Thread Library Project -- without thread synchronization
// By Edward Yuen | edwardyuensf@gmail.com
// Finished April 2019

// Goal was to implement a basic thread library for Linux, to provide applications with the ability to
// create and start new threads
// For the project, I simply aimed to implement core functions from pthread.h myself.
// We store thread information in a Thread Control Block which is stored in a list that is cycled through with a scheduler
// We used setjmp and longjmp to switch contexts between different threads.

#include <pthread.h>
#include <setjmp.h>
#include <iostream>
#include <vector>
#include <string>
#include <setjmp.h>
#include <signal.h>
#include <unistd.h>
#include <stdio.h>

using namespace std;

// Global Variables
static pthread_t nextAvailableID = 0;
static int currentThread = 0;
static bool pthread_init = false;
const string RUNNABLE = "RUNNABLE";
const string EXITED = "EXITED";
static int numcalls = 0;

static long int i64_ptr_mangle(long int p)
{
	long int ret;
    asm(" mov %1, %%rax;\n"
            " xor %%fs:0x30, %%rax;"
            " rol $0x11, %%rax;"
            " mov %%rax, %0;"
        : "=r"(ret)
        : "r"(p)
        : "%rax"
        );
    return ret;
}

struct TCB{
	pthread_t thread_ID;
	jmp_buf context;
	string status;
	void *(*start_routine)(void *);
	void *arg;
	char *stack;
};

static vector<TCB*> table;

void scheduler(int signal){
	// saves information about current thread
	// if a thread longjmp's to a previously saved thread, it will just return
	if (setjmp((table.at(currentThread))->context) == 0){
		currentThread--;
		if (currentThread < 0){
			currentThread = table.size()-1;
		}
		// longjmp's to the next available thread
		longjmp((table.at(currentThread))->context, 1);
	}else{
		return;
	}


}

void timer(){
	// sets up the sigaction struct, points the handler to our scheduler
	struct sigaction sigact;
	sigemptyset(&sigact.sa_mask);
	sigact.sa_handler = scheduler;
	sigact.sa_flags = SA_NODEFER;
	sigaction(SIGALRM, &sigact, NULL);

	// sets up t he ualarm timer, sends SIGALRM every 50 ms
	ualarm(50000,50000);
}

void wrapper(){
	// grabs the current thread's start_routine and arguments
	void *(*start_routine)(void *) = (table.at(currentThread)->start_routine);
	void *arg = (table.at(currentThread))->arg;

	// executes the function passed into pthread_create and then exits
	start_routine(arg);
	pthread_exit(NULL);
}

int pthread_create(pthread_t *thread, const pthread_attr_t *attr, void *(*start_routine)(void *), void *arg){
	if(!pthread_init){

	  pthread_init = true;
	  TCB* mainTCB = new TCB();
	  mainTCB->thread_ID = nextAvailableID;
	  mainTCB->status = RUNNABLE;
	  mainTCB->start_routine = start_routine;
	  mainTCB->arg = arg;
	  mainTCB->stack = NULL;
	  nextAvailableID++;
	  setjmp(mainTCB->context);
	  table.push_back(mainTCB);
	  timer();

	}
	// Assign the next available thread ID to *thread, and create a new TCB with that new threadID/RUNNING/start_routine/arg
	*thread = nextAvailableID;
	void* bottomOfStack = (void*)malloc(32767);
	void* topOfStack = (char*)(bottomOfStack)+32767-21;

	TCB* tempTCB = new TCB();
	
	tempTCB->thread_ID = nextAvailableID;
	tempTCB->status = RUNNABLE;
	tempTCB->start_routine = start_routine;
	tempTCB->arg = arg;
	tempTCB->stack = (char*)bottomOfStack;

	setjmp(tempTCB->context);
	*((unsigned long *)&(tempTCB->context)+6) = i64_ptr_mangle((long int)((long int*)topOfStack));
	*((unsigned long *)&(tempTCB->context)+7) = i64_ptr_mangle((long int)&wrapper);
	table.push_back(tempTCB);

	// Increment the nextAvailableThreadID
	nextAvailableID++;
	return 0;


}

void pthread_exit(void *value_ptr){
	// deletes/frees memory of thread that called pthread_exit
	// then deletes it from the global vector
  if (table.size() > 2)
  	free((table.at(currentThread))->stack); 
  delete(table.at(currentThread)); 
  table.erase(table.begin()+currentThread);
  
  // longjmp to the next available thread
  currentThread--;
  if (currentThread < 0)
    currentThread = table.size()-1;
  longjmp((table.at(currentThread))->context, 1);
  
}

pthread_t pthread_self(void){
  // return the "current" thread's ID
  return (pthread_t)(table.at(currentThread))->thread_ID;
}
