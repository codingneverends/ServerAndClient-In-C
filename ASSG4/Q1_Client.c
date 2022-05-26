#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#define PORT 8080
#define SA struct sockaddr
struct query{
    char content[100];
    int id;
    int val;
};
void func(int sockfd)
{
    struct query _query;
    int n;
    char exp[100];
    read(sockfd, &_query, sizeof(struct query));
    printf("Client id %d\n\n",_query.id);
    while(1) {
        bzero(exp, 100);
        printf("Enter the query : ");
        int i=0;
        while ((exp[i++]=getchar())!='\n');
        if (strncmp("exit", exp, 4) == 0) {
            exit(0);
        }
        //scanf("%\n[^\n]s",exp);
        strcpy(_query.content,exp);
        write(sockfd, &_query, sizeof(_query));
        read(sockfd, &_query, sizeof(struct query));
        printf("\n\n[Server] : %d\n\n",_query.val);
    }
}
   
int main()
{
    int sockfd, connfd;
    struct sockaddr_in servaddr, cli;
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd == -1) {
        printf("socket creation failed...\n");
        exit(0);
    }
    else
        printf("Socket successfully created..\n");
    bzero(&servaddr, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = inet_addr("127.0.0.1");
    servaddr.sin_port = htons(PORT);
    if (connect(sockfd, (SA*)&servaddr, sizeof(servaddr)) != 0) {
        printf("connection with the server failed...\n");
        exit(0);
    }
    else
        printf("connected to the server..\n");
    func(sockfd);
    close(sockfd);
}