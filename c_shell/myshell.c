#include<stdio.h>
#include<string.h>
#include<signal.h>
#include<stdlib.h>     
#include<unistd.h>
#include<errno.h> 
#include<fcntl.h>
#include<sys/utsname.h>
#include<sys/stat.h>
#include<sys/wait.h>
#include<sys/ptrace.h>
#include<sys/types.h>
#include<pwd.h>// 全局声明

typedef int bool;

#define true 1
#define false 0
#define MAX 100
#define LSH_RL_BUFSIZE 1024 /*命令缓冲区*/
#define LSH_TOK_BUFSIZE 64 /*缓冲区定义在拆分字符*/
#define LSH_HIST_SIZE 10 /*最大历史存储*/
#define LSH_TOK_DELIM " \t\r\n\a" /*用于拆非的字符串*/

/* 这个boolx型变量用于&后台触发的flag*/
bool conc = false;
/*宏定义历史命令的最后一个命令*/
int cur_pos = -1;
/*全局定义*/
char *history[LSH_HIST_SIZE];
int cur_bufsize = LSH_TOK_BUFSIZE;
char cwd[MAX];/*保存当前路径*/


int lsh_cd(char **args);
int lsh_help(char **args);
int lsh_exit(char **args);

/* 内建函数列表*/
char *builtin_str[] = {
    "cd",
    "help",
    "exit"
};

int (*builtin_func[]) (char **) = {
    &lsh_cd,
    &lsh_help,
    &lsh_exit
};

int lsh_num_builtins(){
    return sizeof(builtin_str) / sizeof(char *);
}

/*cd内建命令*/
int lsh_cd(char **args)
{
    if(args[1] == NULL){
        fprintf(stderr, "lsh: expected argument to \"cd\"\n");
    }else{
        if(chdir(args[1]) != 0){
            perror("lsh");
        }
    }

    return 1;
}

int lsh_help(char **args)
{
    int i;
    printf("Aman Dalmia's LSH\n");
    printf("Type program names and arguments, and press enter.\n");
    printf("Append \"&\" after the arguments for concurrency between parent-child process.\n");
    printf("The following are built in:\n");

    for(i = 0; i < lsh_num_builtins(); i++){
        printf(" %s\n", builtin_str[i]);
    }

    printf("Use the man command for information on other programs.\n");
    return 1;
}

int lsh_exit(char **args)
{
    return 0;
}

/*主要的命令执行函数*/
int lsh_launch(char **args)
{
    pid_t pid, wpid;
    int status;

    pid = fork();
    if(pid == 0){ /* child process */
        if(execvp(args[0], args) == -1) perror("lsh");
        exit(EXIT_FAILURE);
    }else if(pid > 0){ /* parent process */
        if(!conc){
            do{
                wpid =  waitpid(pid, &status, WUNTRACED);
            }while(!WIFEXITED(status) && !WIFSIGNALED(status));
        }
    }else{ /*fork error */
        perror("lsh");
    }

    conc = false;
    return 1;
}

/*处理用户的输入*/
char **lsh_split_line(char *line){
    cur_bufsize = LSH_TOK_BUFSIZE;
    int position = 0;
    char **tokens = malloc(cur_bufsize * sizeof(char*));
    char *token;

    if(!tokens){
        fprintf(stderr, "lsh: allocation error\n");
        exit(EXIT_FAILURE);
    }

    token = strtok(line, LSH_TOK_DELIM);//将命令行进行按需分割
    while(token != NULL){
        tokens[position] = token;
        position++;

        if(position >= cur_bufsize){
            cur_bufsize += LSH_TOK_BUFSIZE;
            tokens = realloc(tokens, cur_bufsize * sizeof(char*));
            if(!tokens){
                fprintf(stderr, "lsh: allocation error\n");
                exit(EXIT_FAILURE);
            }
        }

        token = strtok(NULL, LSH_TOK_DELIM);
    }
    //下面把buff里面的的最后一位设置成NULL，方便让execve()函数执行，execve函数里面的执行字符串要求末尾以NULL结尾，默认的结尾是“\0”，所以需要进行转换
    if(position > 0 && strcmp(tokens[position - 1], "&") == 0) {
        conc = true;
        tokens[position - 1] = NULL;
    }
    tokens[position] = NULL;
    return tokens;
}

/*内建history命令的实现*/
int lsh_history(char **args)
{
    if(cur_pos == -1 || history[cur_pos] == NULL){
        fprintf(stderr, "No commands in history\n");
        exit(EXIT_FAILURE);
    }

    if(strcmp(args[0], "history") == 0){
        int last_pos = 0, position = cur_pos, count = 0;

        if(cur_pos != LSH_HIST_SIZE && history[cur_pos + 1] != NULL){
            last_pos = cur_pos + 1;
        }

        count = (cur_pos - last_pos + LSH_HIST_SIZE) % LSH_HIST_SIZE + 1;

        while(count > 0){
            char *command = history[position];
            printf("%d %s\n", count, command);
            position = position - 1;
            position = (position + LSH_HIST_SIZE) % LSH_HIST_SIZE;
            count --;
        }
    }else{
        char **cmd_args;
        char *command;
        if(strcmp(args[0], "!!") == 0){
            command = malloc(sizeof(history[cur_pos]));
            strcat(command, history[cur_pos]);
            printf("%s\n", command);
            cmd_args = lsh_split_line(command);
            int i;
            for (i = 0; i < lsh_num_builtins(); i++){
                if(strcmp(cmd_args[0], builtin_str[i]) == 0){
                    return (*builtin_func[i])(cmd_args);
                }
            }
            return lsh_launch(cmd_args);
        }else if(args[0][0] == '!'){
            if(args[0][1] == '\0'){
                fprintf(stderr, "Expected arguments for \"!\"\n");
                exit(EXIT_FAILURE);
            }
            /*历史命令的增加，这里使用了取模运算，十分的巧妙*/
            int offset = args[0][1] - '0';
            int next_pos = (cur_pos + 1) % LSH_HIST_SIZE;
            if(next_pos != 0 && history[cur_pos + 1] != NULL){
                offset = (cur_pos + offset) % LSH_HIST_SIZE;
            }else{
                offset--;
            }
            if(history[offset] == NULL){
                fprintf(stderr, "No such command in history\n");
                exit(EXIT_FAILURE);
            }
            command = malloc(sizeof(history[cur_pos]));
            strcat(command, history[offset]);
            cmd_args = lsh_split_line(command);
            int i;
            for (i = 0; i < lsh_num_builtins(); i++){
                if(strcmp(cmd_args[0], builtin_str[i]) == 0){
                    return (*builtin_func[i])(cmd_args);
                }
            }
            return lsh_launch(cmd_args);
        }else{
            perror("lsh");
        }
    }
}

/*执行前的判断函数*/
int lsh_execute(char *line){
    int i;

    char **args = lsh_split_line(line);

    if(args[0] == NULL){ /*如果输入了空指令，就把标志位置1*/
        return 1;
    }else if(strcmp(args[0], "history") == 0 ||
             strcmp(args[0], "!!") == 0 || args[0][0] == '!'){
        return lsh_history(args);
    }

    cur_pos = (cur_pos + 1) % LSH_HIST_SIZE;
    history[cur_pos] = malloc(cur_bufsize * sizeof(char));
    char **temp_args = args;
    int count=0;

    while(*temp_args != NULL){
        strcat(history[cur_pos], *temp_args);
        strcat(history[cur_pos], " ");
        temp_args++;
    }
    
    if(cur_pos > 0)

    for (i = 0; i < lsh_num_builtins(); i++){
        if(strcmp(args[0], builtin_str[i]) == 0){
            return (*builtin_func[i])(args);
        }
    }

    return lsh_launch(args);
}

/* Read input from stdin */
char *lsh_read_line(void)
{
    cur_bufsize = LSH_RL_BUFSIZE;
    int position = 0;
    char *buffer = malloc(sizeof(char) * cur_bufsize);
    int c;

    if(!buffer){
        fprintf(stderr, "lsh: allocation error\n");
        exit(EXIT_FAILURE);
    }

    while(1){
        /*以每次一字符读入，并且把读取到的字符转存到buff*/
        c = getchar();

        if(c == EOF || c == '\n'){
            buffer[position] = '\0';
            return buffer;
        }else{
            buffer[position] = c;
        }
        position++;

        /*数组内存越界判断，如果超界，重新分配指针内存*/
        if(position >= cur_bufsize){
            cur_bufsize += LSH_RL_BUFSIZE;
            buffer = realloc(buffer, cur_bufsize);
            if(!buffer){
                fprintf(stderr, "lsh: allocation error\n");
                exit(EXIT_FAILURE);
            }
        }
    }
}


/* 用于输出提示符 */
void print_prompt()
{
    // 调用uname获取系统信息
    struct utsname uname_ptr;
	struct passwd *pid_to_name;
    uname(&uname_ptr);
	pid_to_name = getpwuid(getuid());
    //调用 getcwd 获取当前路径名，并存储在 cwd 指向的字符串
    getcwd(cwd, sizeof(cwd));
    setbuf(stdout, NULL);       //禁用 buffer， 直接将 printf 要输出的内容输出
    printf("[myshell]<%s@%s:%s> ",pid_to_name->pw_name,uname_ptr.sysname, cwd);
}

/*循环扫描用户输入*/
void lsh_loop(void)
{
    char *line;
    int status;

    do {
        print_prompt();
        line = lsh_read_line();
        status = lsh_execute(line);

        free(line);
    } while(status);
}

/*主函数*/
int main(void)
{
    lsh_loop();
    return EXIT_SUCCESS;
}
