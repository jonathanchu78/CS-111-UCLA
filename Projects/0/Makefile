#NAME: Jonathan Chu
#EMAIL: jonathanchu78@gmail.com
#ID: 004832220

default:
	gcc -Wall -Wextra -g -o lab0 lab0.c

#checking if file is empty: https://stackoverflow.com/questions/9964823/how-to-check-if-a-file-is-empty-in-bash
check: default inOutTest baseTest stdTest argTest inFileTest outFileTest catchTest segFaultTest 	
	if [ -s log.txt ]; \
	then \
	        cat log.txt; rm -f log.txt;\
	else \
	        echo "Passed Smoke Check"; \
	fi 

clean:
	rm -f lab0 *.o *.txt *.tar.gz

dist:
	tar -czvf lab0-004832220.tar.gz lab0.c Makefile backtrace.png \
	breakpoint.png README

#now all the smoke check tests:
inOutTest:
	echo "here is some text" > file.txt; \
	./lab0 --input=file.txt --output=out.txt; \
	cmp file.txt out.txt; \
	if [[ $$? -ne 0 ]]; \
	then \
		echo "error copying from input file to output file: comparison failed" > log.txt; \
	fi; \
	rm -f file.txt out.txt

baseTest: #same as inOutTest but without comparison; checking exit code
	 echo "here is some text" > file.txt; \
        ./lab0 --input=file.txt --output=out.txt; \
        if [[ $$? -ne 0 ]]; \
        then \
                echo "error copying from input file to output file: exit code 0 not received" > log.txt; \
        fi; \
        rm -f file.txt out.txt


stdTest:
	echo "here is some input" | ./lab0; \
	if [[ $$? -ne 0 ]]; \
        then \
                echo "error copying from stdin to stdout: exit code 0 not received" > log.txt; \
        fi; 


argTest:
	echo "here is some input" | ./lab0 --invalid; \
        if [[ $$? -ne 1 ]]; \
        then \
                echo "error handling invalid parameter: exit code 1 not received" > log.txt; \
        fi;


inFileTest: #relies on that ridiculous file not existing in user's directory
	./lab0 --input=efssafw89gf87d77878f68sdf87sdf78we89sdff.txt; \
	if [[ $$? -ne 2 ]]; \
        then \
                echo "error handling invalid input file: exit code 2 not received" > log.txt; \
        fi;

outFileTest:
	touch out.txt; chmod -w out.txt; \
	./lab0 --output=out.txt; \
	if [[ $$? -ne 3 ]]; \
        then \
                echo "error handling unwritable output file: exit code 3 not received" > log.txt; \
        fi; \
	rm -f out.txt;

catchTest:
	./lab0 --segfault --catch; \
	if [[ $$? -ne 4 ]]; \
        then \
                echo "error handling catch: exit code 4 not received" > log.txt; \
	fi;

segFaultTest:
	./lab0 --segfault; \
        if [[ $$? -ne  139 ]]; \
        then \
                echo "error handling catch: error code 139 not received" > log.txt; \
        fi;

