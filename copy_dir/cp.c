#include<stdio.h>
#include<sys/types.h>
#include<dirent.h>
#include<string.h>


void help();//帮助界面
int FileOrFolder(char argv[]);//判断传入是文件还是目录
int  FileToFileCopy(char *src,char *dst);//文件到文件Copy
int FolderCopyToFolder(char argv[]);//目录到目录copy
//以上为函数定义


void main(int argc,char *argv[]){
		int num=0;
		if(argc!=3)
				help();
		else {
				FileToFileCopy(argv[1],argv[2]);
		}
}

//函数定义
int FileToFileCopy(char *src,char *dst){
		FILE *fp1,*fp2;
		char c=0;
        char pwd="/home/lypto/system_learning/";
        //printf("%s",dst);
		src=strcpy(pwd,src)
        if(fp1=fopen(src,"r")==NULL){
				printf("No such file,please cheak your input!");
				return 0;
		}
		if(fp2=fopen(dst,"w")==NULL){
				//printf("something wrong with output!，open %s error",argv[2]);
				fclose(fp1);
				return 0;
		}
		c=fgetc(fp1);
		while(!feof(fp1)){
				//printf("%c",c);
				fputc(c,fp2);
				c=fgetc(fp1);	
		}
		fclose(fp1);
		fclose(fp2);
		return 1;
}
void help(){
		printf("cp is a command to copy a file.\n\n ");
		printf("cp needs 2 argcs\n");
		printf("--source-file\n");
		printf("--dis-file\n");
		printf("example: cp 1.c 2.c\n");
}
