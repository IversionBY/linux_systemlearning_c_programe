#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include<sys/types.h>
#include<sys/stat.h>
#include<unistd.h>
#include<fcntl.h>
//#define QUEUE_SIZE 20


typedef struct{
    int aData[QUEUE_SIZE];
    int dwHead;
    int dwTail;
}T_QUEUE,*PT_QUEUE;


//初始化循环队列
void InitQue(PT_QUEUE ptQue)
{
    memset(ptQue, 0, sizeof(*ptQue));
}

//向循环队列中插入元素
void EnterQue(PT_QUEUE ptQue, int dwElem)
{
    if(IsQueFull(ptQue))
    {
        printf("Elem %d cannot enter Queue %p(Full)!\n", dwElem, ptQue);
        return;
    }
    ptQue->aData[ptQue->dwTail]= dwElem;
    ptQue->dwTail = (ptQue->dwTail + 1) % QUEUE_SIZE;
}

//从循环队列中取出元素
int LeaveQue(PT_QUEUE ptQue)
{
    if(IsQueEmpty(ptQue))
    {
        printf("Queue %p is Empty!\n", ptQue);
        return -1;
    }
    int dwElem = ptQue->aData[ptQue->dwHead];
    ptQue->dwHead = (ptQue->dwHead + 1) % QUEUE_SIZE;
    return dwElem;
}

//从队首至队尾依次显示队中元素值
void DisplayQue(PT_QUEUE ptQue)
{
    if(IsQueEmpty(ptQue))
    {
        printf("Queue %p is Empty!\n", ptQue);
        return;
    }

    printf("Queue Element: ");
    int dwIdx = ptQue->dwHead;
    while((dwIdx % QUEUE_SIZE) != ptQue->dwTail)
        printf("%d ", ptQue->aData[(dwIdx++) % QUEUE_SIZE]);

    printf("\n");
};

//判断循环队列是否为空
int IsQueEmpty(PT_QUEUE ptQue)
{
    return ptQue->dwHead == ptQue->dwTail;
}

//判断循环队列是否为满
int IsQueFull(PT_QUEUE ptQue)
{
    return (ptQue->dwTail + 1) % QUEUE_SIZE == ptQue->dwHead;
}

//获取循环队列元素数目
int QueDataNum(PT_QUEUE ptQue)
{
    return (ptQue->dwTail - ptQue->dwHead + QUEUE_SIZE) % QUEUE_SIZE;
}

//获取循环队列队首位置
int GetQueHead(PT_QUEUE ptQue)
{
    return ptQue->dwHead;
}
//获取循环队列队首元素
int GetQueHeadData(PT_QUEUE ptQue)
{
    return ptQue->aData[ptQue->dwHead];
}
//获取循环队列队尾位置
int GetQueTail(PT_QUEUE ptQue)
{
    return ptQue->dwTail;
}

