#include <stdio.h>
#include <netdb.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>

struct query{
    char content[100];
    int id;
    int val;
};

#define MAX_CLIENTS 100
#define PORT 8080
#define SA struct sockaddr
int process(char *str){
    int state=0;
    int x=0;
    int y=0;
    char operator='+';
    for(int i=0;str[i]!='\n';i++){
        if(str[i]==' ')
            if(state==0 && i==0)
                continue;
            else{
                if(str[i-1]!=' ')
                    state++;
                continue;
            }
        if(state==0){
            x=x*10+(str[i]-'0');
            if(str[i+1]=='+'||str[i+1]=='-'|str[i+1]=='*'||str[i+1]=='/')
                state++;
        }
        else if(state==1){
            operator=str[i];
            if(str[i+1]!=' ')
                state++;
        }
        else if(state==2){
            y=y*10+(str[i]-'0');
        }
    }
    if(operator == '+')
        return x+y;
    if(operator == '-')
        return x-y;
    if(operator == '*')
        return x*y;
    if(operator == '/')
        return x/y;
    return -1;
}
void func(int connfd,int id)
{
    int size;
    struct query _query;
    int n,querycount,index;
    strcpy(_query.content,"You are client");
    _query.id=id;
    int val;
    write(connfd, &_query, sizeof(_query));
    printf("client pid initiated\n");
    while (1){
        if(read(connfd, &_query, sizeof(struct query))==0){
            printf("Client connection closed\n");
        }
        if (strncmp("exit", _query.content, 4) == 0) {
            printf("Server Exit...\n");
            break;
        }
        else{
            val = process(_query.content);
            printf("\n[Client%d] : %s\n",id,_query.content);
            printf("[Server]: %d\n\n\n",val);
            strcpy(_query.content,"You are client");
            _query.id=id;
            _query.val=val;
            write(connfd, &_query, sizeof(_query));
        }
    }
}

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

    int i = 0;
 
    while (1) {
        addr_size = sizeof(serverStorage);
        newSocket = accept(serverSocket,(struct sockaddr*)&serverStorage,&addr_size);
        if(newSocket<0){
            printf("Server accept failed..\n");
        }
        else{
            printf("server accepted the client ...\n");
            i++;
        }
        int childpid = fork();
        if(childpid == 0){
            close(serverSocket);
            func(newSocket,i);
        }
    }
    close(serverSocket);
}