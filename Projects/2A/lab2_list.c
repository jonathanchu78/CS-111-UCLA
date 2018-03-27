//NAME: Jonathan Chu
//EMAIL: jonathanchu78@gmail.com
//ID: 004832220

#include "SortedList.h"

#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <time.h>
#include <pthread.h>
#include <string.h>
#include <signal.h>

//declare global variables
long long counter = 0;
pthread_mutex_t mutex;
int spinLock = 0;
SortedList_t* list;
SortedListElement_t* elements;
char yieldopts[10] = "-";

//declare argument flags
int threads_flag;
int iterations_flag;
int yield_flag = 0;
int sync_flag;

//declare argument values
int num_threads = 1;
int num_iterations = 1;
int num_elements;
int lock_type = 0; //default no lock
int opt_yield = 0;

//declare lock type flags
const int NONE = 0;
const int MUTEX = 1;
const int SPIN_LOCK = 2;

void setYield(char* optarg){
	int len = strlen(optarg);
	if (len > 3){
		fprintf(stderr, "too many arguments to yield\n");
		exit(1);
	}

	int i = 0;
	for (; i < len; i++){
		switch (optarg[i]){
			case 'i': opt_yield |= INSERT_YIELD; strcat(yieldopts, "i"); break;
			case 'd': opt_yield |= DELETE_YIELD; strcat(yieldopts, "d"); break;
			case 'l': opt_yield |= LOOKUP_YIELD; strcat(yieldopts, "l"); break;
			default:
				fprintf(stderr, "invalid argument to yield!\n");
				exit(1);
		}
	}
}

void handler(){
	fprintf(stderr, "error: segmentation fault\n");
	exit(2);
}

void initializeKeys(){
	srand(time(NULL));
	int i = 0;
	for (; i < num_elements; i++){
		int rand_char = (rand() % 26) + 'a';

		char* rand_key = malloc(2*sizeof(char));
		rand_key[0] = rand_char;
		rand_key[1] = '\0';
		
		elements[i].key = rand_key;
	}
}

void deleteElement(SortedListElement_t* element){
	if (element == NULL){
		fprintf(stderr, "error on SortedList_lookup\n");
		exit(2);
	}

	if (SortedList_delete(element)){
		fprintf(stderr, "error on SortedList_delete\n");
		exit(2);
	}
}

void* listWork(void* tid){
	//insert
	int i = *(int*)tid;
	//printf("%d\n", i);
	for (; i < num_elements; i += num_threads){
		switch (lock_type){
			case 0: //none
				SortedList_insert(list, &elements[i]); //printf(elements[i], "\n");
				break;
			case 1: //mutex
				pthread_mutex_lock(&mutex);
				SortedList_insert(list, &elements[i]);
				pthread_mutex_unlock(&mutex);
				break;
			case 2: //spin lock (test and set)
				while (__sync_lock_test_and_set(&spinLock, 1));
				SortedList_insert(list, &elements[i]);
				__sync_lock_release(&spinLock);
				break;
		}
	}

	//get length
	int len;
	switch (lock_type){
		case 0: //none
			len = SortedList_length(list);
			if (len == -1){
				fprintf(stderr, "error on SortedList_length\n");
				exit(2);
			}
			break;
		case 1: //mutex
			pthread_mutex_lock(&mutex);
			len = SortedList_length(list);
			if (len == -1){
				fprintf(stderr, "error on SortedList_length\n");
				exit(2);
			}
			//printf("%d\n", len);
			pthread_mutex_unlock(&mutex);
			break;
		case 2: //spin lock (test and set)
			while (__sync_lock_test_and_set(&spinLock, 1));
			len = SortedList_length(list);
			if (len == -1){
				fprintf(stderr, "error on SortedList_length\n");
				exit(2);
			}
			__sync_lock_release(&spinLock);
			break;
	}

	//delete old elements
	SortedListElement_t* element;
	for (i = *(int*)tid; i < num_elements; i += num_threads){
		switch (lock_type){
			case 0: //none
				element = SortedList_lookup(list, elements[i].key);
				deleteElement(element);
				break;
			case 1: //mutex
				pthread_mutex_lock(&mutex);
				element = SortedList_lookup(list, elements[i].key);
				deleteElement(element);
				pthread_mutex_unlock(&mutex);
				break;
			case 2: //spin lock (test and set)
				while (__sync_lock_test_and_set(&spinLock, 1));
				element = SortedList_lookup(list, elements[i].key);
				deleteElement(element);
				__sync_lock_release(&spinLock);
				break;
		}
	}
	return NULL; //function must be void* to fit pthread_create argument types
}

char tag[50] = "list";
void getTag(){
	if (strlen(yieldopts) == 1)
		strcat(yieldopts, "none");
	
	strcat(tag, yieldopts);

	switch (lock_type){
		case 0: strcat(tag, "-none"); break;
		case 1: strcat(tag, "-m"); break;
		case 2: strcat(tag, "-s"); break;
	}
}

int main(int argc, char **argv){
	//process options with getopt_long
	static struct option long_options[] = {
        {"threads",		required_argument, &threads_flag,	 1},
		{"iterations",	required_argument, &iterations_flag, 1},
		{"yield",		required_argument, 	   &yield_flag,		 1},
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
		if (opt_index == 2){
			setYield(optarg);
		}
		if (opt_index == 3 && strlen(optarg) == 1){
			switch (optarg[0]){
				case 'm':
					lock_type = MUTEX; break;
				case 's':
					lock_type = SPIN_LOCK; break;
				default:
					fprintf(stderr, "error: invalid sync argument\n");
					exit(1);
			}
		}
	}
	
	signal(SIGSEGV, handler);

	//initialize needed variables
	if (lock_type == MUTEX)
		pthread_mutex_init(&mutex, NULL);
	
	// initialize empty list
	list = malloc(sizeof(SortedList_t));
	list->next = list;
	list->prev = list;
	list->key = NULL;
	
	//create list elements
	num_elements  = num_threads * num_iterations;
	elements = malloc(num_elements*sizeof(SortedListElement_t));
	initializeKeys();
	
	//create threads
	pthread_t* threads = malloc(num_threads * sizeof(pthread_t));
	int* tids = malloc(num_threads * sizeof(int));
	
	//start timing
	struct timespec start_time;
	clock_gettime(CLOCK_MONOTONIC, &start_time);

	//run threads 
	int i = 0;
	for (; i < num_threads; i++){
		tids[i] = i;
		if (pthread_create(threads + i, NULL, listWork, &tids[i])){
			fprintf(stderr, "error on pthread_create\n");
			exit(1);
		}
	}
	//wait for threads
	for (i = 0; i < num_threads; i++){
		int ret = pthread_join(*(threads + i), NULL);
		if (ret){
			fprintf(stderr, "error on pthread_join\n");
			exit(1);
		}
	}
	//stop timing
	struct timespec end_time;
	clock_gettime(CLOCK_MONOTONIC, &end_time);
	
	//free allocated memory
	free(elements);
	free(tids);
	
	//calculate metrics
	long long elapsed_time = (end_time.tv_sec - start_time.tv_sec) * 1000000000;
	elapsed_time += end_time.tv_nsec;
	elapsed_time -= start_time.tv_nsec;
	int num_operations = num_threads * num_iterations * 3;
	int avg_op_time = elapsed_time / num_operations;
	
	//report data
	getTag();
	printf("%s,%d,%d,1,%d,%lld,%d\n", tag, num_threads, num_iterations, num_operations, elapsed_time, avg_op_time);
	
	if (SortedList_length(list) != 0)
		exit(2); //list was not cleared

	exit(0);
}




