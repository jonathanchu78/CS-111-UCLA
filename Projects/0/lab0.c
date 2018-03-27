/*NAME: Jonathan Chu
EMAIL: jonathanchu78@gmail.com
ID: 004832220*/

#include "stdio.h"
#include <unistd.h>
#include <stdlib.h>
#include <getopt.h>
#include <signal.h>
#include <fcntl.h>
#include <string.h>
#include <errno.h>

//strerror example used: http://www.tutorialspoint.com/ansi_c/c_strerror.htm
extern int errno;

void catch(){
	fprintf(stderr, "error: segmentation fault\n");
	exit(4);
}

//referred to old 35L lab for read/write syntax
void copy(int fd1, int fd2){
	char buf;
 	while (1){
		if (read(fd1, &buf, sizeof(char)) == 0) //EOF
			break;
        	write(fd2, &buf, sizeof(char));
	}
}

int main (int argc, char **argv){
	//declare flags for options to set
	static int in_flag;
	static int seg_flag;
	static int out_flag;
	static int catch_flag;
	
	//declare arguments
	char* in_file = 0;
	char* out_file = 0;

	//define CL options
	//getopt reference used: http://man7.org/linux/man-pages/man3/getopt.3.html#RETURN_VALUE
	//getopt example used: https://www.gnu.org/software/libc/manual/html_node/Getopt-Long-Option-Example.html
	static struct option long_options[] =
	{
          {"input",   1, &in_flag,    1},
          {"output",  1, &out_flag,   1},
          {"segfault",0, &seg_flag,   1},
          {"catch",   0, &catch_flag, 1},
		  {0,         0, 0,  	      0}
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
			//printf("input flag set");
			in_file = optarg;
		}
		if (opt_index == 1){
			//printf("%s", "output flag set");
			out_file = optarg;
		}
	}
	/* test flags
	if (in_flag)
		printf("in\n");
	if (out_flag)
                printf("out\n");
	if (seg_flag)
                printf("seg\n");
	if (catch_flag)
                printf("catch\n");
	*/
	
	if (catch_flag)
                signal(SIGSEGV, catch);

	if (seg_flag){ //force segfault
		char *ptr = NULL;
		*ptr = 'c';
	}
	
	if (in_file){ //input file specified
		int ifd = open(in_file, O_RDONLY);
		if (ifd >= 0) { //replace read fd to point to specified file
			close(0);
			dup(ifd);
			close(ifd);
		}
		else{
			fprintf(stderr, "error: %s. Problematic input file: %s\n", strerror(errno), in_file);
			exit(2);
		}
	}

	if (out_file){ //output file specified
		int ofd = creat(out_file, 0666);
		if (ofd >= 0) { //replace write fd to point to specified file
			close(1);
			dup(ofd);
			close(ofd);
		}
		else{
			fprintf(stderr, "error: %s. Problematic output file: %s\n", strerror(errno), out_file);
			exit(3);
		}
	}
	
	copy(0, 1); //copy from read fd (0) to write fd (1)
	exit(0);
}
