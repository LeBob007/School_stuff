/*
myshell.c
Robert Hu
ryhu
CMPS 111
4/20/2019
assgn-1
*/

#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

extern char **getArgs();

int main() {
  int i, ind;
  char **args;

  while (1) {
    printf(">: "); // This is the prompt character
    args = getArgs();
    ind = 0;
    if (args[0] == NULL) {                 // Do nothing if there is no command
    } else if (!strcmp(args[0], "exit")) { // Break out of loop if exit
      break;
    }

    int done = 0; // Keep track of status
    while (!done) {
      int fdPipe1[2], fdPipe2[2];
      int pIn = 0, pOut = 0;
      int cmdDone = 0; // Command Status
      int nxtCmdIndex = 0, curIndex = 0;
      int semicolCheck = 0;
      for (i = 0; args[i] != NULL; i++) { // Set i to the number of args
      }

      while (!cmdDone) {
        int status;
        int redirErr = 0;
        char currDir[256];
        int fdIn = -1;
        int fdOut = -1;
        int argc;

        for (argc = nxtCmdIndex; argc < i; argc++) {
          ind++;
          // Check for '<'
          if (strcmp(args[argc], "<") == 0) {
            args[argc] = NULL;
            fdIn = open(args[argc + 1], O_RDONLY);
            if (fdIn < 0) {
              perror("Open error");
              exit(1);
            }
            ind++;
            break;
          }
          // Check for '>'
          else if ((strcmp(args[argc], ">") == 0) ||
                   (strcmp(args[argc], ">&") == 0)) {
            // Check for '&'
            if (strcmp(args[argc + 1], "&") == 0) {
              redirErr = 1;
              argc = argc + 1;
            }
            // Check for '>&'
            if (strcmp(args[argc], ">&") == 0) {
              redirErr = 1;
            }
            char *temp = args[argc];
            args[argc] = NULL;
            int tmp = argc;
            fdOut = open(args[argc + 1], O_RDWR | O_CREAT, 0777);
            if (fdOut < 0) {
              perror("Open error");
              exit(1);
            }
            ind++;
            // Check for '>&'
            if (strcmp(temp, ">&") == 0) {
              break;
            }
          }
          // Check for '>>'
          else if ((strcmp(args[argc], ">>") == 0) ||
                   (strcmp(args[argc], ">>&") == 0)) {
            // Check for '>>&'
            if (strcmp(args[argc], ">>&") == 0) {
              redirErr = 1;
            }
            args[argc] = NULL;
            fdOut = open(args[argc + 1], O_RDWR | O_APPEND | O_CREAT, 0777);
            if (fdOut < 0) {
              perror("Open error");
              exit(1);
            }
            ind++;
            break;
          }
          // Check for '|'
          else if ((strcmp(args[argc], "|") == 0) ||
                   (strcmp(args[argc], "|&") == 0)) {
            // Check for '|&'
            if (strcmp(args[argc], "|&") == 0) {
              redirErr = 1;
            }
            args[argc] = NULL;
            pOut = 1;
            nxtCmdIndex = argc + 1;
            if (pipe(fdPipe2) == -1) {
              perror("Pipe error");
              exit(1);
            }
            break;
          }
          // Check for 'cd' command
          else if (strcmp(args[argc], "cd") == 0) {
            getcwd(currDir, sizeof(currDir)); // Get current directory
            if (args[argc + 1] == NULL) { // If next term is NULL just 'cd' then
                                          // go to HOME variable
              char *dir = getenv("HOME");
              chdir(dir);
            } else { // Else try 'cd' to the next arg
              chdir(args[argc + 1]);
            }
          }
          // Check for ';'
          else if (strcmp(args[argc], ";") == 0) {
            semicolCheck = 1;
            args[argc] = NULL;
            nxtCmdIndex = argc + 1;
            break;
          }
        }

        pid_t pid = fork(); // Forking code
        if (pid == 0) {
          if (fdIn != -1) {
            close(0);
            dup(fdIn);
          }
          if (fdOut != -1) {
            close(1);
            dup(fdOut);
            if (redirErr) {
              close(2);
              dup(fdOut);
            }
          } else if (pOut || pIn) {
            if (pIn) {
              close(0);
              dup(fdPipe1[0]);
              close(fdPipe1[0]);
              close(fdPipe1[1]);
            }
            if (pOut) {
              close(1);
              dup(fdPipe2[1]);
              close(fdPipe2[0]);
              close(fdPipe2[1]);
              pOut = 0;
            }
          }
          if (execvp(args[curIndex], &args[curIndex]) == -1) {
            perror("execvp() error\n");
            exit(1);
          }
        } else {
          while (wait(&status) != pid)
            ;
          if (semicolCheck) {
            curIndex = nxtCmdIndex;
          }
          if (pOut) {
            close(fdPipe2[1]);
            fdPipe1[0] = fdPipe2[0];
            fdPipe1[1] = fdPipe2[1];
            pIn = pOut;
            curIndex = nxtCmdIndex;
          }
        }
        if (args[ind] == NULL) { // Finish current command args
          cmdDone = 1;
        }
      }
      if (args[ind] == NULL) { // Finish current command args
        done = 1;
      }
    }
  }
  return 0;
}
