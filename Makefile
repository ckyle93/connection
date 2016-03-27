

client: client.c
	gcc -g -O0 -Wall -Wextra -std=gnu11 client.c -o client

read_file: read_file.c
	gcc -g -O0 -Wall -Wextra -std=gnu11 read_file.c -o read_file
