#NAME: Jonathan Chu
#EMAIL: jonathanchu78@gmail.com
#ID: 004832220
.SILENT:

default:
	gcc -lmraa -lm  -Wall -Wextra -g -o lab4b lab4b.c

check: default
	./check.sh
	#if [ -s error_output.txt ]; \
	then \
		cat error_output.txt; rm -f error_output.txt;\
	else \
	   echo "Passed Smoke Check"; \
	fi 

clean:
	rm -f lab4b *.txt *.o *.tar.gz

dist:
	tar -czvf lab4b-004832220.tar.gz Makefile README lab4b.c

argTest:
	./lab4b --random \
	if [[ $$? -ne 1 ]]; \
	then \
		echo "failure to recognize bogus argument: exit code was not 1\n" > error_output.txt \
	fi;
