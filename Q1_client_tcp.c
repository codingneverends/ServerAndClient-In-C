#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#define PORT 8080
#define SA struct sockaddr
struct fruit{
    char name[20];
    int count;
};
void func(int sockfd)
{
    struct fruit _fruit;
    int n;
    char buff[1000];
    char name[20],fruitname[20];
    int fruitcount;
    struct fruit fruits[5];
    while(1) {
        fruitcount=-1;
        bzero(buff, sizeof(buff));
        printf("Enter Request \n\t1.Fruits\n\t2.SendInventory\n\n");
        scanf("%s",name);
        if(strncmp(name,"exit",4)==0 || strncmp(name,"Fruits",6)==0 || strncmp(name,"SendInventory",13)==0){
            strcpy(buff,name);
            write(sockfd, buff, sizeof(buff));
            if ((strncmp(buff, "exit", 4)) == 0) {
                printf("Client Exit...\n");
                break;
            }
            if(strncmp(name,"Fruits",6)==0){
                read(sockfd, buff, sizeof(buff));
                printf("%s",buff);
                scanf("%s",fruitname);
                struct fruit _fruit;
                strcpy(_fruit.name,fruitname);
                printf("Enter fruit count : ");
                scanf("%d",&fruitcount);
                _fruit.count=fruitcount;
                write(sockfd, (void*)&_fruit, sizeof(struct fruit));
                read(sockfd, buff, sizeof(buff));
                printf("%s",buff);
            }
            else if(strncmp(name,"SendInventory",13)==0){
                read(sockfd, fruits, 5*sizeof(struct fruit));
                for(int j=0;j<5;j++){
                    printf("%d %s\n",fruits[j].count,fruits[j].name);
                }
            }
            else{
                printf("\n\n...\n\n");
            }
        }
        else{
            printf("404 -- Bad Request");
        }
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