#include <pthread.h>

void *funcThreadA(void)
{
	int a_loop;
	for(a_loop=1;a_loop<=20;a_loop++)
	{
		printf("A\n");
		sleep(1);
	}
	return;
}

void *funcThreadB(void)
{
	int b_loop;
	for(b_loop=1;b_loop<=10;b_loop++)
	{
		printf("B\n");
		sleep(2);
	}
	return;
}


int main(void)
{
	pthread_t threadA;
	pthread_t threadB;
	
	printf("Main program starting\n");
	pthread_create(&threadA,NULL,&funcThreadA,NULL);
	pthread_create(&threadB,NULL,&funcThreadB,NULL);
	printf("Main program waiting\n");
	pthread_join(threadA,NULL);
	pthread_join(threadB,NULL);
	printf("Main program ending\n");
	return 0;
}