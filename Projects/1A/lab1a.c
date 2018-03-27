//NAME: Jonathan Chu
//EMAIL: jonathanchu78@gmail.com
//ID: 004832220

#include <stdio.h>
#include <getopt.h>
#include <stdlib.h>
#include <unistd.h>
#include <termios.h>
#include <errno.h>
#include <string.h>
#include <poll.h>
#include <signal.h>
#include <sys/wait.h>

//declare flag
static int shell_flag;

//save attributes to be restored on exit
struct termios save;

extern int errno;

// interprocess pipes
int pipe2C[2]; //to child
int pipe2P[2]; //to parent

pid_t pid;

void restoreInput();

void setTerminal(){
	//save attributes to global to be restored later 
	tcgetattr(0, &save);
	atexit(restoreInput);
	
	struct termios attributes;
	tcgetattr(0, &attributes); //get working copy of attributes
	//modify a few
	attributes.c_iflag = ISTRIP;
	attributes.c_oflag = 0;
	attributes.c_lflag = 0;
	//set the new attributes
	if (tcsetattr(0, TCSANOW, &attributes) < 0){
		fprintf(stderr, "Error Setting Terminal Mode");
		exit(1);
	}
}

void restoreInput(){
	tcsetattr(STDIN_FILENO, TCSANOW, &save);
	if (shell_flag) {
		int status;
		if (waitpid(pid, &status, 0) == -1) {
			fprintf(stderr, "error on waitpid");
			exit(1);
		}
		if (WIFEXITED(status)) {
			const int es = WEXITSTATUS(status);
			const int ss = WTERMSIG(status);
			fprintf(stderr, "SHELL EXIT SIGNAL=%d STATUS=%d\n", ss, es);
			exit(0);
		}
	}
}

void createPipe(int p[2]){
	if (pipe(p) == -1){ //create pipe
		fprintf(stderr, "error creating pipe!");
		exit(1);
	}
}

void runChildShell(){
	//close pipes we dont need right now
	close(pipe2C[1]);
        close(pipe2P[0]);
        //copy fd's we need to 0 and 1
	dup2(pipe2C[0], 0);
	dup2(pipe2P[1], 1);
	//close original versions
	close(pipe2C[0]);
	close(pipe2P[1]);
	
	char path[] = "/bin/bash";
	char *args[2] = { path, NULL };
	if (execvp(path, args) == -1){ //execute shell
		fprintf(stderr, "error: %s", strerror(errno));
		exit(1);
	}
}

void shellCopy(int fd1, int fd2);
void runParentIO(){
	close(pipe2C[0]);
	close(pipe2P[1]);
	shellCopy(0, 1);
}

void copy(){
	char buf[256];
	int bytes_read = read(0,buf, sizeof(char)*256);
	char crlf[2] = {'\r', '\n'};
	while (bytes_read){
		int i = 0;
		for (;i<bytes_read;i++){
			switch (buf[i]) {
				case '\4':
					exit(0);
					break;
				case '\r':
				case '\n':
					write(1, &crlf, 2*sizeof(char));
					continue;
				default:
					write(1, &buf[i] , sizeof(char));
			}
		}
		//clear buffer 
		memset(buf, 0, bytes_read);
		bytes_read = read(0, buf, sizeof(char)*256);
	}
}
	
void shellCopy(int fd1, int fd2) {
	//poll example referenced: http://www.unixguide.net/unix/programming/2.1.2.shtml
	struct pollfd poll_list[2];
	poll_list[0].fd = fd1; //input from keyboard
	poll_list[1].fd = pipe2P[0]; //pipe output to parent for writing
	poll_list[0].events = poll_list[1].events = POLLIN | POLLHUP | POLLERR;

 	int retval;
    char buf[256];
    char cur;
	char crlf[2] = {'\r', '\n'};

    while (1) {
       
        retval = poll(poll_list, 2, 0);
        if (retval < 0) {
            fprintf(stderr, "error: error while polling\n");
            exit(1);
        }

       	
        //read keyboard input
        if ((poll_list[0].revents & POLLIN)) {
        	int bytes_read = read(fd1,buf, sizeof(char)*256);
            if (bytes_read < 0){
            	fprintf(stderr, "Error: failed to read from keyboard\n");
            	exit(1);
            } 
			
            int i = 0;
    		for (;i<bytes_read;i++){	
    			cur = buf[i];
				switch (cur){
				case '\4':
					close(pipe2C[1]);
					break;
				case '\3':
					kill(pid, SIGINT);
					break;
				case '\r':
				case '\n':
					write(fd2, &crlf, 2*sizeof(char));
					char c = '\n';
					write(pipe2C[1], &c, sizeof(char));
					continue;
				default:
					write(fd2, &cur, sizeof(char));
					write(pipe2C[1], &buf, sizeof(char));
				}
	        }
	        memset(buf, 0, bytes_read);

        	if (bytes_read < 0){
            	fprintf(stderr, "Error: read from keyboard failed\n");
            	exit(1);
            } 
        }

        //read from shell pollfd
        if ((poll_list[1].revents & POLLIN)) {
        	int bytes_read = read(pipe2P[0],buf, sizeof(char)*256);
        	int i = 0;
            if (bytes_read < 0){
            	fprintf(stderr, "Error: read from shell failed\n");
            	exit(1);
            } 

    		for (; i < bytes_read; i++){
	            cur = buf[i];
				switch (cur){
				case '\n':
					write(1, &crlf, 2*sizeof(char));
					continue;
				default:
					write(1, &cur, sizeof(char));
				}
            }
            memset(buf, 0, bytes_read);

        }

    	if ((POLLERR | POLLHUP) & (poll_list[1].revents)) {
    	    exit(0);
    	}
    }
}

int main(int argc, char **argv) {
	//define CL option 'shell'
	static struct option long_options[] = {
          {"shell",   no_argument, &shell_flag,    1}
        };
	//var to store index of command that getopt finds
	int opt_index;

        int c;
        while (1){
                c = getopt_long(argc, argv, "", long_options, &opt_index);
                if (c == -1)
                        break;
                if (c != 0){
                        fprintf(stderr, "error: invalid argument\n");
                        exit(1);
            	}

	}
	
	//set terminal input mode
	setTerminal();
	
	//create pipes
	createPipe(pipe2C);
	createPipe(pipe2P);

	if (shell_flag){
		pid = fork(); //fork new process
		if (pid < -1){
			fprintf(stderr, "error: %s", strerror(errno));
			exit(1);
		}
		else if (pid == 0){ //child process will redirect to shell
			runChildShell();
		}
		else{ //parent process will do writing
			runParentIO(); //modularity ftw
		}
	}
		
	
	copy(0, 1);
	
	exit(0);
}











