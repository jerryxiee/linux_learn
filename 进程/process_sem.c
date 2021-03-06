#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/stat.h>  

union semun {
   int              val;    /* Value for SETVAL */
   struct semid_ds *buf;    /* Buffer for IPC_STAT, IPC_SET */
   unsigned short  *array;  /* Array for GETALL, SETALL */
   // struct seminfo  *__buf;  /* Buffer for IPC_INFO
							   // (Linux-specific) */
};

static int sem_id = 0;

//配置信号量的值
static int set_semvalue();
//删除信号量的值
static void del_semvalue();
//等待信号
static int semaphore_p();
//发送信号
static int semaphore_v();

int main(int argc, char* argv[])
{
	char message = 'X';
	int i = 0;
	
	//创建信号量
	sem_id = semget((key_t)1234, 1, 0666 | IPC_CREAT);
	
	if(argc > 1)
	{
		//程序第一次被调用，初始化信号量
		if(!set_semvalue())
        {  
            fprintf(stderr, "Failed to initialize semaphore\n");  
            exit(EXIT_FAILURE);  
        }
		//设置要输出的信息，即参数的第一个字符
		message = argv[1][0];
		sleep(2);//休眠2s
	}
	
	for(i = 0; i < 10; i++)
	{
		//进入临界区
		if(!semaphore_p())
			exit(EXIT_FAILURE);
		//向屏幕输出数据
		printf("%c", message);
		//清理缓冲区，然后休眠随机时间
		fflush(stdout);
		sleep(rand() % 3);
		//离开临界区前再输出一次数据
		printf(".%c", message);
		fflush(stdout);
        //离开临界区，休眠随机时间后继续循环  
        if(!semaphore_v())  
            exit(EXIT_FAILURE);  
        sleep(rand() % 2);  
	}
	
	sleep(10);
    printf("\n%d - finished\n", getpid());
	
    if(argc > 1)
    {
        //如果程序是第一次被调用，则在退出前删除信号量  
        sleep(3);  
        del_semvalue();  
    }
    exit(EXIT_SUCCESS);
	
	return 0;
}

static int set_semvalue(void)
{
	//初始化信号量
	union semun sem_union;
    sem_union.val = 1;  

	if(semctl(sem_id, 0, SETVAL, sem_union) == -1)
		return 0;
	return 1;
}

static void del_semvalue(void)
{
	//删除信号量
	union semun sem_union;
	
	if(semctl(sem_id, 0, IPC_RMID, sem_union) == -1)
		fprintf(stderr, "Failed to delete semaphore\n");
}

static int semaphore_p(void)
{
	//对信号量进行减1操作，即等待信号
	struct sembuf sem_b;
	sem_b.sem_num = 0;//操作第0个信号量
	sem_b.sem_op = -1;//等待信号
	sem_b.sem_flg = SEM_UNDO;
	
    if(semop(sem_id, &sem_b, 1) == -1)  
    {  
        fprintf(stderr, "semaphore_p failed\n");  
        return 0;  
    }  
    return 1;  
}

static int semaphore_v(void)
{
	//对信号量进行加1操作，释放信号，使信号量变为可用，即发送信号
	struct sembuf sem_b;
	sem_b.sem_num = 0;//操作第0个信号量
	sem_b.sem_op = 1;//发送信号
	sem_b.sem_flg = SEM_UNDO;
	
    if(semop(sem_id, &sem_b, 1) == -1)  
    {  
        fprintf(stderr, "semaphore_v failed\n");  
        return 0;  
    }  
    return 1;  
}
