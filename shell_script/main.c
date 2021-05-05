/**
 * Simple shell interface starter kit program.
 * Operating System Concepts
 * Project 1
 */

#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>
#include <fcntl.h>

#define MAX_LINE 80 /* 80 chars per line, per command */
char *command1[5];
char *command2[5];

void pipecall(){     // Code refrence Texbook operating system figure 3.22 on page -142
    int filedirec[2] ;
    pid_t subchildid;
    int check = pipe(filedirec);
    if(check == -1){
        printf("ERROR loading PIPE \n");
    }
    subchildid = fork();
    if(subchildid < 0){
        printf("ERROR forking subchild \n");
    }else if(subchildid == 0){   // child
        close(filedirec[0]);
        dup2(filedirec[1],1);  // 1 is for standard out
        execvp(command1[0],command1);
        //printf("childptr1 %s \n",childptr1[0]);
        close(filedirec[1]);
    }else{
        close(filedirec[1]);
        dup2(filedirec[0],0);  // 0 is for standard in
        execvp(command2[0],command2);
        //printf("childptr1 %s \n",childptr1[0]);
        close(filedirec[0]);
    }
}
int main(void)
{
    char *args[MAX_LINE/2 + 1];	/* command line (of 80) has max of 40 arguments */
    int should_run = 1;
    char history[80];
    pid_t child_id;

    while (should_run){
          printf("mysh:~$ ");
          fflush(stdout);

      //user input
          char args[80];

          //scanf("%[^\n]s",args);
          fgets(args,sizeof args,stdin);

          if((strcmp(args,"!!\n") == 0) && (strcmp(history,"") != 0)){  // fgets has a \n at the end
              strncpy(args,history,80);
              printf("echoning command -> %s",args);
          }else{
              strncpy(history,args,80);
          }
          //printf("%s \n",args);
          char *ptrs[5];
          char space[6] = " \n";
          ptrs[0] = strtok(args, space);      // REFENCE of strtok from terminal command man 3 strtok
          int ipts = 1;
          while((ptrs[ipts] = strtok('\0',space)) != NULL){
              ipts = ipts + 1;
          }
          ptrs[ipts] = NULL;
          //printf("thsis-%s \n",ptrs[ipts-1]);

          int t = 0;
          int b = 0;
          int redirecout = 0;
          int redirecin = 0;
          int pipeopen = 0;
          char *save_filename = ptrs[ipts-1];
          //printf("thsis-%s \n",save_filename);
          while(ptrs[t] != '\0'){
              if(strcmp(ptrs[t],"&") == 0){
                 b = 1;
                 ptrs[t] = NULL;
              }else if(strcmp(ptrs[t],">") == 0){
                 redirecout = 1;
                 ptrs[t+1] = NULL;
                 ptrs[t] = NULL;
              }else if(strcmp(ptrs[t],"<") == 0){
                 redirecin = 1;
                 ptrs[t+1] = NULL;
                 ptrs[t] = NULL;
              }else if(strcmp(ptrs[t],"|") == 0){
                 pipeopen = 1;
                 ptrs[t] = NULL; // only | is set to null
                 break;
              }
              t = t + 1;
          }
          //printf("thsis-%s \n",ptrs[2]);
          //printf("thsis B-%d \n",t);
          //printf("thsis redirec-%d \n",redirecin);
          //char *command1[5];
          if(pipeopen == 1){
              int p1 =0;
              for(p1 = 0; p1 < t; p1++){
                 command1[p1] = ptrs[p1];
              }
              //char *command2[5];
          }else{
             // do nothing
          }

              int p2 = 0;
              for(p2 = 0;p2 < (ipts-1-t);p2++ ){
                 command2[p2] = ptrs[t+p2+1];
              }
              command1[t] = NULL;
              command2[ipts-1-t] = NULL;

          //printf("thsis commadn-%d \n",(ipts-1-t));
        /**
          * After reading user input, the steps are:
          * (1) fork a child process
          * (2) the child process will invoke execvp()
          * (3) if command includes &, parent and child will run concurrently
          */

          if(strcmp(ptrs[0],"cd") == 0){  // cd command refrence proffesor course forum
              chdir(ptrs[1]);
          }

          if(strcmp(ptrs[0],"exit") == 0){
             should_run = 0;
          }
          // Refernece of the coding for flock system call taken
          // from Figure 3.8 operating system 10th edition Textbook
          //pid_t child_id;
          child_id = fork();

          if(child_id < 0){
              fprintf(stderr, "fork system call failed");
              return 1;
          }else if (child_id == 0){   // child process

              if(redirecout == 1){
                  int filedis = open(save_filename, O_WRONLY | O_CREAT, 0777);  // Refrence linux ma7.org manual for linux users
                  if(filedis == -1){
                     return 3;  // random error
                  }
                  //printf("fd to result.txt %s \n",ptrs[2]);
                  dup2(filedis,STDOUT_FILENO);  // taking file name
                  //printf("after dup2 \n");
                  close(filedis);
              }else if(redirecin == 1){
                  int infile = open(save_filename,O_RDONLY);
                  if(infile == -1){
                     printf("file not found \n");
                     return 3;  // random error
                  }
                  dup2(infile,STDIN_FILENO);
                  close(infile);

              }

              if(b == 1){
                  //int backis = open("backgroundout.txt", O_WRONLY | O_CREAT, 0777);  // Refrence linux ma7.org manual for linux users
                  int backis = open("/dev/null",O_RDWR);
                  if(backis == -1){
                     return 4;  // random error
                  }
                  //printf("fd to result.txt %s \n",ptrs[2]);
                  dup2(backis,STDOUT_FILENO);  // taking file name
                  //printf("after dup2 \n");
                  if(strcmp(ptrs[0],"exit") != 0){   // check for exit
                       execvp(ptrs[0],ptrs);
                  }
                  close(backis);
              }
              else if(redirecout == 1 || redirecin == 1){

                  if(strcmp(ptrs[0],"exit") != 0){   // check for exit
                       execvp(ptrs[0],ptrs);
                  }

              }else if(pipeopen == 1){
                    pipecall();
              }else{
                  //execlp("/bin/ls","ls",NULL);
                  if(strcmp(ptrs[0],"exit") != 0){   // check for exit
                       execvp(ptrs[0],ptrs);
                  }
              }
              return 0;
          }else{
              if((strcmp(args,"!!") == 0)){
                  printf("No commands in history.\n");
              }

              if(b == 0){
                wait(NULL);
                //printf("child process ended \n");
              }
              if(b == 1){
                 sleep(1);
                 kill(child_id,SIGTERM); // kill then wait
                 wait(NULL);     // wait is important to return to inital place
              }
          }

    }

    return 0;
}
