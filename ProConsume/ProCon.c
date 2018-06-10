/*author:   lypto
 * data :   june 6th
 * describe:
 * 这是个实现实现生产者消费者的问题，在对关键区域的操作上采用互斥锁实现,
 * buffer区域数据结构采用循环队列
 * */


#include<stdio.h>
#include<pthread.h>
#include<unistd.h>

#define PRODUCER_NUM   4  //生产者数
#define CONSUMER_NUM   3  //消费者数
#define PRD_NUM        20 //最多生产的产品数
#define DELAY_TIME    
#define QUEUE_SIZE     (PRD_NUM+1) //队列最大容纳QUEUE_SIZE-1个元素


#include "queue.h"

T_QUEUE gtQueue;


//线程锁的定义，静态锁
pthread_mutex_t gtQueLock =PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t gtPrdCond =PTHREAD_COND_INITIALIZER; //Full->Not Full
pthread_cond_t gtCsmCond =PTHREAD_COND_INITIALIZER; //Empty->Not Empty


/*由于生产者函数功能仅仅涉及对关键区的读写，功能比较单一，每个producer或者consumer的功能都一样，不能很好的体现多线程并行处理不同逻辑应用。所以引用slee生成随机数的方法来使得每个线程在执行时间上的相对不同，从而模拟出在多线程并行处理时，在关键区调用互斥锁的情况*/


//生产者函数
void *ProducerThread(void *pvArg)
{
    pthread_detach(pthread_self());
    int dwThrdNo = (int)pvArg;
    while(1)
    {
        pthread_mutex_lock(&gtQueLock);
        while(IsQueFull(&gtQueue))  //队列由满变为非满时，生产者线程被唤醒
            pthread_cond_wait(&gtPrdCond, &gtQueLock);

        EnterQue(&gtQueue, GetQueTail(&gtQueue)); //将队列元素下标作为元素值入队
        if(QueDataNum(&gtQueue) == 1) //当生产者开始产出后，通知(唤醒)消费者线程
            pthread_cond_broadcast(&gtCsmCond);
        printf("[Producer %2u]Current Product Num: %u\n", dwThrdNo, QueDataNum(&gtQueue));

        pthread_mutex_unlock(&gtQueLock);
        sleep(rand()%DELAY_TIME + 1);
    }
}


//消费者生成函数
void *ConsumerThread(void *pvArg)
{
    pthread_detach(pthread_self());
    int dwThrdNo = (int)pvArg;
    while(1)
    {
        pthread_mutex_lock(&gtQueLock);
        while(IsQueEmpty(&gtQueue)) //队列由空变为非空时，消费者线程将被唤醒
            pthread_cond_wait(&gtCsmCond, &gtQueLock);

        if(GetQueHead(&gtQueue) != GetQueHeadData(&gtQueue))
        {
            printf("[Consumer %2u]Product: %d, Expect: %d\n", dwThrdNo,
                   GetQueHead(&gtQueue), GetQueHeadData(&gtQueue));
            exit(0);
        }  
        LeaveQue(&gtQueue);
        if(QueDataNum(&gtQueue) == (PRD_NUM-1)) //当队列由满变为非满时，通知(唤醒)生产者线程
            pthread_cond_broadcast(&gtPrdCond);
        printf("[Consumer %2u]Current Product Num: %u\n", dwThrdNo, QueDataNum(&gtQueue));

        pthread_mutex_unlock(&gtQueLock);
        sleep(rand()%DELAY_TIME + 1);
    }
}


int main(void)
{
    InitQue(&gtQueue);
    srand(10);  //初始化随机函数发生器

    pthread_t aThrd[CONSUMER_NUM+PRODUCER_NUM];
    int dwThrdIdx;
    for(dwThrdIdx = 0; dwThrdIdx < CONSUMER_NUM; dwThrdIdx++)
    {  //创建CONSUMER_NUM个消费者线程，传入(void*)dwThrdIdx作为ConsumerThread的参数
        pthread_create(&aThrd[dwThrdIdx], NULL, ConsumerThread, (void*)dwThrdIdx);
    }
    sleep(2);
    for(dwThrdIdx = 0; dwThrdIdx < PRODUCER_NUM; dwThrdIdx++)
    {
        pthread_create(&aThrd[dwThrdIdx+CONSUMER_NUM], NULL, ProducerThread, (void*)dwThrdIdx);
    }
    while(1);//此处必须使用一个死循环来让producer 和 consumer来进行生产和消费的模拟
    return 0 ;
}



