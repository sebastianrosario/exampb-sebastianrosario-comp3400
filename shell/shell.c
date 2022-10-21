#include <stddef.h> // For NULL
#include <stdio.h> // For printf/fgets and others
#include <stdbool.h> // For true/false
#include <unistd.h> // For certain syscalls
#include <sys/wait.h> // For wait
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>

extern int errno ;

int run_one(const char* command, char* args[]) {
    pid_t pid = fork();
    int res;
    if (0 > pid) {printf("Error\n"); return -1;}
    if (0 == pid) {
        execvp(command, args);
        return 1;
    } else {
        waitpid(pid, &res, 0);
        if(WEXITSTATUS(res) != 0) {
            printf("Child Process Error!\n");
            return 1;
        }
    }
    return 0;
}

int run_pipe(const char* command1, char* args1[], const char* command2, char* args2[]) {
    int fd[2];
    pipe(fd);
    pid_t pid;
    int res;

    pid = fork();
    if ( pid < 0) {return 1;}
    if ( pid == 0) {
        close(fd[0]);
        dup2(fd[1], STDOUT_FILENO);
        execvp(command2, args2);
        return 1;
    } else {
        close(fd[1]);
        waitpid(pid, &res, 0);
        if(WEXITSTATUS(res) != 0) {
            printf("Bad waiting\n");
            return 1;
        }

        dup2(fd[0], STDIN_FILENO);
        execvp(command1, args1);
        return 1;
    }
    return 0;
}

// return length of broken out input, put into input_arr
int split_input(char* string, char* input_arr[]) {
    if(string[strlen(string)-1]=='\n'){
        string[strlen(string)-1] = '\0';
    }

    char* token;
    token = strtok(string, " ");
    int i = 0;
    while(token != NULL) {
        input_arr[i] = token;
        token = strtok(NULL, " ");
        ++i; 
    }
    return i;
}

void split_args(int argc, char* argv[], char* args1[], char* args2[]) {
    int i = 0;

    while(strcmp(argv[i],"|") != 0 ) {
        args1[i] = argv[i];
        ++i;
        // checking if only 1 cmd
        if(i == argc) {return;}
    }
    ++i;
    int j = 0;
    while(i != argc){
        args2[j] = argv[i];
        ++j;
        ++i; 
    }

}


int main(int argc, char* argv[]) {

    size_t raw_input_size = 32;

    char** raw_input_arr = malloc(sizeof(char*) * 20);
    char* raw_input = malloc(sizeof(char) * raw_input_size);

    while(1) {    

        printf("\n$ ");
        int res = getline(&raw_input, &raw_input_size, stdin);
        if(res < 2){
            // if nothing is inputed, reset raw input, restart loop
            // memset(raw_input, 0, sizeof(char));
            continue;
        }

        // check if user wants to exit
        if(strcmp(raw_input, "exit\n") == 0){break;}

        int raw_input_len = split_input(raw_input, raw_input_arr);

        char** cmd1 = malloc(sizeof(char*) * raw_input_len);
        char** cmd2 = malloc(sizeof(char*) * raw_input_len);

        split_args(raw_input_len, raw_input_arr, cmd1, cmd2);

        // if pipe not set then cmd2 will not be populated
        if(!cmd2[0]) {
            int rc = run_one(cmd1[0], cmd1);
            if (0 != rc) {
                printf("Error return code, %d\n", errno);
            }
        } 

        if(cmd2[0] != NULL) {
            int rc = run_pipe(cmd2[0], cmd2, cmd1[0], cmd1);
            if (0 != rc){
                return 1;
            }
        }


    }

    free(raw_input);
    free(raw_input_arr);

    return 0;
}