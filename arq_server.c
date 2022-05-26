#include <stdio.h>
#include <netdb.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <pthread.h>

struct response{
    char data[20];
    int ack;
};

struct packet{
    char data[20];
    int ack;
};

#define MAX_CLIENTS 100
#define PORT 8080
#define SA struct sockaddr


void* func(void *_connfd)
{
    int connfd=*((int*)_connfd);
    int size;
    char buff[1000];
    struct response _response;
    struct packet _packet;
    while (1){
        bzero(buff, 1000);
        if(read(connfd, buff, sizeof(buff))==0){
            printf("Client connection closed\n");
            pthread_exit(NULL);
        }
        if (strncmp("exit", buff, 4) == 0) {
            printf("Server Exit...\n");
            break;
        }
        else if(strncmp("givedata", buff, 6) == 0){
            printf("Client requested for data.\n");
            write(connfd, buff, sizeof(buff));
            if(read(connfd, buff, sizeof(struct fruit))==0){
                printf("Client connection closed\n");
                pthread_exit(NULL);
            }
            write(connfd, buff, sizeof(buff));
        }
    }
}


pthread_t manage_client[MAX_CLIENTS];

int main()
{
    int serverSocket,newSocket;
    struct sockaddr_in serverAddr;
    struct sockaddr_storage serverStorage;
   
    socklen_t addr_size;

    serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (serverSocket == -1) {
        printf("socket creation failed...\n");
        exit(0);
    }
    else
        printf("Socket successfully created..\n");
   
    serverAddr.sin_addr.s_addr = INADDR_ANY;
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(PORT);

    if ((bind(serverSocket,(struct sockaddr*)&serverAddr,sizeof(serverAddr))) != 0) {
        printf("socket bind failed...\n");
        exit(0);
    }
    else
        printf("Socket successfully binded..\n");
   
    if ((listen(serverSocket, 5)) != 0) {
        printf("Listen failed...\n");
        exit(0);
    }
    else
        printf("Server listening..\n");

        
    pthread_t tid[MAX_CLIENTS];
    int i = 0;
 
    while (1) {
        addr_size = sizeof(serverStorage);
        newSocket = accept(serverSocket,(struct sockaddr*)&serverStorage,&addr_size);
        if(newSocket<0){
            printf("Server accept failed..\n");
        }
        else{
            printf("server accepted the client ...\n");
        }
        if (pthread_create(&manage_client[i++], NULL,func, &newSocket)!= 0)
            printf("Failed to create thread\n");
        if (i >= 50) {
            i = 0;
            while (i < 50) {
                pthread_join(manage_client[i++],NULL);
            }
            i = 0;
        }
    }
    close(serverSocket);
}