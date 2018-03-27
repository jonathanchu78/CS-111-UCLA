//NAME: Jonathan Chu
//EMAIL: jonathanchu78@gmail.com
//ID: 004832220

#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <time.h>
#include <pthread.h>
#include <string.h>

//declare global variables
long long counter = 0;
pthread_mutex_t mutex;
int spinLock = 0;

//declare argument flags
int threads_flag;
int iterations_flag;
int yield_flag = 0;
int sync_flag;

//declare argument values
int num_threads = 1;
int num_iterations = 1;
int lock_type = 0; //default no lock

//declare lock type flags
const int NONE = 0;
const int MUTEX = 1;
const int SPIN_LOCK = 2;
const int COMPARE_AND_SWAP = 3;


//the original add function
void add(long long *pointer, long long value) {
		long long sum = *pointer + value;
		//printf("%d\n", counter);
		if (yield_flag) { sched_yield(); }
		*pointer = sum;
}

void* test(){
	long long value = 1;
	for (; value > -2; value -= 2){
		int i = 0;
		for (; i < num_iterations; i++){
			switch (lock_type) {
				case 0: //NONE
					add(&counter, value);
					break;
				case 1: //MUTEX
					pthread_mutex_lock(&mutex);
					add(&counter, value);
					pthread_mutex_unlock(&mutex);
					break;
				case 2: //SPIN_LOCK
					while (__sync_lock_test_and_set(&spinLock, 1));
					add(&counter, value);
					__sync_lock_release(&spinLock);
					break;
				case 3: //COMPARE_AND_SWAP
					;
					long long old;
					long long new;
					do {
						old = counter;
						new = old + value;
						if (yield_flag) { sched_yield(); }
					} while (__sync_val_compare_and_swap(&counter, old, new) != old);
					break;
			}
		}
	}
	return NULL;
}

char str[20] = "";
//function to get string after "add" in output
void getTag(){
	if (yield_flag) strcat(str, "-yield");
	
	switch (lock_type){
		case 0: strcat(str, "-none"); break;
		case 1: strcat(str, "-m"); break;
		case 2: strcat(str, "-s"); break;
		case 3: strcat(str, "-c"); break;
	}
}

int main(int argc, char **argv){
	//process options with getopt_long
	static struct option long_options[] = {
        {"threads",		required_argument, &threads_flag,	 1},
		{"iterations",	required_argument, &iterations_flag, 1},
		{"yield",		no_argument, 	   &yield_flag,		 1},
		{"sync",		required_argument, &sync_flag,		 1}
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
			num_threads = atoi(optarg);
		}
		if (opt_index == 1){
			num_iterations = atoi(optarg);
		}
		if (opt_index == 3 && strlen(optarg) == 1){
			switch (optarg[0]){
				case 'm':
					lock_type = MUTEX; break;
				case 's':
					lock_type = SPIN_LOCK; break;
				case 'c':
					lock_type = COMPARE_AND_SWAP; break;
				default:
					fprintf(stderr, "error: invalid sync argument\n");
					exit(1);
			}
		}
	}

	//initialize needed variables
	if (lock_type == MUTEX)
		pthread_mutex_init(&mutex, NULL);
	
	
	//collect start time
	struct timespec start_time;
	clock_gettime(CLOCK_MONOTONIC, &start_time);
	
	//start threads
	pthread_t *tids = malloc(num_threads * sizeof(pthread_t));
	int i = 0;
	for (; i < num_threads; i++){
		if (pthread_create(&tids[i], NULL, test, NULL)){
			fprintf(stderr, "error creating thread(s)\n");
			exit(1);
		}
			
	}
	
	//wait for all threads to exit
	i = 0;
	for (; i < num_threads; i++){
		if (pthread_join(tids[i], NULL)){
			fprintf(stderr, "error joining threads\n");
			exit(1);
		}
	}
	
	//collect end time
	struct timespec end_time;
	clock_gettime(CLOCK_MONOTONIC, &end_time);
	
	free(tids);
	
	//calculate the elapsed time
	long long elapsed_time = (end_time.tv_sec - start_time.tv_sec) * 1000000000;
	elapsed_time += end_time.tv_nsec;
	elapsed_time -= start_time.tv_nsec;
	int num_operations = num_threads * num_iterations  * 2;
	int avg_op_time = elapsed_time / num_operations;
	
	//report data
	getTag();
    printf("add%s,%d,%d,%d,%lld,%d,%lld\n", str, num_threads, num_iterations, 
		num_operations, elapsed_time, avg_op_time, counter);
	
	exit(0);
}





