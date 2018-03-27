//NAME: Jonathan Chu
//EMAIL: jonathanchu78@gmail.com
//ID: 004832220

#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#include <getopt.h>
#include <poll.h>
#include <signal.h>
#include <string.h>
#include <sys/types.h>
#include <sys/errno.h>
#include <time.h>
#include <sys/time.h>
#include <math.h>
#include <mraa.h>
#include <mraa/aio.h>


int period_flag;
int scale_flag;
int log_flag;

int period = 1;
char scale = 'F';
FILE* log_file = 0;

int paused = 0; //we're outputting time and temperature


//declare inputs
mraa_aio_context tempSensor;
mraa_gpio_context button;

double getTemp(){
	int reading = mraa_aio_read(tempSensor);
	//int reading = 5;
	int B = 4275; //thermistor value
	double temp = 1023.0 / ((double)reading) - 1.0;
	temp *= 100000.0;
	double C = 1.0 / (log(temp/100000.0)/B + 1/298.15) - 273.15;
	switch (scale){
		case 'F': return C * 9/5 + 32;
		default: return C;
	}
}

int checkMatch(const char* str1, const char* str2){
	int k = 0;
	if (strlen(str1) != strlen(str2) + 1){
		return 0;
	}
	int len = strlen(str2);
	for (; k < len; k++){
		if (str1[k] == '\n') break;
		if (str1[k] != str2[k])
			return 0;
	}
	return 1;
}

void printCmd(const char* cmd){
	if (log_flag)
		fprintf(log_file, "%s\n", cmd);
	fflush(log_file);
}

void shutDown(){
	struct timeval clk;
	struct tm* now;
	gettimeofday(&clk, 0);
	now = localtime(&(clk.tv_sec));
	char buf_out[128];
	sprintf(buf_out, "%02d:%02d:%02d SHUTDOWN\n", now->tm_hour, now->tm_min, now->tm_sec);
	fputs(buf_out, stdout);
	if (log_flag)
		fputs(buf_out, log_file);
	exit(0);
}

void handleLog(const char* cmd){
	if (!log_flag)
		return;
	fputs(cmd, log_file);
}

int handlePeriod(const char* cmd){
	char per_str[] = "PERIOD=";
	int i = 0;
	while (cmd[i] != '\0' && per_str[i] != '\0'){
		if (cmd[i] != per_str[i])
			return 0;
		i++;
	}
	int len = strlen(cmd) - 1;
	if (i != 7) return 0;
	while (i < len){
		if (!isdigit(cmd[i])){
			return 0;
		}
		i++;
	}
	return atoi(&cmd[7]);
}

void cmdHandler(const char* cmd){
	if (checkMatch(cmd, "OFF")){
		printCmd("OFF");
		shutDown();
	}
	else if (checkMatch(cmd, "STOP")) { paused = 1; printCmd("STOP"); }
	else if (checkMatch(cmd, "START")) { paused = 0; printCmd("START"); }
	else if (checkMatch(cmd, "SCALE=F")) { scale = 'F'; printCmd("SCALE=F"); }
	else if (checkMatch(cmd, "SCALE=C")) { scale = 'C'; printCmd("SCALE=C"); }
	else if (strlen(cmd) > 4 && cmd[0] == 'L' && cmd[1] == 'O' && cmd[2] == 'G' && cmd[3] == ' '){
		handleLog(cmd);
	}
	else{
		int new_period = handlePeriod(cmd);
		if (new_period == 0){ printCmd(cmd); printCmd("error: invalid command\n"); exit(1); }
		printCmd(cmd);
		period = new_period;
	}
}

int main(int argc, char **argv){
	//process options with getopt_long
        static struct option long_options[] = {
                {"period",	required_argument, &period_flag,	1},
                {"scale",	required_argument, &scale_flag,		1},
                {"log",		required_argument, &log_flag,		1},
		{0,		0,		   0,			0}
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
                        period = atoi(optarg);
                }
                if (opt_index == 1){
			if (strlen(optarg) != 1 || (optarg[0] != 'C' && optarg[0] != 'F')){
				fprintf(stderr, "unrecognized scale argument\n");
				exit(1);
			}
                        scale = optarg[0];
                }
                if (opt_index == 2){
                        log_file = fopen(optarg, "w");;
                }
	}
	
	//printf("point 1\n"
	//initialize I/O pins
	tempSensor = mraa_aio_init(1);
	button = mraa_gpio_init(62);
	mraa_gpio_dir(button, MRAA_GPIO_IN);

	//initialize poll stuff
	struct pollfd pollfds[1];
	pollfds[0].fd = STDIN_FILENO;
	pollfds[0].events = POLLIN | POLLHUP | POLLERR;

	//struct pollfd poll_stdin = { 0, POLLIN, 0 };
	char buf_out[128];
	struct timeval clk;
	time_t next = 0;
	struct tm* now;
	while (1){
		//get the time
		gettimeofday(&clk, 0);
		
		double temperature = getTemp();
		
		if (!paused && clk.tv_sec >= next){
			now = localtime(&(clk.tv_sec));
			sprintf(buf_out, "%02d:%02d:%02d %.1f\n", now->tm_hour, now->tm_min, now->tm_sec, temperature);
			fputs(buf_out, stdout);
			if (log_flag)
				fputs(buf_out, log_file);
			next = clk.tv_sec + period;
		}
		//poll for command
		int retval = poll(pollfds, 1, 0);
		if (retval < 0){
			fprintf(stderr, "error polling\n");
			exit(1);
		}
		
		if ((pollfds[0].revents & POLLIN)){
			char cmd[50];
			fgets(cmd, 50, stdin);
			cmdHandler(cmd);
		}
		
		if (mraa_gpio_read(button)){
			shutDown();
			//printf("button pressed\n");
		}
	}
	
	mraa_aio_close(tempSensor);
	mraa_gpio_close(button);
	exit(0);
}





