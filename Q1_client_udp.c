#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
   
#define PORT     8080
#define MAXLINE 1024

struct fruit{
    char name[20];
    int count;
};

struct data
{
    int status;
    char command[20];
    char text[100];
    char fruit_name[20];
    int fruit_count;
};

int main() {
    int sockfd;
    char buffer[MAXLINE];
    char *hello = "Hello from client";
    struct sockaddr_in     servaddr;
    if ( (sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0 ) {
        perror("socket creation failed");
        exit(EXIT_FAILURE);
    }
    memset(&servaddr, 0, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(PORT);
    servaddr.sin_addr.s_addr = INADDR_ANY;
       
    struct data _data,data_;
    struct data *__data=malloc(sizeof(struct data));
    strcpy(_data.command,"heeya");
    _data.status=-1;
    strcpy(_data.fruit_name,"apple");
    _data.fruit_count=10;

    char fruitname[20];
    int fruitcount;
    char buff[20];
    struct fruit fruits[5];

    int n, len;
    while(1){
        printf("Enter option \n\t1.Fruits\n\t2.SendInventory\n\n");
        scanf("%s",buff);
        strcpy(_data.command,buff);
        if(strncmp("Fruits", buff, 6) == 0){
            _data.status=0;
            _data.fruit_count=0;
            strcpy(_data.fruit_name,"");
            sendto(sockfd, (struct data*)&_data, sizeof(_data),MSG_CONFIRM, (const struct sockaddr *) &servaddr,sizeof(servaddr));
            n=recvfrom(sockfd, (struct data*)&data_ , MAXLINE, MSG_WAITALL, (struct sockaddr *) &servaddr,&len);
            _data.status=1;
            printf("%s",data_.text);
            scanf("%s",fruitname);
            printf("Enter fruit count : ");
            scanf("%d",&fruitcount);
            _data.fruit_count=fruitcount;
            strcpy(_data.fruit_name,fruitname);
            strcpy(_data.command,"Fruits");
            sendto(sockfd, (struct data*)&_data, sizeof(_data),MSG_CONFIRM, (const struct sockaddr *) &servaddr,sizeof(servaddr));
            n=recvfrom(sockfd, (struct data*)&data_ , MAXLINE, MSG_WAITALL, (struct sockaddr *) &servaddr,&len);
            printf("%s",data_.text);
        }
        if(strncmp("SendInventory", buff, 13) == 0){
            _data.status=2;
            _data.fruit_count=0;
            strcpy(_data.fruit_name,"");
            sendto(sockfd, (struct data*)&_data, sizeof(_data),MSG_CONFIRM, (const struct sockaddr *) &servaddr,sizeof(servaddr));
            n=recvfrom(sockfd, fruits , MAXLINE, MSG_WAITALL, (struct sockaddr *) &servaddr,&len);        
            for(int j=0;j<5;j++){
                printf("%d %s\n",fruits[j].count,fruits[j].name);
            }
        }
    }
    sendto(sockfd, (struct data*)&_data, sizeof(_data),MSG_CONFIRM, (const struct sockaddr *) &servaddr,sizeof(servaddr));
    printf("cmd send %s.\n",_data.command);
    n = recvfrom(sockfd, (struct data*)&data_ , MAXLINE, MSG_WAITALL, (struct sockaddr *) &servaddr,&len);
    //printf("Server Replied\n");
    printf("cmd recived %s.\n",data_.command);
   
    close(sockfd);
    return 0;
}