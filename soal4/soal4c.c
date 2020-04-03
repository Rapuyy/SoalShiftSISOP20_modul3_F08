#include<stdio.h>
#include<stdlib.h>
#include<fcntl.h>
#include<errno.h>
#include<sys/wait.h>
#include<unistd.h>


int main(){
    int p[2];
    pid_t pp;
    if(pipe(p) == -1)
    {
        fprintf(stderr, "pipe failed");
        return 1;
    }
    pp = fork();
    if(pp < 0){
        fprintf(stderr, "fork failed");
    }
    else if(pp !=  0) //parent process
    {
        close(0);
        dup2(p[0], 0);
        close(p[0]);
        close(p[1]);
        char *argv[] = {"wc", "-l", NULL};
        execv("/usr/bin/wc", argv);
    }
    else //child
    {
        close(0);
        dup2(p[1], 1);
        close(p[0]);
        close(p[1]);
        char *argv[] = {"ls", NULL};
        execv("/bin/ls", argv);
    }
    
    
}
