client: client.c
	gcc -g -O0 -Wall -Wextra -std=gnu11 client.c -o client

clean: client
	rm client
