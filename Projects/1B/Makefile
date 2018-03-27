#NAME: Jonathan Chu
#EMAIL: jonathanchu78@gmail.com
#ID: 004832220

default:
	gcc -Wall -Wextra -lz -g -o lab1b-client lab1b-client.c
	gcc -Wall -Wextra -lz -g -o lab1b-server lab1b-server.c

clean:
	rm -f lab1b-client lab1b-server *.o *.txt *.tar.gz

dist:
	tar -czvf lab1b-004832220.tar.gz lab1b-client.c lab1b-server.c Makefile README

