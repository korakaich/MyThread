/*
Group info:
rkulkar2 Rishkul Kulkarni
kaich Korak Aich
ssharma8 Siddharth Sharma
*/
#ifndef MYTHREAD_H
#include "mythread.h"
#endif


#include <stdio.h>
#include <sys/types.h>
#include <signal.h>
#include <stdlib.h>
//#include <sys_wait.h>
#include <sched.h>
#include <sys/syscall.h>

#include "futex.h"
//#include "mythread.c"

TCB TCBHead = NULL;
TCB TCBTail = NULL;
KNODE keyListHead=NULL;

mythread_key_t keyCounter=0;

RETVALNODE retQHead=NULL;


/*--- Contributed by Korak Aich ---*/
/******************* Code for retQ List Manipulation BEGIN *****************/
int pushToRetQ(RETVALNODE incoming) //push at the end
{
	if(incoming==NULL)
		return 0; //nothing inserted
	if(retQHead==NULL)
	{
		retQHead=incoming;
		return 1;
	}
	RETVALNODE temp=retQHead;
	while(temp->next!=NULL)
		temp=temp->next;
	temp->next=incoming;
	incoming->prev=temp;
	incoming->next=NULL; //just to make sure//////////
	return 1;	  //incoming inserted
}

RETVALNODE findInRetQ(mythread_key_t val)
{
	if (retQHead==NULL)
		return NULL; // no list in place.. so return nothing
	RETVALNODE temp=retQHead;
	while(temp->tid!=val && temp!=NULL )
		temp=temp->next;
	return temp;
}

void printRetQ()
{
	if (retQHead!=NULL)
	{
		RETVALNODE temp=retQHead;
		while(temp!=NULL)
		{
			printf("tid %d \t",temp->tid);
			if(temp->value!=NULL)
				printf("value %d \n",*(int*)temp->value);
			temp=temp->next;
		}
	}
	printf("\n");
}

void freeRetQ()
{
	if(retQHead!=NULL)
	{
		RETVALNODE cur= retQHead;
		RETVALNODE prev=NULL;
		while(cur!=NULL)
		{
			prev=cur;
			cur=cur->next;
			free(cur);
		}
	}
}

/******************* Code for retQ List Manipulation END *****************/
/*--- End of Contributed by Korak Aich ---*/

/*--- Contributed by Rishkul Kulkarni ---*/

// Code to manipulate teh run queue

//function that finds size of run queue
long sizeOfQueue()
{
	long count = 0;
	TCB TCBtemp = TCBHead;
	while(TCBtemp != NULL)
	{
		count++;
		TCBtemp = TCBtemp->next;
	}
	return count;
}

//function to populate run queue
int pushToQueue(TCB incoming)
{
	//printf("Pushing...\n");
	if(TCBHead == NULL)		//first element
	{
		incoming->prev = NULL;
		incoming->next = NULL;
		TCBTail = incoming;
		TCBHead = incoming;
	}
	else
	{
		incoming->next = NULL;
		if(TCBTail->prev != NULL)	
		{
			//TCBTail->prev->next = incoming;
			incoming->prev = TCBTail;
			TCBTail->next = incoming;
		}
		else 
		{
			TCBHead->next = incoming;
			incoming->prev = TCBHead;
			TCBTail->next = incoming;
			
		}
		TCBTail = incoming;
	}
	
	//printf("Pushed and data\n");
	//if(incoming->prev!=NULL)	printf("prev tid = %ld\n",incoming->prev->tid);
	//if(incoming->next!=NULL)	printf("next tid = %ld\n",incoming->next->tid);
	return 1;
}

//Function to remove threads from run queue
TCB popFromQueue()
{
//	printf("Before---- %d\n",sizeOfQueue());
	if(TCBHead == NULL)
		return NULL;
		
	TCB temp = TCBHead;
		
	TCBHead = TCBHead->next;
//	printf("After ---- %d\n",sizeOfQueue());
	return temp;
}

//function to find a TCB in a queue
TCB findInQueue(long id)
{
	TCB TCBQueue = TCBHead;
	while(TCBQueue!=NULL)
	{
		if(TCBQueue->tid == id)
			return TCBQueue;
		TCBQueue = TCBQueue->next;
	}
		//printf("exited while\n");
	return NULL;
}
/*--- End of Contributed by Rishkul Kulkarni ---*/

/******************* Code for Key List Manipulation BEGIN *****************/

/*--- Contributed by Korak Aich ---*/
//function to put stuff in to global key record
int pushToKeyList(KNODE incoming) //push at the end
{
	if(incoming==NULL)
		return 0; //nothing inserted
	if(keyListHead==NULL)
	{
		keyListHead=incoming;
		return 1;
	}
	KNODE temp=keyListHead;
	while(temp->next!=NULL)
		temp=temp->next;
	temp->next=incoming;
	incoming->prev=temp;
	incoming->next=NULL; //just to make sure//////////
	return 1;	  //incoming inserted
}

//function to find presence of a key in system
KNODE findInKeyList(mythread_key_t val)
{
	if (keyListHead==NULL)
		return NULL; // no list in place.. so return nothing
	KNODE temp=keyListHead;
	while(temp->key!=val && temp!=NULL )
		temp=temp->next;
	return temp;
}


//function to remove from global key list
int deleteFromKeyList(mythread_key_t val)
{
	if(keyListHead==NULL)
		return 0;
	KNODE cur=findInKeyList(val);
	if(cur==NULL)
		return 0;
	KNODE prev=cur->prev;
	KNODE next=cur->next;
	if(prev!=NULL)
		prev->next=next;
	else	
	{
		keyListHead=next;
		keyListHead->prev=NULL;
	}
	if(next!=NULL)
		next->prev=prev;
	else
	{
		prev->next=NULL;
	}
	free(cur);
	return 1;
}


//debugging function that prints key list
void printKeyList()
{
	if (keyListHead!=NULL)
	{
		KNODE temp=keyListHead;
		while(temp!=NULL)
		{
			printf(" %d \t",temp->key);
			temp=temp->next;
		}
	}
	printf("\n");

}
/*--- End of Contributed by Korak Aich ---*/

/*--- Contributed by Siddharth Sharma ---*/
/******************* Code for TCB key list Manipulation BEGIN*****************/
//pushing data into key stored in TCB
int pushToLocalKeyList(TCB T, TK incoming) //push at the end
{
	//TK TKHead = T->TKHead;
	//if(T==NULL)	printf("waah\n");
	if(incoming==NULL)
	return 0; //nothing inserted
		
	if(T->TKHead==NULL)
	{
		//printf("inside push = %d\n",*(int*)incoming->value);
		T->TKHead=incoming;
		
		return 1;
	}
	
	TK temp=T->TKHead;
	
	while(temp->next!=NULL)
		temp=temp->next;
	
	temp->next=incoming;
	incoming->prev=temp;
	incoming->next=NULL; //just to make sure//////////
	
	return 1;	  //incoming inserted
}


//finding data in TCB
TK findInLocalKeyList(TCB T, mythread_key_t val)
{
	//printf("find input key = %d\n",val);
	if (T->TKHead==NULL)
		return NULL; // no list in place.. so return nothing

	TK temp=T->TKHead;

	while(temp->key!=val)
	{
		//printf("find input after while key = %d\n",temp->key);
		temp=temp->next;

		if(temp==NULL) 	break;
	}
	
	
	return temp;
}

//function to clear from the TCB
int deleteFromLocalKeyList(TCB T, mythread_key_t val)
{
	if(T->TKHead==NULL)
		return 0;
	TK cur=findInLocalKeyList(TCBHead,val);
	if(cur==NULL)
		return 0;
	TK prev=cur->prev;
	TK next=cur->next;
	if(prev!=NULL)
		prev->next=next;
	else	
	{
		T->TKHead=next;
		T->TKHead->prev=NULL;
	}
	if(next!=NULL)
		next->prev=prev;
	else
	{
		prev->next=NULL;
	}
	free(cur);
	return 1;
}

//debugging -- helped by Korak
void printLocalKeyList(TK TKHead)
{
	if (TKHead!=NULL)
	{
		TK temp=TKHead;
		while(temp!=NULL)
		{
			printf(" %d \t",temp->key);
			temp=temp->next;
		}
	}
	printf("\n");

}

/******************* Code for Local key list Manipulation END************/
/*--- end Contributed by Siddharth Sharma ---*/

//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

//Wrapper function for idle thread - contributed by Rishkul Kulkarni
int IdleFunction(void* some)
{
	TCB in = (TCB) some;
	futex_down(&in->f);
	while(sizeOfQueue()>1)
	{	
		mythread_yield();
	}
	
	return 0;
}

//Wrapper function for a generic thread - contributed by Rishkul Kulkarni
int threadWrapperFunction( void* argument )
{
	TCB in = (TCB) argument;
	
	//write(1,"Yahaan\n",8);
	
	futex_down(&in->f);

	(in->functionPTR)(in->threadARG);

	//if user does not explicitly call exit()
	if(findInQueue(in->tid) != NULL)	mythread_exit(NULL);
      
      return 0;
}

//wrapper to return thread id
static pid_t gettid(void)
{
	return(pid_t) syscall(SYS_gettid);
}

//Function to return thread ID
mythread_t mythread_self(void)
{
	return (mythread_t)gettid();
}

//Function to create new thread - collaborative work on debugging by all members, coding partially contributed for main, idle and generic thread creation

int mythread_create(mythread_t *tid, mythread_attr_t *attr, void* (*threadFunction)(void *), void* arg )
{
	int stck = 0;
	if(attr != NULL)
		stck = *attr;
	else
		stck = SIGSTKSZ;
	
	
	if(sizeOfQueue() == 0)	
	{
		// Making structure for main()----------------------------------------------------main()
			//printf("size of queue 0\n");	
			
			mythread_t pidM = 0;
				
			struct futex tempfM;
			//futex_init(&tempfI,1);							// FUTEX INITIALIZATION  0 because initialize with blocking
			tempfM.count = 0;
				
			TCB tempTCBM = (TCB)malloc(sizeof(struct threadControlBlock));
			tempTCBM->stackPTR = NULL;
			tempTCBM->f = tempfM;
			tempTCBM->functionPTR = NULL;
			tempTCBM->threadARG = NULL;
			tempTCBM->TKHead = NULL;
	
			pushToQueue(tempTCBM);
			//------------------TODO------------------------------
			//store tid in TCB structure      
			//store stackTop in TCB structure.. this is to be freed in the exit function..
			//store other stuff if required
		
			*tid=pidM; //set the tid of the thread.
			tempTCBM->tid = *tid;
			//printf("clone1 execute hua\n");
		
		// Done making structure for main()----------------------------------------------------main()
		//printf("size of queue %ld\n",sizeOfQueue());	
		void* stackI;
		mythread_t pidI;
	
		void* stckaddrI = &stackI;
		posix_memalign(stckaddrI, 64, stck);
	
	//stackI=malloc(MAX_STACK_SIZE);	
		//if ( stackI == 0 )
		//{
		 //     printf( "malloc---: could not allocate stack \n" );
		 //     exit( 1 );
		//} 
		//else
		//{
		
			//printf( "hmmm ---------------- \n" );
		//}
	
		struct futex tempfI;
		//futex_init(&tempfI,1);							// FUTEX INITIALIZATION  1 because initialize with blocking
		tempfI.count = 0;
	
		char* stackTopI=(char* ) stackI + stck - sizeof(sigset_t);
			
		TCB tempTCBI = (TCB)malloc(sizeof(struct threadControlBlock));
		tempTCBI->stackPTR = stackI;
		tempTCBI->f = tempfI;
		tempTCBI->functionPTR = (void*)&(IdleFunction);
		tempTCBI->threadARG = NULL;
		tempTCBI->TKHead = NULL;
	
		pushToQueue(tempTCBI);
		
		//------------------TODO------------------------------
		//store tid in TCB structure      
		//store stackTop in TCB structure.. this is to be freed in the exit function..
		//store other stuff if required
		
		//printf( "Creating child thread\n" ); //debug_del
		pidI=clone(&IdleFunction, stackTopI , CLONE_FS | CLONE_FILES | CLONE_VM, (void*)tempTCBI );
		pidI=200;
		if ( pidI == -1 )
		{
		       	printf( "clone\n" );
			exit( 2 );
	      	}

	      
	      	//futex_down(&(tempTCBI->f));
	      
		*tid=pidI; //set the tid of the thread.
		tempTCBI->tid = *tid;
		//printf("clone1 execute hua\n");
		
	}
	
	void* stack;
	mythread_t pid;
	//stack=malloc(MAX_STACK_SIZE);	
	
	void* stckaddr = &stack;
	posix_memalign(stckaddr, 64, stck);
	//if ( stack == 0 )
	//{
          //    printf( "malloc: could not allocate stack \n" );
            //  exit( 1 );
	//}
		
	struct futex tempf;
	//futex_init(&tempf,5);							// FUTEX INITIALIZATION  1 because initialize with blocking
	tempf.count = 0;
	
	char* stackTop=(char* ) stack+ stck - sizeof(sigset_t);
	//arg=threadFunction;
	
	TCB tempTCB = (TCB)malloc(sizeof(struct threadControlBlock));
	tempTCB->stackPTR = stack;
	tempTCB->f = tempf;
	tempTCB->functionPTR = threadFunction;
	tempTCB->threadARG = arg;
	//printf("size of queue %ld\n",*((int*)arg));
	tempTCB->TKHead = NULL;
	//write(1,"thread arg outside TCB--------------------- =  %d\n",50);//*(int*)arg);
	//write(1,"thread arg in TCB--------------------- =  %d\n",50);//*(int*)(tempTCB->threadARG));
	pushToQueue(tempTCB);
	//printf("size of queue %ld\n",sizeOfQueue());
      //------------------TODO------------------------------
	  //store tid in TCB structure      
	  //store stackTop in TCB structure.. this is to be freed in the exit function..
	  //store other stuff if required
	void* tptr = (void*)	tempTCB;
      //printf( "Creating child thread\n" ); //debug_del
     
     
      tempTCB->tid = 66;
     
      pid=clone(&threadWrapperFunction, stackTop ,CLONE_FS | CLONE_FILES | CLONE_VM, tptr);
      if ( pid == -1 )
      {
               //printf( "clone\n" );
               exit( 2 );
      }
      //printf("clone execute hua\n");
      //write(1," id in threadcreat - %d\n",50);//pid);
      *tid=pid; //set the tid of the thread.
      tempTCB->tid = *tid;
      //sleep(5000);
      //while(1){}
      return *tid; 
}   


//Function to implement joining of thread - contributed by Rishkul Kulkarni
int mythread_join(mythread_t tid, void** status)
{
	
	while(findInQueue(tid)!=NULL)
	{
		mythread_yield();
	}
	
	RETVALNODE temp=findInRetQ(tid);
	if(temp==NULL)
		return 0;
	else
	{
		status=&temp->value;
	}
	//printf("some: %d\n",*(int*)*status);
	return 1;
}

//Function to implement yield mechanism - contributed by Korak Aich
int mythread_yield(void)
{
	TCB curr = popFromQueue();

//	write(1,"yeild\n",7);

	pushToQueue(curr);
	
	futex_up(&TCBHead->f);

	if(curr->f.count>1){}//write(1,"pangaa\n",8);

	futex_down(&curr->f);	

	return 1;
}
/*
void mythread_exit(void* retval)
{
	
	if(TCBHead->tid == 0)
	{
		while(sizeOfQueue() > 2)
		{
			mythread_yield();
		}
		exit(0);
	}
	
	TCB curr = popFromQueue();
	free(curr);	
	futex_up(&TCBHead->f);
	
	//return 1;
}
*/

//function to handle exit conditions - contributed by Siddharth Sharma
void mythread_exit(void* retval)
{
	
	//handle special case for main thread	
	if(TCBHead->tid == 0)
	{
		while(sizeOfQueue() > 2)
		{
			mythread_yield();
		}
		
		printRetQ();//debug .. to be removed
		exit(0);
	}
	
	
	TCB curr = popFromQueue();
	RETVALNODE temp;
	temp=(RETVALNODE)malloc(sizeof(struct retVals));
	temp->tid=curr->tid;
	temp->value=retval;
	temp->next=NULL;
	temp->prev=NULL;
	pushToRetQ(temp);
	
	free(curr);	
	futex_up(&TCBHead->f);
	
	//return 1;
}

//Function to create a key - contributed by Rishkul Kulkarni
int mythread_key_create(mythread_key_t *key, void (*destructor)(void*))
{
	KNODE temp = (KNODE)malloc(sizeof(struct keyNode));
	temp -> key = keyCounter++;
	temp -> next = NULL;
	temp -> prev = NULL;
	temp -> desPTR = destructor;
	*key = temp -> key;
	pushToKeyList(temp);	

	return 1;
}
        
//Function to delete a key - contributed by Rishkul Kulkarni        
int mythread_key_delete(mythread_key_t key)
{
	
	if(findInKeyList(key) == NULL)		return -1;
	
	TCB trav = TCBHead;
	//(desPTR)((void*)trav->key);
	
	while(trav != NULL)
	{
		deleteFromLocalKeyList(trav, key);
		trav = trav -> next;
	}
	
	
	
	return 1;
}

//Function to get specific data from key - Siddharth Sharma
void *mythread_getspecific(mythread_key_t key)
{
	if(findInKeyList(key) != NULL)
	{
		TK probable = findInLocalKeyList(TCBHead,key);
		 
		if(probable== NULL)
		{	
			TK temp = (TK)malloc(sizeof(struct threadKey));
			temp -> key = key;
			temp -> value = NULL;
			temp -> next = NULL;
			temp -> prev = NULL;

			pushToLocalKeyList(TCBHead,temp);

			return NULL;
		}
		else
		{
			//printf("Local mein mili\n");					
			const int *tmpPTR = (int*)probable->value;
			void *retval = (void*)&(*tmpPTR);
			
			return(retval);
		}
	}
	else
	{
		//printf("Global mein nahi mili\n");			
		return NULL;
	}
}

//function to set specific data for a key - contributed by Siddharth Sharma and Korak Aich in collaborative
int mythread_setspecific(mythread_key_t key, const void *value)	// *#*#*#*#*#*#*##**#*#**#*##* There was a const qualifier to void *value
{
	//printf("TID From sys = %d\n",mythread_getself());
	if(findInKeyList(key) != NULL)
	{
	
		TK probable = findInLocalKeyList(TCBHead,key);
		 
		if(probable== NULL)
		{	
			TK temp = (TK)malloc(sizeof(struct threadKey));
			temp -> key = key;
			temp -> value = value;
			temp -> next = NULL;
			temp -> prev = NULL;

			pushToLocalKeyList(TCBHead,temp);
			//TCBHead->TKHead = temp;
			//printf("inside setspecific = %d\n",*(int*)TCBHead->TKHead->value);

			if(TCBHead->TKHead == NULL){}//	printf("fataa !!\n");
			

			
		}
		else
		{
			//printf("Local waale mein mil rahi\n");			
			probable -> value = value;			
		}
	}
	else
	{
		//printf("Nahi mil rahi\n");
	}
	
	return 1;
}



// Debugging for TCB - collaborative
void printTCB()
{

        TCB print = TCBHead;
        while(print!=NULL)
        {
        	printf("TID: %d F: %d\n",print->tid,print->f.count);
        	
        	print=print->next;	
        }

}
