#NAME: Jonathan Chu
#EMAIL: jonathanchu78@gmail.com
#ID: 004832220
.SILENT:

default:
	gcc -lmraa -lm -Wall -Wextra -g -o lab4c_tcp lab4c_tcp.c
	gcc -lmraa -lm -lssl -lcrypto -Wall -Wextra -g -o lab4c_tls lab4c_tls.c

clean:
	rm -f lab4c_tcp lab4c_tls *.o *.tar.gz

dist:
	tar -czvf lab4c-004832220.tar.gz Makefile README lab4c_tcp.c lab4c_tls.c
