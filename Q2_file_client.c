#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <libgen.h>

#define MAX_LINE 512
#define PORT 8080
#define SA struct sockaddr
#define BUFFSIZE 512
ssize_t writefile(int sockfd, FILE *fp)
{
    ssize_t total=0;
    ssize_t n;
    char buff[MAX_LINE] = {0};
    while ((n = recv(sockfd, buff, MAX_LINE, 0)) > 0 && strncmp(buff,"dataover",8)!=0) 
    {
	    total+=n;
        if (n == -1)
        {
            perror("Receive File Error");
            exit(1);
        }
        
        if (fwrite(buff, sizeof(char), n, fp) != n)
        {
            perror("Write File Error");
            exit(1);
        }
        memset(buff, 0, MAX_LINE);
    }
    return total;
}

void ReciveVedio(int connfd){ 
    char filename[BUFFSIZE] = {0}; 
    if (recv(connfd, filename, BUFFSIZE, 0) == -1) 
    {
        perror("Can't receive filename");
        exit(1);
    }
    printf("%s\n",filename);
    FILE *fp = fopen(filename, "wb");
    if (fp == NULL) 
    {
        perror("Can't open file");
        exit(1);
    }
    char addr[INET_ADDRSTRLEN];
    printf("Start receive file: %s\n", filename);
    ssize_t total = writefile(connfd, fp);
    printf("Receive Success, NumBytes = %ld\n", total);
    fclose(fp);
}

void func(int sockfd)
{
    int n;
    char buff[1000];
    char name[20],fruitname[20];
    int fruitcount;
    while(1) {
        fruitcount=-1;
        bzero(buff, sizeof(buff));
        printf("Enter Request \n\n");
        scanf("%s",name);
        if(strncmp(name,"bye",3)==0 || strncmp(name,"GivemeyourVedio",15)==0){
            strcpy(buff,name);
            write(sockfd, buff, sizeof(buff));
            if ((strncmp(buff, "bye", 3)) == 0) {
                printf("Client Exit...\n");
                break;
            }
            else{
                ReciveVedio(sockfd);
            }
        }
        else{
            write(sockfd, buff, sizeof(buff));
            printf("hm\n");
        }
    }
}
   
int main(int argc, char *argv[])
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