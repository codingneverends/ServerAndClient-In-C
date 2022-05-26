#include <stdio.h>
#include <netdb.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <pthread.h>

struct fruit{
    char name[20];
    int count;
};

#define MAX_CLIENTS 100
#define PORT 8080
#define SA struct sockaddr

struct fruit fruits[5];

int getindex(char *str){
    if(strncmp(str,"apple",5)==0)
        return 0;
    if(strncmp(str,"mango",5)==0)
        return 1;
    if(strncmp(str,"banana",6)==0)
        return 2;
    if(strncmp(str,"chikoo",6)==0)
        return 3;
    if(strncmp(str,"papaya",6)==0)
        return 4;
    return -1;
}

void* func(void *_connfd)
{
    int connfd=*((int*)_connfd);
    int size;
    char buff[1000];
    int n,fruitcount,index;
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
        else if(strncmp("Fruits", buff, 6) == 0){
            printf("Client requested for fruits.\n");
            strcpy(buff,"Enter the name of fruit : ");
            write(connfd, buff, sizeof(buff));
            if(read(connfd, buff, sizeof(struct fruit))==0){
                printf("Client connection closed\n");
                pthread_exit(NULL);
            }
            struct fruit _fruit=*((struct fruit*)(buff));
            strcpy(buff,_fruit.name);
            fruitcount=_fruit.count;
            printf("client request for %d of %s \n",fruitcount,buff);
            index=getindex(_fruit.name);
            if(fruits[index].count>fruitcount && index!=-1){
                fruits[index].count=fruits[index].count-fruitcount;
                strcpy(buff,"Granted..\n");
                size=sizeof(buff);
                printf("Granting fruits\n");
            }
            else{
                strcpy(buff,"Not Available\n");
                size=sizeof(buff);
                printf("Not Available..\n");
            }
            write(connfd, buff, sizeof(buff));
        }
        else if(strncmp("SendInventory", buff, 13) == 0){
            write(connfd, fruits, 5*sizeof(struct fruit));
            printf("Sending Inventory ..\n");
        }
        else{
            printf("unexpected request from client");
            strcpy(buff,"404 - Bad Request \n");
            write(connfd, buff, sizeof(buff));
        }
    }
}


pthread_t manage_client[MAX_CLIENTS];

int main()
{
    strcpy(fruits[0].name,"apple");
    strcpy(fruits[1].name,"mango");
    strcpy(fruits[2].name,"banana");
    strcpy(fruits[3].name,"chikoo");
    strcpy(fruits[4].name,"papaya");
    for(int f=0;f<5;f++)
        fruits[f].count=100;
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