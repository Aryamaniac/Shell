#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdbool.h>

#define BUFFSIZE 8192

/**
 * This method reads the command from the input line and
 * places in into an array of characters.
 * @returns char pointer to the command line arg
 **/
char * readInput() {
    char * input = malloc(BUFFSIZE);
    for (int i = 0; i < 8192; i++) {
        input[i] = 0;
    }
    read(STDIN_FILENO, input, BUFFSIZE);
    return input;
}

int main(int argc, char * envp[]) {
    char * pro;
    char printdir[8192];
    char dir[] = "/home/myid/";
    char dir2[8192];
    for (int i = 0; i < 8192; i++) {
        dir2[i] = 0;
        printdir[i] = 0;
    }
    setbuf(stdout, NULL);
    bool exit = false;
    char seps[] = " \n";
    int count = 0;
    char * p;

    for (int i = 5; envp[15][i] != 0; i++) {
        dir2[count] = envp[15][i];
        count++;
    }
    strcat(dir, dir2);
    chdir(dir);
    int dirlen = 0;
    while (dir[dirlen] != 0) {
        dirlen++;
    }

    bool redirin, app, trun;
    int redirm = 0;
    char* fop[128];
    char* ifile = NULL;
    char* ofile = NULL;
    char* afile = NULL;

    while(!exit) {
        char inputs[30][30];
        for (int i = 0; i < 128; i++) {
            fop[i] = 0;
        }

        for (int i = 0; i < 8192; i++) {
            dir2[i] = 0;
            printdir[i] = 0;
        }
        count = 0;
        redirin = false;
        app = false;
        trun = false;
        ifile = NULL;
        ofile = NULL;
        afile = NULL;


        //get dir and print prompt
        getcwd(dir2, BUFFSIZE);
        printf("1730sh:");
        if (strstr(dir2, dir)) {
            if (strcmp(dir2, dir) == 0) {
                printdir[0] = '~';
            } else {
                printdir[0] = '~';
                int track = 1;
                for (int i = dirlen; dir2[i] != 0; i++) {
                    printdir[track] = dir2[i];
                    track++;
                }
            }
            printf(printdir);
        } else {
            printf(dir2);
        }
        printf("$ ");

        //read input and seperate into array
        pro = readInput();
        p = strtok(pro, seps);
        while(p != NULL){
            strcpy(inputs[count], p);
            count++;
            p = strtok(NULL, seps);
        }

        //test for input redirection
        //printf("start testing\n");
        for (int i = 0; i < count; i++) {
            if (strcmp(inputs[i], "<") == 0) {
                redirin = true;
                ifile = inputs[i+1]; // set to next value inputs[]
                if (redirm == 0) {
                    redirm = i;
                }
            } else if (strcmp(inputs[i], ">") == 0) {
                trun = true;
                ofile = inputs[i+1];
                if (redirm == 0) {
                    redirm = i;
                }
            } else if (strcmp(inputs[i], ">>") == 0) {
                app = true;
                afile = inputs[i+1];
                if (redirm == 0) {
                    redirm = i;
                }
            }
        }

        if (!redirin && !trun && !app) {
            redirm = count;
        }

        if (strcmp(inputs[0], "exit") == 0) {
            free(pro);
            return EXIT_SUCCESS;
        }
        //handle cd
        if (strcmp(inputs[0], "cd") == 0) {
            if (strcmp(inputs[1], dir) == 0) {
                //printdir[0] = '~';
            }
            if (chdir(inputs[1]) < 0) {
                printf("1730sh: cd: ");
                printf(inputs[1]);
                printf(": No such file or directory\n");
            }
        } else { //all other commands
            for (int i = 0; i < redirm; i++) {
                fop[i] = inputs[i];
            }
            //for (int i = 0; fop[i] != 0; i++) {
            //    printf(fop[i]);
            //}
            pid_t pid;
            for (int i = 0; i < redirm; i++) {
            }
            if ((pid = fork()) < 0) perror("fork");
            else if (pid == 0) {
                if(ifile != NULL) {  // if there was a "<" found
                    int fd = open(ifile, O_RDWR);
                    if (dup2(fd, STDIN_FILENO) == -1){
                        perror("dup2");
                    }
                    close(fd);
                } // ifile

                if (ofile != NULL) { // if there was a ">" found
                    int fd2 = open(ofile, O_CREAT | O_RDWR, 0644);
                    dup2(fd2, STDOUT_FILENO);
                    close(fd2);
                } // OFILE

                if (afile != NULL) { // if there was a ">>" found
                    int fd3 = open(afile, O_CREAT | O_RDWR | O_APPEND, 0644);
                    dup2(fd3, STDOUT_FILENO);
                    close(fd3);
                } // APPEND
                execvp(inputs[0], fop);
            } else {
                int status;
                wait(&status);
            } // else, wait for child
        }
        free(pro);
    }
}
