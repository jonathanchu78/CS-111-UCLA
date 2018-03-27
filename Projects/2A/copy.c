// NAME: Jeffrey Chan
// EMAIL: jeffschan97@gmail.com
// ID: 004-611-638

#include<stdio.h>
#include<stdlib.h>
#include<getopt.h>
#include<time.h>
#include<pthread.h>
#include<string.h>

#define BILL 1000000000L

// gloabl variables
int numThreads = 1;
int numIterations = 1;
int yieldFlag = 0;
char syncOption;
long long counter = 0;
pthread_mutex_t mutex;
int spinCondition = 0;
char returnString[50] = "";

// provided add function
void add (long long *pointer, long long value) {
    long long sum = *pointer + value;
    if (yieldFlag) {
        sched_yield();
    }
    *pointer = sum;
}

// function that wraps around add function
void* add_one (void *pointer) {
    pointer = (long long*) pointer;
    int i = 0;

    // increment counter by 1 for numIterations
    for (i = 0; i < numIterations; i++) {
        if (syncOption == 'm') {
            pthread_mutex_lock(&mutex);
            add(pointer, 1);
            pthread_mutex_unlock(&mutex);
        } else if (syncOption == 's') {
            while (__sync_lock_test_and_set(&spinCondition, 1));
            add(pointer, 1);
            __sync_lock_release(&spinCondition);
        } else if (syncOption == 'c') {
            long long prev;
            long long sum;
            do {
        	prev = counter;
        	sum = prev + 1;
        	if (yieldFlag) { sched_yield(); }
            } while (__sync_val_compare_and_swap(&counter, prev, sum) != prev);
        } else {
            add(pointer, 1);
        }
    }

    // decrement counter by 1 for numIterations
    for (i = 0; i < numIterations; i++) {
    	if (syncOption == 'm') {
    	    pthread_mutex_lock(&mutex);
    	    add(pointer, -1);
    	    pthread_mutex_unlock(&mutex);
    	} else if (syncOption == 's') {
    	    while (__sync_lock_test_and_set(&spinCondition, 1));
    	    add(pointer, -1);
    	    __sync_lock_release(&spinCondition);
    	} else if (syncOption == 'c') {
    	    long long prev;
    	    long long sum;
    	    do {
    		prev = counter;
    		sum = prev - 1;
    		if (yieldFlag) { sched_yield(); }
    	    } while (__sync_val_compare_and_swap(&counter,prev,sum) != prev);
    	} else {
    	    add(pointer, -1);
    	}
    }
}

char* getTestTag() {
    if (yieldFlag) { strcat(returnString, "-yield"); }

    if (!syncOption) { strcat(returnString, "-none"); }
    else if (syncOption == 'm') { strcat(returnString, "-m"); }
    else if (syncOption == 's') { strcat(returnString, "-s"); }
    else if (syncOption == 'c') { strcat(returnString, "-c"); }

    return returnString;
}

int main(int argc, char **argv) {
    int option = 0; // used to hold option

    // arguments this program supports
    static struct option options[] = {
        {"threads", required_argument, 0, 't'},
        {"iterations", required_argument, 0, 'i'},
        {"yield", no_argument, 0, 'y'},
        {"sync", required_argument, 0, 's'}
    };

    // iterate through options
    while ((option = getopt_long(argc, argv, "t:i:y:s", options, NULL)) != -1) {
    	switch (option) {
    	    case 't':
                numThreads = atoi(optarg);
                break;
    	    case 'i':
                numIterations = atoi(optarg);
                break;
    	    case 'y':
        		yieldFlag = 1;
        		break;
    	    case 's':
        		if ((optarg[0] == 'm' || optarg[0] == 's' || optarg[0] == 'c') && strlen(optarg) == 1) {
        		    syncOption = optarg[0];
        		} else {
        		    fprintf(stderr, "error: unrecognized sync argument\n");
        		    exit(1);
        		}
    		          break;
    	    default:
        		fprintf(stderr, "error: unrecognized argument\nrecognized arguments:\n--threads=#\n--iterations=#\n--yield\n--sync=[msc]\n");
        		exit(1);
    	}
    }

    // initialize mutex if needed
    if (syncOption == 'm') { pthread_mutex_init(&mutex, NULL); }

    // start timing
    struct timespec start;
    if (clock_gettime(CLOCK_MONOTONIC, &start) == -1) { fprintf(stderr, "error: error in getting start time\n"); exit(1); }

    // creates the number of specified threads
    pthread_t *threads = malloc(numThreads * sizeof(pthread_t));
    int k;
    for (k = 0; k < numThreads; k++) {
    	if (pthread_create(threads + k, NULL, add_one, &counter)) {
    	    fprintf(stderr, "error: error in thread creation\n");
    	    exit(1);
    	}
    }

    // wait for all the threads to complete
    for (k = 0; k < numThreads; k++) {
    	if (pthread_join(*(threads + k), NULL)) {
    	    fprintf(stderr, "error: error in joining threads\n");
    	    exit(1);
    	}
    }

    // stop timing
    struct timespec end;
    if (clock_gettime(CLOCK_MONOTONIC, &end) == -1) { fprintf(stderr, "error: error in getting stop time\n"); exit(1); }

	// free memory
	free(threads);

    long totalTime = BILL * (end.tv_sec - start.tv_sec) + (end.tv_nsec - start.tv_nsec); // calculate total time elapsed
    int numOperations = numThreads * numIterations * 2; // calculate number of operations
    int timePerOperation = totalTime / numOperations; // calculate the total time cost per operations

    char* testTag = getTestTag(); // get tag that corresponds to test we're running
    printf("add%s,%d,%d,%d,%d,%d,%d\n", testTag, numThreads, numIterations, numOperations, totalTime, timePerOperation, counter);

    exit(0);
}