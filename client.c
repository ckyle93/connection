#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <assert.h>

void error(const char *msg)
{
    perror(msg);
    exit(0);
}

void send_nickname(const char *nick, int sock) {
    char buffer[512];
    bzero(buffer,512);
    strcpy(buffer, "NICK ");
    strcat(buffer, nick);
    strcat(buffer, "\r\n");
    printf("%s", buffer);
    int n = write(sock,buffer,strlen(buffer));
    if (n < 0) 
         error("ERROR writing to socket");
}

void send_username(const char *user_name, const char *real_name, int sock) {
    char buffer[512];
    bzero(buffer,512);
    strcpy(buffer, "USER ");
    strcat(buffer, user_name);
    strcat(buffer, " 0 * :");
    strcat(buffer, real_name);
    strcat(buffer, "\r\n");
    printf("%s", buffer);
    int n = write(sock,buffer,strlen(buffer));
    if (n < 0) 
         error("ERROR writing to socket");
}

void listen_to_server(int sock) {
    char buffer[512];
    while(1) {
        bzero(buffer,512);
        int n = read(sock,buffer,511);
        if (n < 0) 
             error("ERROR reading from socket");
        char *buffstr = strdup(buffer);
        assert(buffstr != NULL);
        if (strcmp(strsep(&buffstr, " "), "PING") == 0){
            return;
            char response[128];
            strcpy(response, "PONG ");
            strcat(response, strsep(&buffstr, " "));
            strcat(response, "\r\n");
            n = write(sock, response, strlen(response)); 
            printf("%s", response);
        }
        printf("%s",buffer);
    }
}

int main(int argc, char *argv[])
{
    int sockfd, portno, n;
    struct sockaddr_in serv_addr;
    struct hostent *server;

    char buffer[512];

    if (argc != 2) {
        printf("Usage: read_file <filename>");
        exit(0);
    }
    FILE *address_file = fopen(argv[1], "r");
    char buff[255];
    fgets(buff, 255, address_file);
    char *str = strdup(buff);
    assert(str != NULL);
    char *web_address = strsep(&str, ":");
    size_t port_number = atoi(strsep(&str, ":"));
    fgets(buff, 255, address_file);
    char *nickname = strtok(strdup(buff), "\n");
    assert(nickname != NULL);
    fgets(buff, 255, address_file);
    char *username = strtok(strdup(buff), "\n");
    assert(username != NULL);
    fgets(buff, 255, address_file);
    char *realname = strtok(strdup(buff), "\n");
    assert(realname != NULL);
    fclose(address_file);
    free(str);

    portno = port_number;
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) 
        error("ERROR opening socket");
    server = gethostbyname(web_address);
    if (server == NULL) {
        fprintf(stderr,"ERROR, no such host\n");
        exit(0);
    }
    bzero((char *) &serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    bcopy((char *)server->h_addr, 
         (char *)&serv_addr.sin_addr.s_addr,
         server->h_length);
    serv_addr.sin_port = htons(portno);
    if (connect(sockfd,(struct sockaddr *) &serv_addr,sizeof(serv_addr)) < 0) 
        error("ERROR connecting");

    send_nickname(nickname, sockfd);
    free(nickname);
    send_username(username, realname, sockfd);
    free(username);
    free(realname);
    listen_to_server(sockfd);
    close(sockfd);
    return 0;
}
