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

int clientCount = 0;

char password[10];

static pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
static pthread_cond_t cond = PTHREAD_COND_INITIALIZER;

struct client{
	int index;
	int sockID;
	struct sockaddr_in clientAddr;
	int len;
    char name[1024];
	int disconnected;
	int verfied;
};

struct message{
    char msg[1024];
    char sender_name[1024];
    int toall;
    int fromserver;
	int verified;
};

struct client Client[1024];
pthread_t thread[1024];

void * doNetworking(void * ClientDetail){

    struct message _message;
	struct message commom_msg; 
	struct client* clientDetail = (struct client*) ClientDetail;
	int index = clientDetail -> index;
	clientDetail->verfied=0;
	int clientSocket = clientDetail -> sockID;
    strcpy(clientDetail->name,"Client");
    clientDetail->disconnected=0;
    printf("Client %d connected.\n",index + 1);

	while(clientDetail->disconnected==0){
		_message.verified=0;
        _message.fromserver=0;

		char data[1024];
		int read = recv(clientSocket,data,1024,0);

		if(read==0){
			printf("Client Disconnected");
			clientDetail->disconnected=1;
			clientDetail->verfied=0;
            _message.toall=1;
			strcpy(_message.sender_name,clientDetail->name);
			strcpy(_message.msg,"Bye!\n");
            for(int _id=0;_id<clientCount;_id++)
                if(_id!=index && Client[_id].verfied==1)
			        send(Client[_id].sockID,&_message,sizeof(_message),0);	
		}

		data[read] = '\0';

		char output[1024];

		if(strcmp(data,"LIST") == 0){

			int l = 0;

			for(int i = 0 ; i < clientCount ; i ++){

				if(i != index && Client[i].verfied==1)
					l += snprintf(output + l,1024,"PI %d. %s is at socket %d.\n",i + 1,Client[i].name,Client[i].sockID);

			}
            _message.fromserver=1;
            strcpy(_message.msg,output);
			send(clientSocket,&_message,sizeof(_message),0);
			continue;

		}
		if(strcmp(data,"SEND") == 0){
			read = recv(clientSocket,data,1024,0);
			data[read] = '\0';
			int id = atoi(data) - 1;
			read = recv(clientSocket,data,1024,0);
			data[read] = '\0';
            strcpy(_message.msg,data);
            strcpy(_message.sender_name,clientDetail->name);
            _message.toall=0;
            if(id==-1){
                _message.toall=1;
                for(int _id=0;_id<clientCount;_id++)
                    if(_id!=index  && Client[_id].verfied==1)
			            send(Client[_id].sockID,&_message,sizeof(_message),0);	
            }
            else
			    send(Client[id].sockID,&_message,sizeof(_message),0);			

		}
        if(strcmp(data,"NAME")==0){

			read = recv(clientSocket,data,1024,0);
			data[read] = '\0';

            strcpy(clientDetail->name,data);
            _message.fromserver=1;
            strcpy(_message.msg,"Your name changed.\n");
            printf("Client %d changed name to %s\n",index,clientDetail->name);
			send(clientSocket,&_message,sizeof(_message),0);
        }
		
        if(strcmp(data,"PASS")==0){
			read = recv(clientSocket,data,1024,0);
			if(strncmp(data,password,3)==0){
				clientDetail->verfied=1;
            	strcpy(_message.msg,"Verified");
				_message.verified=1;
	            printf("Co PI %s is Verified\n",clientDetail->name);
				commom_msg.fromserver=0;
				commom_msg.toall=1;
				strcpy(commom_msg.sender_name,clientDetail->name);
				strcpy(commom_msg.msg,"Hi!");
                for(int _id=0;_id<clientCount;_id++)
                    if(_id!=index && Client[_id].verfied==1)
			            send(Client[_id].sockID,&commom_msg,sizeof(commom_msg),0);	
			}
			else
            	strcpy(_message.msg,"Wrong Password");
            _message.fromserver=1;
			send(clientSocket,&_message,sizeof(_message),0);
        }
	}

	return NULL;

}

int main(){
	strcpy(password,"123");
	int serverSocket = socket(PF_INET, SOCK_STREAM, 0);

	struct sockaddr_in serverAddr;

	serverAddr.sin_family = AF_INET;
	serverAddr.sin_port = htons(8080);
	serverAddr.sin_addr.s_addr = htons(INADDR_ANY);


	if(bind(serverSocket,(struct sockaddr *) &serverAddr , sizeof(serverAddr)) == -1) return 0;

	if(listen(serverSocket,1024) == -1) return 0;

	printf("Server started listenting on port 8080 ...........\n");

	while(1){

		Client[clientCount].sockID = accept(serverSocket, (struct sockaddr*) &Client[clientCount].clientAddr, &Client[clientCount].len);
		Client[clientCount].index = clientCount;

		pthread_create(&thread[clientCount], NULL, doNetworking, (void *) &Client[clientCount]);

		clientCount ++;
 
	}

	for(int i = 0 ; i < clientCount ; i ++)
		pthread_join(thread[i],NULL);

}