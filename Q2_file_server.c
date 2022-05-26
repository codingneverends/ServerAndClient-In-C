#include <stdio.h>
#include <netdb.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <pthread.h>
#include<time.h>
#include <libgen.h>

enum { NS_PER_SECOND = 1000000000 };

#define MAX_LINE 512
#define MAX_CLIENTS 100
#define PORT 8080
#define SA struct sockaddr
#define BUFFSIZE 512

struct funcdata{
    int connfd;
    char *filename;
    char *filelocation;
};


void sub_timespec(struct timespec t1, struct timespec t2, struct timespec *td)
{
    td->tv_nsec = t2.tv_nsec - t1.tv_nsec;
    td->tv_sec  = t2.tv_sec - t1.tv_sec;
    if (td->tv_sec > 0 && td->tv_nsec < 0)
    {
        td->tv_nsec += NS_PER_SECOND;
        td->tv_sec--;
    }
    else if (td->tv_sec < 0 && td->tv_nsec > 0)
    {
        td->tv_nsec -= NS_PER_SECOND;
        td->tv_sec++;
    }
}

void sendfile(FILE *fp, int sockfd) 
{
    int total=0;
    int n; 
    char sendline[MAX_LINE] = {0}; 
    struct timespec start, finish, delta;
    File *tempfile;
    tempfile=fopen("temprec.txt","w");
    while ((n = fread(sendline, sizeof(char), MAX_LINE, fp)) > 0) 
    {
	    total+=n;
        if (n != MAX_LINE && ferror(fp))
        {
            perror("Read File Error");
            exit(1);
        }
        clock_gettime(CLOCK_REALTIME, &start);
        if (send(sockfd, sendline, n, 0) == -1)
        {
            perror("Can't send file");
            exit(1);
        }
        clock_gettime(CLOCK_REALTIME, &finish);
        sub_timespec(start, finish, &delta);
        printf("%d.%.9ld\n", (int)delta.tv_sec, delta.tv_nsec);
        if(tempfile==NULL)
            printf("ahh\n");
        fprintf(tempfile,"%d.%.9ld,", (int)delta.tv_sec, delta.tv_nsec);
        memset(sendline, 0, MAX_LINE);
    }
    fclose(tempfile);
}

void OpenVedio(int sockfd,char *filename,char *filelocation){ 
    printf("%s %s" ,filelocation,filename);
    if (filename == NULL)
    {
        perror("Can't get filename");
        exit(1);
    }
    
    char buff[BUFFSIZE] = {0};
    strncpy(buff, filename, strlen(filename));
    if (send(sockfd, buff, BUFFSIZE, 0) == -1)
    {
        perror("Can't send filename");
        exit(1);
    }
    
    FILE *fp = fopen(filelocation, "rb");
    if (fp == NULL) 
    {
        perror("Can't open file");
        exit(1);
    }
    sendfile(fp,sockfd);
    fclose(fp);
}

void* func(void *_func_data)
{
    struct funcdata _data=*((struct funcdata*)_func_data);
    int connfd=_data.connfd;
    int size;
    char buff[1000];
    while (1){
        bzero(buff, 1000);
        if(read(connfd, buff, sizeof(buff))==0){
            printf("Client connection closed\n");
            pthread_exit(NULL);
        }
        if (strncmp("bye", buff, 3) == 0) {
            printf("Server Exit...\n");
            pthread_exit(NULL);
            break;
        }
        else if(strncmp("GivemeyourVedio", buff, 15) == 0){
            printf("Sending Vedio..\n");
            OpenVedio(connfd,_data.filename,_data.filelocation);
            strcpy(buff,"dataover");
            printf("\nVedio Sent..\n\n");
            send(connfd, buff, sizeof(buff), 0);
        }
    }
}


pthread_t manage_client[MAX_CLIENTS];

int main(int argc, char *argv[])
{
    int serverSocket,newSocket;
    struct sockaddr_in serverAddr;
    struct sockaddr_storage serverStorage;
    struct funcdata func_data;
   
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
        func_data.connfd=newSocket;
        func_data.filelocation = argv[1];
        func_data.filename=basename(argv[1]);
        if (pthread_create(&manage_client[i++], NULL,func, &func_data)!= 0)
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