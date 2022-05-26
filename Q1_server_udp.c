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

   
int main() {
    strcpy(fruits[0].name,"apple");
    strcpy(fruits[1].name,"mango");
    strcpy(fruits[2].name,"banana");
    strcpy(fruits[3].name,"chikoo");
    strcpy(fruits[4].name,"papaya");
    for(int f=0;f<5;f++)
        fruits[f].count=100;
    
    int sockfd;
    char buffer[MAXLINE];
    struct sockaddr_in servaddr, cliaddr;
    if ( (sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0 ) {
        perror("socket creation failed");
        exit(EXIT_FAILURE);
    }  
    memset(&servaddr, 0, sizeof(servaddr));
    memset(&cliaddr, 0, sizeof(cliaddr));
    servaddr.sin_family    = AF_INET; 
    servaddr.sin_addr.s_addr = INADDR_ANY;
    servaddr.sin_port = htons(PORT);
    if ( bind(sockfd, (const struct sockaddr *)&servaddr,sizeof(servaddr)) < 0 )
    {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }
    
    struct data _data,data_;
    strcpy(_data.command,"test");
    _data.status=-1;
    strcpy(_data.fruit_name,"banana");
    _data.fruit_count=10;
    
    int len, n;
    int index=-1;
    len = sizeof(cliaddr);
    int status=-1;
    while (1)
    {
        n = recvfrom(sockfd, (struct data*)&data_, MAXLINE,  MSG_WAITALL, ( struct sockaddr *) &cliaddr, &len);
        status=data_.status;
        if(status==0){
            _data.status=22;
            strcpy(_data.command,"reply");
            _data.fruit_count=0;
            strcpy(_data.fruit_name,"");
            strcpy(_data.text,"Enter name of fruit : ");
            sendto(sockfd, (struct data*)&_data,sizeof(_data),MSG_CONFIRM, (const struct sockaddr *) &cliaddr,len);
            printf("Client requesting for fruits\n");
        }
        else if(status==1){
            _data.status=23;
            strcpy(_data.command,"reply");
            printf("client asking for %d of %s\n",data_.fruit_count,data_.fruit_name);
            index=getindex(data_.fruit_name);
            if(fruits[index].count > data_.fruit_count){
                fruits[index].count=fruits[index].count-data_.fruit_count;
                strcpy(_data.text,"Granted..\n");
                printf("Granted..\n");
            }else{
                strcpy(_data.text,"Not Available..\n");
                printf("Not Available...\n");
            }
            _data.fruit_count=fruits[index].count;
            strcpy(_data.fruit_name,fruits[index].name);
            sendto(sockfd, (struct data*)&_data,sizeof(_data),MSG_CONFIRM, (const struct sockaddr *) &cliaddr,len);
        }
        else if(status==2){
            //special call
            sendto(sockfd, fruits,sizeof(fruits),MSG_CONFIRM, (const struct sockaddr *) &cliaddr,len);
            printf("Sending Inventory..\n");
        }    
    }   
    return 0;
}