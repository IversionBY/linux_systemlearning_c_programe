#include<stdio.h>
#include<unistd.h>


int a=0;
int add=1;
int  main(){
    add=add+1;
	printf("hello_fork!\n");
	while(a<2)
    {
    printf("while_begin\n\n\n\n");
    pid_t pid=fork();
	if(pid==-1)//失败
        printf("fork_error!\n");
	else if(pid==0){//子进程
        
        printf("son_a:%d,and add=%d\n",a,add);
		printf("son:%d\n",getpid());
        a++;
	}
    else{//父进程

    printf("parents_a:%d\n",a);
	printf("parent:%d\n",getpid());
    a++;
    }

}
}
