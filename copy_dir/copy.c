#include<stdio.h>
#include<unistd.h>
#include<fcntl.h>
#include<unistd.h>
#include<stdlib.h>
#include<sys/types.h>
#include<sys/stat.h>
#include<string.h>
#include<dirent.h>
#include <libgen.h>
#define BUFFERSIZE 4096//每次读取的缓冲区设置
//#define COPYMODE 0644//设置文件默认打开权限，已经弃用

struct stat info;
char *deal_path(char *,char *);
char *deal_path1(char *,struct dirent *);
int is_file(char *);
char *deal_with(char *,char *);
void oops(char *,char *);
int exists(char *);
void do_cp(char *,char *);
int main(int argc,char *argv[])
{
        char answer[10];
        char c;
        //设置buf
        struct stat filebuf1;
        struct stat filebuf2;
        struct stat tmpbuf;
        char *filename=NULL;
        char *filename2=NULL;
        struct dirent  *dirname;
        DIR *dir_ptr;
        //判断参数
                if(argc != 3){
                fprintf(stderr,"Usage: %s source destination\n",*argv);
                exit(1);
        }
        //判断源文件和目标文件是否相等
        if(strcmp(argv[1],argv[2]) == 0)
        {
                fprintf(stderr,"No Copy Source File equal Dest File\n");
                exit(1);
        }
        //测试文件是否可以访问

        if(access(argv[2],F_OK) == 0){
                //询问文件是否可以覆盖
                printf("Can you over the file (y/n):");
                scanf("%9s",&answer);

        //判断用户的输入

                if(*answer != 'y' && *answer != 'Y'){
                        fprintf(stderr,"the dst file exists,don't over\n");
                        exit(1);
        }
        }

        //判断目标是否是目录
        stat(argv[2],&filebuf2);
        stat(argv[1],&filebuf1);
        if(!S_ISDIR(filebuf1.st_mode)){
                if(S_ISDIR(filebuf2.st_mode))
                        filename = deal_path(argv[2],argv[1]);
                else
                        filename = argv[2];
        //      printf("%s\n",filename);
                do_cp(argv[1],filename);
                free(filename);
             }
             
        else{
                if(S_ISDIR(filebuf2.st_mode)){

                        if((dir_ptr = opendir(argv[1])) == NULL)
                                sprintf("stderr","Can't open dir %s\n",argv[1]);
                        else
                                while((dirname=readdir(dir_ptr)) != NULL){
                                             filename = deal_path(argv[1],dirname->d_name);
                                             if(is_file(filename)){
                                                     filename2 = deal_with(filename,argv[2]);
                                                     do_cp(filename,filename2);
                                                     free(filename);
                                                     free(filename2);
                                             }else{
                                                     filename2=deal_path(argv[2],dirname->d_name);
                                                     mkdir(filename2,filebuf1.st_mode);
                                                     free(filename);
                                                     free(filename2);
                                                     continue;
                                             }
                                 }
                        closedir(dir_ptr);
                }
                else{
                    printf("error:%s is not a dir or dir is not built\n",argv[2]);
                }
    }
}


//文件copy函数
void do_cp(char *path1,char *path2)
{
        int in_fd,out_fd,n_chars;
        struct stat filebuf1;//保证copy的文件和原始的文件权限一致
        char buf[BUFFERSIZE];

        stat(path1,&filebuf1);
        if((in_fd = open(path1,O_RDONLY)) == -1)
                oops("Cannot open",path1);
        if((out_fd = open(path2,O_WRONLY|O_CREAT,filebuf1.st_mode)) == -1)
                oops("Cannot create",path2);
        //开始读取
        while((n_chars = read(in_fd,buf,BUFFERSIZE)) > 0)
                if(write(out_fd,buf,n_chars) != n_chars)
                oops("Write error to",path2);
        //判断最后是否写入
        if(n_chars == -1)
                oops("Read error from,path1","");
                        //判断最后是否写入
        if(n_chars == -1)
                oops("Read error from,path1","");
        //关闭文件
        if(close(in_fd) == -1||close(out_fd) == -1)
                oops("Error closing files","");
}


void oops(char *s1,char *s2)
{
        fprintf(stderr,"Error:%s ",s1);
        perror(s2);
        exit(1);
}


int exists(char *filename)
{
        return access(filename,F_OK);

}

int is_file(char *filename)
{
        struct stat filebuf;
        stat(filename,&filebuf);
        if(S_ISREG(filebuf.st_mode))
                return 1;
        else
                return 0;
}

char *deal_with(char *filename,char *filename2)
{
        char *file=NULL;;
        if((file = (char *)malloc(strlen(basename(filename))+strlen(filename2)+3)) == NULL)
                perror("malloc error");
                        else{
                if(filename2[strlen(filename2)-1] == '/'){
                        strcpy(file,filename2);
                        strcat(file,basename(filename));
                }else{
                        strcpy(file,filename2);
                        strcat(file,"/");
                        strcat(file,basename(filename));
                     }
        }
        return file;
}

char *deal_path(char *file,char *file2)
{
        char *filename=NULL;
        if((filename = (char *)malloc(strlen(file)+strlen(file2)+3)) == NULL)
                                perror("Malloc erro:");
        else{

        if(file[strlen(file)-1] == '/'){
                        strcpy(filename,file);
                        strcat(filename,file2);
        }else{
                        strcpy(filename,file);
                        strcat(filename,"/");
                        strcat(filename,file2);
                }
        }
        return filename;

}
