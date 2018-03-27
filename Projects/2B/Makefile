#NAME: Jonathan Chu
#EMAIL: jonathanchu78@gmail.com
#ID: 004832220
.SILENT:

default:
	gcc -pthread -lrt  -Wall -Wextra -g SortedList.c  -o lab2_list lab2_list.c

tests: default
	echo " " > lab2b_list.csv
	#1, 2
	echo "running tests for 1 and 2"
	./lab2_list --threads=1  --iterations=1000 --sync=m   >> lab2b_list.csv
	./lab2_list --threads=2  --iterations=1000 --sync=m   >> lab2b_list.csv
	./lab2_list --threads=4  --iterations=1000 --sync=m   >> lab2b_list.csv
	./lab2_list --threads=8  --iterations=1000 --sync=m   >> lab2b_list.csv
	./lab2_list --threads=12 --iterations=1000 --sync=m   >> lab2b_list.csv
	./lab2_list --threads=16 --iterations=1000 --sync=m   >> lab2b_list.csv
	./lab2_list --threads=24 --iterations=1000 --sync=m   >> lab2b_list.csv
	echo "halfway through 1 and 2"
	./lab2_list --threads=1  --iterations=1000 --sync=s   >> lab2b_list.csv
	./lab2_list --threads=2  --iterations=1000 --sync=s   >> lab2b_list.csv
	./lab2_list --threads=4  --iterations=1000 --sync=s   >> lab2b_list.csv
	./lab2_list --threads=8  --iterations=1000 --sync=s   >> lab2b_list.csv
	./lab2_list --threads=12 --iterations=1000 --sync=s   >> lab2b_list.csv
	./lab2_list --threads=16 --iterations=1000 --sync=s   >> lab2b_list.csv
	./lab2_list --threads=24 --iterations=1000 --sync=s   >> lab2b_list.csv
	#3
	echo "running tests for 3"
	-./lab2_list --threads=1  --iterations=1   --yield=id --lists=4   >> lab2b_list.csv
	-./lab2_list --threads=1  --iterations=2   --yield=id --lists=4   >> lab2b_list.csv
	-./lab2_list --threads=1  --iterations=4   --yield=id --lists=4   >> lab2b_list.csv
	-./lab2_list --threads=1  --iterations=8   --yield=id --lists=4   >> lab2b_list.csv
	-./lab2_list --threads=1  --iterations=16  --yield=id --lists=4   >> lab2b_list.csv
	-./lab2_list --threads=4  --iterations=1   --yield=id --lists=4   >> lab2b_list.csv
	-./lab2_list --threads=4  --iterations=2   --yield=id --lists=4   >> lab2b_list.csv
	-./lab2_list --threads=4  --iterations=4   --yield=id --lists=4   >> lab2b_list.csv
	-./lab2_list --threads=4  --iterations=8   --yield=id --lists=4   >> lab2b_list.csv
	-./lab2_list --threads=4  --iterations=16  --yield=id --lists=4   >> lab2b_list.csv
	-./lab2_list --threads=8  --iterations=1   --yield=id --lists=4   >> lab2b_list.csv
	-./lab2_list --threads=8  --iterations=2   --yield=id --lists=4   >> lab2b_list.csv
	-./lab2_list --threads=8  --iterations=4   --yield=id --lists=4   >> lab2b_list.csv
	-./lab2_list --threads=8  --iterations=8   --yield=id --lists=4   >> lab2b_list.csv
	-./lab2_list --threads=8  --iterations=16  --yield=id --lists=4   >> lab2b_list.csv
	-./lab2_list --threads=12 --iterations=1   --yield=id --lists=4   >> lab2b_list.csv
	-./lab2_list --threads=12 --iterations=2   --yield=id --lists=4   >> lab2b_list.csv
	-./lab2_list --threads=12 --iterations=4   --yield=id --lists=4   >> lab2b_list.csv
	-./lab2_list --threads=12 --iterations=8   --yield=id --lists=4   >> lab2b_list.csv
	-./lab2_list --threads=12 --iterations=16  --yield=id --lists=4   >> lab2b_list.csv
	-./lab2_list --threads=16 --iterations=1   --yield=id --lists=4   >> lab2b_list.csv
	-./lab2_list --threads=16 --iterations=2   --yield=id --lists=4   >> lab2b_list.csv
	-./lab2_list --threads=16 --iterations=4   --yield=id --lists=4   >> lab2b_list.csv
	-./lab2_list --threads=16 --iterations=8   --yield=id --lists=4   >> lab2b_list.csv
	-./lab2_list --threads=16 --iterations=16  --yield=id --lists=4   >> lab2b_list.csv
	echo "1/3 way through 3"
	./lab2_list --threads=1  --iterations=1   --yield=id --lists=4  --sync=m   >> lab2b_list.csv
	./lab2_list --threads=1  --iterations=2   --yield=id --lists=4  --sync=m   >> lab2b_list.csv
	./lab2_list --threads=1  --iterations=4   --yield=id --lists=4  --sync=m   >> lab2b_list.csv
	./lab2_list --threads=1  --iterations=8   --yield=id --lists=4  --sync=m   >> lab2b_list.csv
	./lab2_list --threads=1  --iterations=16  --yield=id --lists=4  --sync=m   >> lab2b_list.csv
	./lab2_list --threads=4  --iterations=1   --yield=id --lists=4  --sync=m   >> lab2b_list.csv
	./lab2_list --threads=4  --iterations=2   --yield=id --lists=4  --sync=m   >> lab2b_list.csv
	./lab2_list --threads=4  --iterations=4   --yield=id --lists=4  --sync=m   >> lab2b_list.csv
	./lab2_list --threads=4  --iterations=8   --yield=id --lists=4  --sync=m   >> lab2b_list.csv
	./lab2_list --threads=4  --iterations=16  --yield=id --lists=4  --sync=m   >> lab2b_list.csv
	./lab2_list --threads=8  --iterations=1   --yield=id --lists=4  --sync=m   >> lab2b_list.csv
	./lab2_list --threads=8  --iterations=2   --yield=id --lists=4  --sync=m   >> lab2b_list.csv
	./lab2_list --threads=8  --iterations=4   --yield=id --lists=4  --sync=m   >> lab2b_list.csv
	./lab2_list --threads=8  --iterations=8   --yield=id --lists=4  --sync=m   >> lab2b_list.csv
	./lab2_list --threads=8  --iterations=16  --yield=id --lists=4  --sync=m   >> lab2b_list.csv
	./lab2_list --threads=12 --iterations=1   --yield=id --lists=4  --sync=m   >> lab2b_list.csv
	./lab2_list --threads=12 --iterations=2   --yield=id --lists=4  --sync=m   >> lab2b_list.csv
	./lab2_list --threads=12 --iterations=4   --yield=id --lists=4  --sync=m   >> lab2b_list.csv
	./lab2_list --threads=12 --iterations=8   --yield=id --lists=4  --sync=m   >> lab2b_list.csv
	./lab2_list --threads=12 --iterations=16  --yield=id --lists=4  --sync=m   >> lab2b_list.csv
	./lab2_list --threads=16 --iterations=1   --yield=id --lists=4  --sync=m   >> lab2b_list.csv
	./lab2_list --threads=16 --iterations=2   --yield=id --lists=4  --sync=m   >> lab2b_list.csv
	./lab2_list --threads=16 --iterations=4   --yield=id --lists=4  --sync=m   >> lab2b_list.csv
	./lab2_list --threads=16 --iterations=8   --yield=id --lists=4  --sync=m   >> lab2b_list.csv
	./lab2_list --threads=16 --iterations=16  --yield=id --lists=4  --sync=m   >> lab2b_list.csv
	echo "2/3 way through 3"
	./lab2_list --threads=1  --iterations=1   --yield=id --lists=4  --sync=s   >> lab2b_list.csv
	./lab2_list --threads=1  --iterations=2   --yield=id --lists=4  --sync=s   >> lab2b_list.csv
	./lab2_list --threads=1  --iterations=4   --yield=id --lists=4  --sync=s   >> lab2b_list.csv
	./lab2_list --threads=1  --iterations=8   --yield=id --lists=4  --sync=s   >> lab2b_list.csv
	./lab2_list --threads=1  --iterations=16  --yield=id --lists=4  --sync=s   >> lab2b_list.csv
	./lab2_list --threads=4  --iterations=1   --yield=id --lists=4  --sync=s   >> lab2b_list.csv
	./lab2_list --threads=4  --iterations=2   --yield=id --lists=4  --sync=s   >> lab2b_list.csv
	./lab2_list --threads=4  --iterations=4   --yield=id --lists=4  --sync=s   >> lab2b_list.csv
	./lab2_list --threads=4  --iterations=8   --yield=id --lists=4  --sync=s   >> lab2b_list.csv
	./lab2_list --threads=4  --iterations=16  --yield=id --lists=4  --sync=s   >> lab2b_list.csv
	./lab2_list --threads=8  --iterations=1   --yield=id --lists=4  --sync=s   >> lab2b_list.csv
	./lab2_list --threads=8  --iterations=2   --yield=id --lists=4  --sync=s   >> lab2b_list.csv
	./lab2_list --threads=8  --iterations=4   --yield=id --lists=4  --sync=s   >> lab2b_list.csv
	./lab2_list --threads=8  --iterations=8   --yield=id --lists=4  --sync=s   >> lab2b_list.csv
	./lab2_list --threads=8  --iterations=16  --yield=id --lists=4  --sync=s   >> lab2b_list.csv
	./lab2_list --threads=12 --iterations=1   --yield=id --lists=4  --sync=s   >> lab2b_list.csv
	./lab2_list --threads=12 --iterations=2   --yield=id --lists=4  --sync=s   >> lab2b_list.csv
	./lab2_list --threads=12 --iterations=4   --yield=id --lists=4  --sync=s   >> lab2b_list.csv
	./lab2_list --threads=12 --iterations=8   --yield=id --lists=4  --sync=s   >> lab2b_list.csv
	./lab2_list --threads=12 --iterations=16  --yield=id --lists=4  --sync=s   >> lab2b_list.csv
	./lab2_list --threads=16 --iterations=1   --yield=id --lists=4  --sync=s   >> lab2b_list.csv
	./lab2_list --threads=16 --iterations=2   --yield=id --lists=4  --sync=s   >> lab2b_list.csv
	./lab2_list --threads=16 --iterations=4   --yield=id --lists=4  --sync=s   >> lab2b_list.csv
	./lab2_list --threads=16 --iterations=8   --yield=id --lists=4  --sync=s   >> lab2b_list.csv
	./lab2_list --threads=16 --iterations=16  --yield=id --lists=4  --sync=s   >> lab2b_list.csv
	#4
	echo "running tests for 4"
	./lab2_list --threads=1  --iterations=1000  --lists=4  --sync=m   >> lab2b_list.csv
	./lab2_list --threads=2  --iterations=1000  --lists=4  --sync=m   >> lab2b_list.csv
	./lab2_list --threads=4  --iterations=1000  --lists=4  --sync=m   >> lab2b_list.csv
	./lab2_list --threads=8  --iterations=1000  --lists=4  --sync=m   >> lab2b_list.csv
	./lab2_list --threads=12 --iterations=1000  --lists=4  --sync=m   >> lab2b_list.csv
	./lab2_list --threads=1  --iterations=1000  --lists=8  --sync=m   >> lab2b_list.csv
	./lab2_list --threads=2  --iterations=1000  --lists=8  --sync=m   >> lab2b_list.csv
	./lab2_list --threads=4  --iterations=1000  --lists=8  --sync=m   >> lab2b_list.csv
	./lab2_list --threads=8  --iterations=1000  --lists=8  --sync=m   >> lab2b_list.csv
	./lab2_list --threads=12 --iterations=1000  --lists=8  --sync=m   >> lab2b_list.csv
	./lab2_list --threads=1  --iterations=1000  --lists=16 --sync=m   >> lab2b_list.csv
	./lab2_list --threads=2  --iterations=1000  --lists=16 --sync=m   >> lab2b_list.csv
	./lab2_list --threads=4  --iterations=1000  --lists=16 --sync=m   >> lab2b_list.csv
	./lab2_list --threads=8  --iterations=1000  --lists=16 --sync=m   >> lab2b_list.csv
	./lab2_list --threads=12 --iterations=1000  --lists=16 --sync=m   >> lab2b_list.csv
	#5
	echo "running tests for 5"
	./lab2_list --threads=1  --iterations=1000  --lists=4  --sync=s   >> lab2b_list.csv
	./lab2_list --threads=2  --iterations=1000  --lists=4  --sync=s   >> lab2b_list.csv
	./lab2_list --threads=4  --iterations=1000  --lists=4  --sync=s   >> lab2b_list.csv
	./lab2_list --threads=8  --iterations=1000  --lists=4  --sync=s   >> lab2b_list.csv
	./lab2_list --threads=12 --iterations=1000  --lists=4  --sync=s   >> lab2b_list.csv
	./lab2_list --threads=1  --iterations=1000  --lists=8  --sync=s   >> lab2b_list.csv
	./lab2_list --threads=2  --iterations=1000  --lists=8  --sync=s   >> lab2b_list.csv
	./lab2_list --threads=4  --iterations=1000  --lists=8  --sync=s   >> lab2b_list.csv
	./lab2_list --threads=8  --iterations=1000  --lists=8  --sync=s   >> lab2b_list.csv
	./lab2_list --threads=12 --iterations=1000  --lists=8  --sync=s   >> lab2b_list.csv
	./lab2_list --threads=1  --iterations=1000  --lists=16 --sync=s   >> lab2b_list.csv
	./lab2_list --threads=2  --iterations=1000  --lists=16 --sync=s   >> lab2b_list.csv
	./lab2_list --threads=4  --iterations=1000  --lists=16 --sync=s   >> lab2b_list.csv
	./lab2_list --threads=8  --iterations=1000  --lists=16 --sync=s   >> lab2b_list.csv
	./lab2_list --threads=12 --iterations=1000  --lists=16 --sync=s   >> lab2b_list.csv


lab2_list: lab2_list.o SortedList.o
	gcc -o $@ -pthread -g -L /u/eng/ugrad/weihong/lib/ -lprofiler SortedList.o lab2_list.o
	#gcc -o $@ -pthread -g lab2_list.o SortedList.o

#profile: lab2_list Makefile
#	-rm -f ./raw.gperf
#	LD_PRELOAD=/u/eng/ugrad/weihong/lib/libprofiler.so.0 CPUPROFILE=./raw.gperf ./lab2_list --threads=12 --iterations=1000 --sync=s
#	pprof --text ./lab2_list ./raw.gperf > profile.out
#	pprof --list=listWork ./lab2_list ./raw.gperf >> profile.out
#	rm -f ./raw.gperf

profile:
	-rm -f profile.gperf 
	LD_PRELOAD=/usr/lib64/libprofiler.so CPUPROFILE=profile.gperf ./lab2_list --threads=12 --iterations=1000 --sync=s
	pprof --text lab2_list profile.gperf > profile.out
	pprof --list=listWork lab2_list profile.gperf >> profile.out
	-rm -f profile.gperf

graphs: tests
	./lab2_list.gp

clean:
	rm -f *.o *.tar.gz lab2_list

dist: tests
	tar -czvf lab2b-004832220.tar.gz profile.out lab2_list.c SortedList.h SortedList.c lab2b_list.csv lab2_list.gp lab2b_1.png lab2b_2.png lab2b_3.png lab2b_4.png lab2b_5.png Makefile README

