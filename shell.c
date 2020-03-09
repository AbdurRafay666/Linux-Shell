#include<stdio.h> 
#include<string.h> 
#include<stdlib.h> 
#include<unistd.h> 
#include<sys/types.h> 
#include<sys/wait.h> 

#define clear() printf("\033[H\033[J")

void prompt() 
{ 
    char dir[200]; 
    getcwd(dir, sizeof(dir)); 
    printf("\n%s", dir);
    printf(":$> ");
} 

void split(char* com,char** args)
{
    char* token = strtok(com, " ");
    int i = 0;
    while(token != NULL)
    {
        args[i] = token;
        token = strtok(NULL, " ");
            i++;
    }
    args[i] = NULL;
}

void execute(char** args) 
{ 
    pid_t pid = fork();  
    if (pid == -1)
    { 
        printf("\nFailed forking child.."); 
        return; 
    }
    else if (pid == 0)
    {
        if (execvp(args[0], args) < 0)
        { 
            printf("\nCould not execute command.."); 
        } 
        exit(0); 
    }
    else
    { 
        wait(NULL);  
        return; 
    } 
} 

void execute_cd(char** args) 
{
    if (chdir(args[1]) < 0)
    { 
        printf("\nNo such file or directory"); 
    }
} 

void checkPipe(char* str, char** strpiped) 
{ 
    int i; 
    for (i = 0; i < 2; i++)
    { 
        strpiped[i] = strsep(&str, "|"); 
        if (strpiped[i] == NULL) 
            break; 
    }
}

void execute_piped(char** args, char** pipe_args) 
{ 
    int pipefd[2];  
    pid_t p1, p2; 
    pipe(pipefd);
    
    p1 = fork();
    if (p1 == 0)
    { 
        close(pipefd[0]); 
        dup2(pipefd[1], STDOUT_FILENO); 
        close(pipefd[1]); 
  
        if (execvp(args[0], args) < 0)
        { 
            printf("\nCould not execute command 1.."); 
            exit(0); 
        } 
    }
    
    else
    { 
        p2 = fork();
        if (p2 == 0)
        { 
            close(pipefd[1]); 
            dup2(pipefd[0], STDIN_FILENO); 
            close(pipefd[0]); 
            if (execvp(pipe_args[0], pipe_args) < 0)
            { 
                printf("\nCould not execute command 2.."); 
                exit(0); 
            } 
        }
        else
        { 
            wait(NULL); 
            wait(NULL); 
        } 
    } 
} 

int main()
{
    clear();
    printf("MY SHELL: Type 'exit' to exit.\n");
    while (1)
    {
        prompt();
        fflush(NULL);
        char com[100];
        char* args[10];
        char* pipe_args[10];
        char* re_args[10];
        char* str[2];
        
        gets(com);
        if(strcmp(com,"exit")==0)
            break;
        
        if(com[0] == 'c' && com[1] == 'd')
        {
            split(com,args);
            execute_cd(args);
        }
        
        else if(strchr(com,'|') != NULL)
        {
            checkPipe(com,str);
            split(str[0],args);
            split(str[1],pipe_args);
            execute_piped(args,pipe_args);
        }
        
        else
        {
            split(com,args);
            execute(args);
        }
    }
    return 0;
}
