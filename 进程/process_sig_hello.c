#include <unistd.h>
#include <signal.h>
#include <stdio.h>

void handler()
{
	printf("signal trigged\n");
}

int main(int argc, char* argv[])
{
	int i ;
	signal(SIGALRM, handler);
	alarm(5);
	
	for(i=1;i<7;i++){
		printf("sleep %d....\n",i);
		sleep(1);
	}
	
	return 0;
}
