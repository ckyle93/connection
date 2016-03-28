/* By: Chris Kyle
 * This program is based off the programming challenge
 * Challenge #258 [Easy] IRC: Making a Connection
 * www.reddit.com/r/dailyprogrammer/comments/4ad23z/20160314_challenge_258_easy_irc_making_a/
*/

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
        char *buffstr;
        strcpy(buffstr, buffer);
        assert(buffstr != NULL);
        if (strcmp(strsep(&buffstr, " "), "PING") == 0){
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

int main(int argc, char *argv[]) {
    int sockfd, portno;
    struct sockaddr_in serv_addr;
    struct hostent *server;

    if (argc != 2) {
        printf("Usage: client <filename>");
        exit(0);
    }
    FILE *address_file = fopen(argv[1], "r");
    char buff[255];

    // Get address of the form www.example.com:1234
    fgets(buff, 255, address_file);
    char *str;
    strcpy(str, buff);
    char *web_address = strsep(&str, ":");
    size_t port_number = atoi(strsep(&str, ":"));

    // Get nickname and strip newlines.
    fgets(buff, 255, address_file);
    char nickname[64];
    strtok(buff, "\n");
    strcpy(nickname, buff);

    // Get username and strip newlines.
    fgets(buff, 255, address_file);
    char username[64];
    strtok(buff, "\n");
    strcpy(username, buff);

    // Get realname and strip newlines
    fgets(buff, 255, address_file);
    char realname[64];
    strtok(buff, "\n");
    strcpy(realname, buff);
    fclose(address_file);

    // Connect to server
    // This is based off of the C socket tutorial at
    // http://www.linuxhowtos.org/C_C++/socket.htm
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
    send_username(username, realname, sockfd);
    listen_to_server(sockfd);
    close(sockfd);
    return 0;
}
