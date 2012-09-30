/*
Group info:
rkulkar2 Rishkul Kulkarni
kaich Korak Aich
ssharma8 Siddharth Sharma
*/

/*
 * mythread.h -- interface of user threads library
 */

#ifndef MYTHREAD_H
#define MYTHREAD_H

#ifndef FALSE
#define FALSE 0
#endif

#ifndef TRUE
#define TRUE 1
#endif


#include <sys/types.h>
#include "futex.h"

#define MAX_STACK_SIZE 1024*64

/* Keys for thread-specific data */
typedef unsigned int mythread_key_t;
typedef unsigned int mythread_t;
typedef unsigned int mythread_attr_t;
/* add your code here */


//struct for storing return values from exit//
struct retVals
{
	mythread_key_t tid;
	void *value;
	struct retVals * next;
	struct retVals * prev;
};
typedef struct retVals* RETVALNODE;

//functions...
int pushToRetQ(RETVALNODE);
RETVALNODE popFromRetQ();
RETVALNODE findInRetQ();
void printRetQ();


//struct for Thread Control Block
struct threadControlBlock
{
	mythread_t tid;
	struct futex f;
	void* stackPTR;
	void* (*functionPTR)(void*);
	void* threadARG;
	struct threadControlBlock* next;
	struct threadControlBlock* prev;
	struct threadKey* TKHead;
};
typedef struct threadControlBlock* TCB; 




//function to push, pop and find TCB in queue
int pushToQueue(TCB);
TCB popFromQueue();
TCB findInQueue(long);


//struct for storing key list node

struct keyNode
{
	mythread_key_t key;
	void* desPTR;
	struct keyNode *next;
	struct keyNode *prev;
};
typedef struct keyNode* KNODE;


//functions for key list 
int pushToKeyList(KNODE);
int deleteFromKeyList(mythread_key_t);
KNODE findInKeyList(mythread_key_t);


//



// LOCAL KEY LIST

struct threadKey
{
       mythread_key_t key;
       const void* value;
       struct threadKey* next;
       struct threadKey* prev;
};
typedef struct threadKey* TK;


//functions
int pushToLocalKeyList(TCB, TK);
int deleteFromLocalKeyList(TCB, mythread_key_t);
TK findInLocalKeyList(TCB, mythread_key_t);
void printLocalKeyList();

//--------------------------------
//-------------------------end of add your code here ----------------
/*
 * mythread_self - thread id of running thread
 */
mythread_t mythread_self(void);

/*
 * mythread_create - prepares context of new_thread_ID as start_func(arg),
 * attr is ignored right now.
 * Threads are activated (run) according to the number of available LWPs
 * or are marked as ready.
 */
int mythread_create(mythread_t *new_thread_ID,
		    mythread_attr_t *attr,
		    void * (*start_func)(void *),
		    void *arg);

/*
 * mythread_yield - switch from running thread to the next ready one
 */
int mythread_yield(void);

/*
 * mythread_join - suspend calling thread if target_thread has not finished,
 * enqueue on the join Q of the target thread, then dispatch ready thread;
 * once target_thread finishes, it activates the calling thread / marks it
 * as ready.
 */
int mythread_join(mythread_t target_thread, void **status);

/*
 * mythread_exit - exit thread, awakes joiners on return
 * from thread_func and dequeue itself from run Q before dispatching run->next
 */
void mythread_exit(void *retval);

/*
 * mythread_key_create - thread-specific data key creation
 * The  mythread_key_create()  function shall create a thread-specific data
 * key visible to all threads in  the  process.  Key  values  provided  by
 * mythread_key_create()  are opaque objects used to locate thread-specific
 * data. Although the same key value may be used by different threads, the
 * values  bound  to  the key by mythread_setspecific() are maintained on a
 *per-thread basis and persist for the life of the calling thread.
 */
int mythread_key_create(mythread_key_t *key, void (*destructor)(void*));

/*
 * mythread_key_delete - thread-specific data key deletion
 * The  mythread_key_delete()  function shall delete a thread-specific data
 * key previously returned by  mythread_key_create().  The  thread-specific
 * data  values  associated  with  key  need  not  be  NULL  at  the  time
 * mythread_key_delete() is called.  It is the responsibility of the appli‐
 * cation  to  free any application storage or perform any cleanup actions
 * for data structures related to the deleted key  or  associated  thread-
 * specific data in any threads; this cleanup can be done either before or
 * after mythread_key_delete() is called. Any attempt to use key  following
 * the call to mythread_key_delete() results in undefined behavior.
 */
int mythread_key_delete(mythread_key_t key);

/*
 * mythread_getspecific - thread-specific data management
 * The mythread_getspecific() function shall  return  the  value  currently
 * bound to the specified key on behalf of the calling thread.
 */
void *mythread_getspecific(mythread_key_t key);

/*
 * mythread_setspecific - thread-specific data management
 * The  mythread_setspecific()  function  shall associate a thread-specific
 * value with a key obtained via a previous call to  mythread_key_create().
 * Different threads may bind different values to the same key. These val‐
 * ues are typically pointers to blocks of  dynamically  allocated  memory
 *that have been reserved for use by the calling thread.
 */
int mythread_setspecific(mythread_key_t key, const void *value);


#endif /* MYTHREAD_H */
