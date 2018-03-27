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
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <zlib.h>

//declare flag
int port_flag;
int compress_flag;

//save attributes to be restored on exit
struct termios save;

extern int errno;

// interprocess pipes
int pipe2C[2]; //to child
int pipe2P[2]; //to parent

pid_t pid;
int sfd1, sfd2; //socket fd's

//initialize compression streams
z_stream in_shell;
z_stream shell_out;

void createPipe(int p[2]){
	if (pipe(p) == -1){ //create pipe
		fprintf(stderr, "error creating pipe!");
		exit(1);
	}
}

void sigHandler(int sig){
	if(sig == SIGINT)
		kill(pid, SIGINT);
	if(sig == SIGPIPE)
		exit(1);
}

void runShell(){
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

void closeServer() {
	//close connections
    close(pipe2C[1]);
    close(pipe2P[0]);
    close(sfd1);
    close(sfd2);
	

	int status;
    if (waitpid(pid, &status, 0) == -1) {
        fprintf(stderr, "error on waitpid");
        exit(1);
    }
    if (WIFEXITED(status)) {
        const int sig = WEXITSTATUS(status);
        const int stat = WTERMSIG(status);
		//printf("SHELL EXIT SIGNAL=%d STATUS=%d\n", sig, stat);
        fprintf(stderr, "SHELL EXIT SIGNAL=%d STATUS=%d\n", sig, stat);
        exit(0);
    }
	/*
	int status = 0;
	if(waitpid(pid, &status, 0) < 0){
		fprintf(stderr, "Error waiting: %s\n", strerror(errno));
		exit(1);
	}
	if(WIFEXITED(status) || WIFSIGNALED(status)) {
		int sig = WTERMSIG(status);
		int stat = WEXITSTATUS(status);
		fprintf(stderr, "SHELL EXIT SIGNAL=%d STATUS=%d\n", sig, stat);
		exit(0);
	}*/
}
	
void initializeCompression(){
	in_shell.zalloc = Z_NULL;
	in_shell.zfree = Z_NULL;
	in_shell.opaque = Z_NULL;

	if (deflateInit(&in_shell, Z_DEFAULT_COMPRESSION) != Z_OK){
		fprintf(stderr, "error on deflateInit");
		exit(1);
	}
}

void initializeDecompression(){
	shell_out.zalloc = Z_NULL;
	shell_out.zfree = Z_NULL;
	shell_out.opaque = Z_NULL;

	if (inflateInit(&shell_out) != Z_OK){
		fprintf(stderr, "error on inflateInit");
		exit(1);
	}
}

void copy(int sfd) {
	//poll example referenced: http://www.unixguide.net/unix/programming/2.1.2.shtml
	struct pollfd poll_list[2];
	poll_list[0].fd = sfd; //input from keyboard
	poll_list[1].fd = pipe2P[0]; //pipe output to parent for writing
	poll_list[0].events = poll_list[1].events = POLLIN | POLLHUP | POLLERR;

 	int retval;
    unsigned char buf[256];
	unsigned char compression_buf[256];
	const int compression_buf_size = 256;
    char cur;
	//char crlf[2] = {'\r', '\n'};

    while (1) {
		int status;
		if (waitpid(pid, &status, WNOHANG) == -1) {
			fprintf(stderr, "error on waitpid");
			char bull = 'F';
			write(1, &bull, 1);
			exit(1);
		}
		
        retval = poll(poll_list, 2, 0);
        if (retval < 0) {
            fprintf(stderr, "error: error while polling\n");
            exit(1);
        }

       	
        //read socket input
        if (poll_list[0].revents & POLLIN) {
        	int bytes_read = read(sfd, buf, sizeof(char)*256);
            if (bytes_read < 0){
            	fprintf(stderr, "Error: failed to read from keyboard\n");
            	exit(1);
            } 
			
			if (compress_flag  && bytes_read > 0){
				//DECOMPRESS
				initializeDecompression();
				
				//assume compression buf large enough
				shell_out.avail_in = bytes_read; 
				shell_out.next_in = buf;
				shell_out.avail_out = compression_buf_size; 
				shell_out.next_out = compression_buf;

				//inflate
				do{
					if (inflate(&shell_out, Z_SYNC_FLUSH) != Z_OK){
						fprintf(stderr, "error inflating");
						exit(1);
					}
				} while (shell_out.avail_in > 0);
				
				inflateEnd(&shell_out);
				//if (inflateEnd(&shell_out) != Z_OK){
				//	fprintf(stderr, "error on inflate end");
				//	exit(1);
				//}
				
				int stop = compression_buf_size - shell_out.avail_out;
				int i = 0;
				for (;i<stop;i++){	
					cur = compression_buf[i];
					switch (cur){
					case '\4':
					case '\3':
						closeServer(1);
						break;
						//closeServer(0);
						//break;
					case '\r':
					case '\n':
						;
						char c = '\n';
						//write(1, &c, sizeof(char)); //test
						write(pipe2C[1], &c, sizeof(char));
						continue;
					default:
						//write(1, &cur, sizeof(char)); //test
						write(pipe2C[1], &cur, sizeof(char));
					}
				}
				memset(compression_buf, 0, stop);
				
			}
			else{ //no --compress; write to shell normally without decompressing
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
						;
						char c = '\n';
						write(pipe2C[1], &c, sizeof(char));
						continue;
					default:
						write(pipe2C[1], &buf, sizeof(char));
					}
				}
				memset(buf, 0, bytes_read);

				if (bytes_read < 0){
					fprintf(stderr, "Error: read from keyboard failed\n");
					exit(1);
				} 
			}
        }

		
        
		//read from shell pollfd
        if ((poll_list[1].revents & POLLIN)) {
        	int bytes_read = read(pipe2P[0],buf, sizeof(char)*256);
			
			//now working
			//write(1, &buf, bytes_read);
        	
			int i = 0;
            if (bytes_read < 0){
            	fprintf(stderr, "Error: read from shell failed\n");
            	exit(1);
            } 
			if (compress_flag  && bytes_read > 0){
				//COMPRESS
				initializeCompression();
				
				//assume compression buf large enough
				in_shell.avail_in = bytes_read; //INPUT BUF HERE
				in_shell.next_in = buf;
				in_shell.avail_out = compression_buf_size; //COMPRESSION BUF HERE
				in_shell.next_out = compression_buf;

				//deflate
				do{
					if (deflate(&in_shell, Z_SYNC_FLUSH) != Z_OK){
						fprintf(stderr, "error deflating");
						exit(1);
					}
				} while (in_shell.avail_in > 0);
				
				//write(1, &compression_buf, compression_buf_size - in_shell.avail_out);
				write(sfd, &compression_buf, compression_buf_size - in_shell.avail_out);
				
				deflateEnd(&in_shell);
				//if (deflateEnd(&in_shell) != Z_OK){
				//	fprintf(stderr, "error on deflate end");
				//	exit(1);
				//}
				
			}
			else{
				for (; i < bytes_read; i++){
					cur = buf[i];
					write(sfd, &cur, sizeof(char));
				}
				memset(buf, 0, bytes_read);
			}
			
			
				if ((poll_list[1].revents & (POLLHUP | POLLERR))) {
					fprintf(stderr, "error: POLLHUP|POLLERR\n");
					exit(0);
				}
        }
    }
}

int main(int argc, char **argv) {
	atexit(closeServer);
	signal(SIGINT, sigHandler);
	signal(SIGPIPE, sigHandler);
	
	int port_num;
	socklen_t  client_length;
	struct sockaddr_in server_address, client_address;
	
	static struct option long_options[] = {
        {"port",		required_argument, &port_flag,	1},
		{"compress",	no_argument, &compress_flag,	1}
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
		if (opt_index == 0){
			port_num = atoi(optarg);
		}
	}
	
	//set up socket
    sfd1 = socket(AF_INET, SOCK_STREAM, 0);
    if (sfd1 < 0) { 
		fprintf(stderr, "error opening socket\n"); 
		exit(1); 
	}
    memset((char*) &server_address, 0, sizeof(server_address));
	
    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(port_num);
    server_address.sin_addr.s_addr = INADDR_ANY;

    //bind host
    if (bind(sfd1, (struct sockaddr *) &server_address, sizeof(server_address)) < 0) {
        fprintf(stderr, "error binding socket");
        exit(1);
    }

    //listen for clients
    listen(sfd1, 5);
    client_length = sizeof(client_address);

    //connect to incoming client
    sfd2 = accept(sfd1, (struct sockaddr *) &client_address, &client_length);
    if (sfd2 < 0) {
        fprintf(stderr, "error accepting client");
        exit(1);
    }
	
	//create pipes
	createPipe(pipe2C);
	createPipe(pipe2P);

	pid = fork(); //fork new process
	if (pid < 0){
		fprintf(stderr, "error: %s", strerror(errno));
		exit(1);
	}
	else if (pid == 0){ //child process 
		runShell();
	}
	else{ //parent process 
		close(pipe2C[0]);
		close(pipe2P[1]);
		copy(sfd2);
	}
	
	exit(0);
}











