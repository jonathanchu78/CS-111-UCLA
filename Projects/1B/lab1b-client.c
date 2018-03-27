//NAME: Jonathan Chu
//EMAIL: jonathanchu78@gmail.com
//ID: 004832220

#define _GNU_SOURCE
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


//declare flags
int port_flag;
int log_flag;
int compress_flag;

//arguments
int port_num;
char* log_file = NULL;

//save attributes to be restored on exit
struct termios save;

int sfd;

extern int errno;

//initialize compression streams
z_stream in_shell;
z_stream shell_out;

void restoreInput();

void setTerminal(){
	//save attributes to global to be restored later 
	tcgetattr(0, &save);
	atexit(restoreInput);
	
	if (!isatty(STDIN_FILENO)) {
        fprintf(stderr, "stdin not a terminal.\n");
        exit(EXIT_FAILURE);
    }
	
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
}

int log_fd;
	
void sendLog(char cur){
	char c = '\n';
	write(log_fd, "SENT 1 byte: ", 13);
	write(log_fd, &cur, sizeof(char));
	write(log_fd, &c, sizeof(char));
}

void receiveLog(char cur){
	char c = '\n';
	write(log_fd, "RECEIVED 1 byte: ", 17);
	write(log_fd, &cur, sizeof(char));
	write(log_fd, &c, sizeof(char));
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
	int fd1 = 0;
	poll_list[0].fd = fd1; //input from keyboard
	poll_list[1].fd = sfd; //socket fd
	poll_list[0].events = poll_list[1].events = POLLIN | POLLHUP | POLLERR | POLLRDHUP;

 	int retval;
    unsigned char buf[256];
	unsigned char compression_buf[256];
	const int compression_buf_size = 256;
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
        		int bytes_read = read(fd1, buf, sizeof(char)*256);
        	    if (bytes_read < 0){
        	    	fprintf(stderr, "Error: failed to read from keyboard\n");
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
					
					//write to socket
					//write(1, &compression_buf, compression_buf_size - in_shell.avail_out); 
					write(sfd, &compression_buf, compression_buf_size - in_shell.avail_out);
					
					if (log_flag){
						int stop0 = compression_buf_size - in_shell.avail_out;
						int j = 0;
						for (; j<stop0; j++){	
							cur = compression_buf[j];
							//write(sfd, &cur, sizeof(char));
							sendLog(cur);
						}
					}
					
					
					deflateEnd(&in_shell);
					//if (deflateEnd(&in_shell) != Z_OK){
					//	fprintf(stderr, "error on deflate end");
					//	exit(1);
					//}
				}
				
				
				//write byte by byte to stdout and to socket if no --compress
        	    int i = 0;
    			for (;i<bytes_read;i++){	
    				cur = buf[i];
					switch (cur){
					case '\r':
					case '\n':
						write(1, &crlf, 2*sizeof(char));
						if (!compress_flag){ 
							write(sfd, &cur, sizeof(char)); 
							if (log_flag){
								sendLog(cur);
							}
						}
						continue;
					default:
						write(1, &cur, sizeof(char));
						if (!compress_flag){ 
							write(sfd, &cur, sizeof(char)); 
							if (log_flag){
								sendLog(cur);
							}
						}
					}
		        }
		        memset(buf, 0, bytes_read);
	
	        	if (bytes_read < 0){
	            	fprintf(stderr, "Error: read from keyboard failed\n");
	            	exit(1);
	            } 
	        }

        	//read from socket pollfd
        	if ((poll_list[1].revents & POLLIN)) {
        		int bytes_read = read(sfd, buf, sizeof(char)*256);
				
				if (bytes_read < 0){
        	    	fprintf(stderr, "Error: read from shell failed\n");
        	    	exit(1);
        	    } 
				if (bytes_read == 0){
					exit(0);
				}
				
				if (log_flag){
					int j = 0;
					for (; j<bytes_read; j++){	
						cur = buf[j];
						//write(sfd, &cur, sizeof(char));
						receiveLog(cur);
					}
				}
				
				if (compress_flag && bytes_read > 0){
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
					for (; i < stop; i++){
						cur = compression_buf[i];
						/*if (log_flag){
							receiveLog(cur);
						}*/
						switch (cur){
						case '\n':
						case '\r':
							write(1, &crlf, 2*sizeof(char));
							continue;
						default:
							write(1, &cur, sizeof(char));
						}
					}
				}
				else{
					int i = 0;
					for (; i < bytes_read; i++){
						cur = buf[i];
						if (log_flag){
							receiveLog(cur);
						}
						switch (cur){
						case '\n':
						case '\r':
							write(1, &crlf, 2*sizeof(char));
							continue;
						default:
							write(1, &cur, sizeof(char));
						}
					}
				}
				memset(compression_buf, 0, bytes_read);
				
        	}

    	if ((POLLERR | POLLHUP | POLLRDHUP) & (poll_list[1].revents)) {
    	    exit(0);
    	}
   }
}

int main(int argc, char **argv) {
	static struct option long_options[] = {
        {"port",		required_argument, &port_flag,	1},
		{"log",			required_argument, &log_flag,	1},
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
		if (opt_index == 1){
			log_file = optarg;
		}
	}

	if (!port_flag){
		fprintf(stderr, "error: must specify port number!");
		exit(1);
	}
	
	if (log_flag)
		log_fd = open(log_file, O_WRONLY|O_CREAT|O_TRUNC, 0666);
	
	
	struct hostent *server;
    struct sockaddr_in server_address;
	
	//set up socket, get file descriptor
    sfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sfd < 0) { 
		fprintf(stderr, "error opening socket\n"); 
		exit(0); 
	}
    server = gethostbyname("127.0.0.1");
    if (server == NULL) { 
		fprintf(stderr, "error finding host\n"); 
		exit(0); 
	}

    //set up server address
    memset((char*) &server_address, 0, sizeof(server_address));
    server_address.sin_family = AF_INET;
    memcpy((char*) &server_address.sin_addr.s_addr, (char*) server->h_addr, server->h_length);
    server_address.sin_port = htons(port_num);

    if (connect(sfd, (struct sockaddr *) &server_address, sizeof(server_address)) < 0) {
        fprintf(stderr, "error: error in connecting\n");
        exit(0);
    }
	
	//set terminal input mode
	setTerminal();
	
	copy(sfd);
	
	exit(0);
}











