#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <pthread.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>

int verified=0;

struct message{
    char msg[1024];
    char sender_name[1024];
    int toall;
    int fromserver;
};

void * doRecieving(void * sockID){

	int clientSocket = *((int *) sockID);
    struct message _message;
	while(1){
		int read = recv(clientSocket,&_message,sizeof(struct message),0);
        if(_message.fromserver==1){
            printf("%s",_message.msg);
			if(strcmp(_message.msg,"Verified")==0){
				verified=1;
				printf("\n\nWelcome to project COBRA\n\n");
			}
			printf("\n");
        }
        else{
			if(strcmp(_message.msg,"")==0)
				continue;
            if(_message.toall==1)
		        printf(" \n[To All:%s] : %s\n\n",_message.sender_name,_message.msg);
            else 
		        printf(" \n[%s] : %s\n\n",_message.sender_name,_message.msg);
        }

	}

}

int main(){

    char space;
	char input[1024];

	int clientSocket = socket(PF_INET, SOCK_STREAM, 0);

	struct sockaddr_in serverAddr;

	serverAddr.sin_family = AF_INET;
	serverAddr.sin_port = htons(8080);
	serverAddr.sin_addr.s_addr = htonl(INADDR_ANY);

	if(connect(clientSocket, (struct sockaddr*) &serverAddr, sizeof(serverAddr)) == -1) return 0;

	printf("Connection established ............\n");
    pthread_t thread;
	pthread_create(&thread, NULL, doRecieving, (void *) &clientSocket );
	strcpy(input,"Hi!");
	send(clientSocket,input,1024,0);
	while(1){

		scanf("%s",input);

		if(strcmp(input,"LIST") == 0){
			if(verified==0){
				printf("Please verify first.\n");
				continue;
			}
			send(clientSocket,input,1024,0);

		}
		if(strcmp(input,"SEND") == 0){
			if(verified==0){
				printf("You are not allowed to send msg. Not verified.\n");
				continue;
			}
			send(clientSocket,input,1024,0);
			
			scanf("%s",input);
			send(clientSocket,input,1024,0);
			//%c reading space line 
			scanf("%c%[^\n]s",&space,input);
			send(clientSocket,input,1024,0);

		}
        if(strcmp(input,"NAME")==0){
            send(clientSocket,input,1024,0);
			//%c reading space line
			scanf("%c%[^\n]s",&space,input);
			send(clientSocket,input,1024,0);
        }
		if(strcmp(input,"PASS")==0){
			if(verified==1){
				printf("Already Verified.\n");
			}
            send(clientSocket,input,1024,0);
			scanf("%c%[^\n]s",&space,input);
			send(clientSocket,input,1024,0);
		}
	}


}