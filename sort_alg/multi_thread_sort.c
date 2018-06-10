/*Author:lypto
 *Data:2018.6.4
 *description:
 *两个线程来对一个大的数组进行拆分排序，然后返回给主程序使用归并排序的思想来整合数组，在两个子线程中使用的是快速排序算法*/

#include<stdio.h>
#include<pthread.h>
#include<stdlib.h>
#define MAX 15

//int arr_test[]={1,2,4,3,9,6,5,7,8,0};
typedef struct mypara
{
    int *array;
    int low;
    int high;
}para;//定义传参结构体

int  quicksort_begin(para *arg);//快速排序
void show(int *array,int len);//数据展示函数
void mergesort(int array[],int start,int middle,int end);//数组合并
void quicksort(int *array,int low,int high);


void main(){

    int arr_test[]={1,2,4,3,3,70,9,6,5,7,8,20,27,34,0};
    //int arr_test[]={20,15,1,2,3,4,5,9,7,10,6,11,8,14,13,16,19,17,18,12};
    show(arr_test,MAX);
    int ret1,ret2;
    //从结构体传参
    para para1,para2;
    //给结构题数组赋值
    para1.array=arr_test;
    para1.low=0;
    para1.high=MAX/2-1;

    para2.array=arr_test;
    para2.low=MAX/2;
    para2.high=MAX-1;

    pthread_t id1;//第一个线程
    pthread_t id2;//第二个线程
    int result1,result2;
    if(ret1=pthread_create(&id1,NULL,(void *) &quicksort_begin,&(para1))!=0) 
        printf("id1 error!");
    if(ret2=pthread_create(&id1,NULL,(void *) &quicksort_begin,&(para2))!=0) 
        printf("id2 error!");
    pthread_join(id1,&result1);
    pthread_join(id2,&result2);
    while(result1!=1&&result2!=1);
    mergesort(arr_test,0,MAX/2-1,MAX-1);
    show(arr_test,MAX);
    return (0);
}

void show(int array[],int len){
    //int len=sizeof(array)/sizeof(array[0]);这个方法只在主函数有效
    for(int i=0;i<len;i++)
        printf("%d ",array[i]);
    printf("\n");
}

void swap(int *s,int i,int j){
    int temp;
    temp=s[i];
    //printf("curent temp:%d\n",temp);
    s[i]=s[j];
    s[j]=temp;
}

int  quicksort_begin(para *arg){
    //printf("entry quicksort_begin\n");
    int low=arg->low;
    int last=low;//基准
    int high=arg->high;
    int *array=arg->array;
    if(low<high){
    for(int i=low+1;i<=high;i++){
        if(array[i]<array[low])
            swap(array,++last,i);
    }    //printf("array:%d\n",array[3]);
    swap(array,last,low);
    quicksort(array,low,last-1);
    quicksort(array,last+1,high);
    
}
   // show(array,MAX);
    return 1;
}

void quicksort(int *array,int low,int high){
    int last=low;
    //printf("entry quicksort!");
    if(low<high){
    for(int i=low+1;i<=high;i++){
        if(array[i]<array[low])
            swap(array,++last,i);
    }
    swap(array,last,low);
    quicksort(array,low,last-1);
    quicksort(array,last+1,high);
}
}

void mergesort(int array[],int start,int middle,int end){
  
    int tmp[MAX] = {0};  // 临时数组 
    int i; //第一个数组索引  
    int j; //第二个数组索引  
    int k; //临时数组索引  
      
    for (i = start, j = middle+1, k = 0; k <= end-start; k++) 
    {  
        if (i == middle+1)  
        {  
            tmp[k] = array[j++];  
            continue;  
        }  
        if (j == end+1)  
        {  
            tmp[k] = array[i++];  
            continue;  
        }  
        if (array[i] <array[j])  
        {  
            tmp[k] = array[i++];  
        }  
        else  
        {  
            tmp[k] = array[j++];  
        }  
    }  
      
    for (i = start, j = 0; i <= end; i++, j++)  
    {  
        array[i] = tmp[j];  
    }  
}  

   /* printf("into mergesort!");
    int tmp[MAX]={0};
    int index_1;
    int index_2;
    int index_t;
    for(index_1=start,index_2=middle+1,index_t=0;index_t<=end-start;index_t++)
    {
        if(index_1==middle+1)
        {
            tmp[index_t]=array[index_2++];
            continue;
        }
        if(index_2==end+1)
        {
            tmp[index_2]=array[index_1++];
            continue;
        }
        if(array[index_1]<array[index_2])
        {
            tmp[index_t]=array[index_1++];
        }
        else
        {
            tmp[index_t]=array[index_2++];
        }
    }
    for(index_1=start,index_2=0;index_1<=end;index_1++,index_2++)
    {
        array[index_1]=tmp[index_2];
    }
    return 0;

}
*/

