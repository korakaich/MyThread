/*
Group info:
rkulkar2 Rishkul Kulkarni
kaich Korak Aich
ssharma8 Siddharth Sharma
*/
/* Main program to test functionality of the library*/

#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include"mythread.h"

mythread_key_t key1, key2, key3, key4, key5; 
int aa = 55;
void* threadFunction1(void* argument)
{
	char* a = (char*)argument;
	
	write(1,a,strlen(a)+1);
	
	int i = 10;
	
	mythread_key_create(&key2, NULL);
	mythread_setspecific(key2,(void*)&i);
	printf("Thread 1-2(10) = %d\n",*(int*)mythread_getspecific(key2));
	
	i=20;
	mythread_yield();

	mythread_setspecific(key2,(void*)&i);
	printf("Thread 1-2 = %d\n",*(int*)mythread_getspecific(key2));
        
	printf("Thread 1-1 = %d\n",(int*)mythread_getspecific(key1));

	return 0;
}

void* threadFunction3(void* argument)
{
	char* a = (char*)argument;
	
	write(1,a,strlen(a)+1);
	
	int i = 40;
	int i1 = 70;
	
	
	mythread_key_create(&key4, NULL);
	mythread_key_create(&key5, NULL);
	mythread_setspecific(key4,(void*)&i);
	mythread_setspecific(key5,(void*)&i1);
	
	printf("Thread 3-1 = %d\n",(int*)mythread_getspecific(key1));
	printf("Thread 3-2 = %d\n",(int*)mythread_getspecific(key2));
	printf("Thread 3-3 = %d\n",(int*)mythread_getspecific(key3));
	printf("Thread 3-4 = %d\n",*(int*)mythread_getspecific(key4));
	printf("Thread 3-5 = %d\n",*(int*)mythread_getspecific(key5));
	mythread_key_delete(key4);
	int aaa = 77;
	if(1)
		mythread_exit(&aaa);
	else
		return 0;
}


void* threadFunction2(void* argument)
{
	mythread_t tid3;
	char* a = (char*)argument;
	write(1,a,strlen(a));
	//printf("Thread argu in threadFn %d\n",*a);
	
	int i = 30;
	
	mythread_setspecific(key1,(void*)&i);
	printf("Thread 2-1 (30)= %d\n",*(int*)mythread_getspecific(key1));
	
    mythread_key_create(&key3,NULL);
    mythread_setspecific(key3,(void*)&i);    
    printf("Thread 2-3 (30)= %d\n",*(int*)mythread_getspecific(key3));
    
    //create new thread tid3
    char *temp="kein";
    tid3=mythread_create(&tid3, NULL, &threadFunction3,(void*)temp);
	//mythread_yield();
	int ts = 88;
	int* st=&ts;
	mythread_join(tid3,(void**)st);
	
	i=50;
	mythread_setspecific(key2,(void*)&i);
	printf("Thread 2-2(50) = %d\n",*(int*)mythread_getspecific(key2));
	printf("something %d\n",*(int*)st);
	
	return 0;
}
// main calls t1, t2.. t2 calls t3

int main()
{
        
        mythread_t tid1, tid2;

        int keyint = 1000;
        
        char* ab = "Heute"; 
        void* cd;
        char* xy="futball";
        tid1=mythread_create(&tid1, NULL, &threadFunction1,(void*)ab);
        cd = xy;
        tid2=mythread_create(&tid2, NULL, &threadFunction2,cd);


	mythread_key_create(&key1, NULL);
	mythread_setspecific(key1,(void*)&keyint);
        mythread_exit(NULL);
	printf("Main Key = %d\n",*(int*)mythread_getspecific(key1));


		return 0;
}

