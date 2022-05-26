#include <stdio.h>
  #include <stdlib.h>
  #include <unistd.h>
  #include <string.h>
  #include <strings.h>
  #include <arpa/inet.h>
  #include <netinet/in.h>
  #include <sys/types.h>
  #include <sys/socket.h>

  void DieWithError(char *err) {
       perror(err);
       exit(1);
  }

  struct TextMessage
  {
       char request_Type[100];
       unsigned int SenderId;                       /* unique client identifier */
       unsigned int RecipientId;                       /* unique client identifier */
       char message[100];                           /* text message */
  };

  int main(int argc, char *argv[]) 
  { 
       int sock;                        /* Socket descriptor */
       struct sockaddr_in echoServAddr; /* Echo server address */
       struct sockaddr_in fromAddr;     /* Source address of echo */
       unsigned short echoServPort;     /* Echo server port */
       unsigned int fromSize;           /* In-out of address size for recvfrom() */
       char *servIP;                    /* IP address of server */
       int structLen;                   /* Length of string to echo */
       int respStringLen;               /* Length of received response */


       if (!argv[1]) {
              fprintf(stderr,"No server IP sepcified at arg 1\n");
              exit(1);
       }

       else if (!argv[2]) {
              fprintf(stderr,"No port Number Sepcified at arg 2\n");
              exit(2);
       }

       else if (!argv[3]) {
              fprintf(stderr,"no message string specified at arg 3\n");
              exit(3);
       }



        struct TextMessage newMess = { "Send" , 300 , 200 };

        //    memset(&newMess, 0, sizeof(newMess));    /* Zero out structure */
        //    strcpy(newMess.request_Type,"Send"); 
        //    newMess.SenderId = 300;
        //    newMess.RecipientId = 200;
        strncpy(newMess.message, argv[3],sizeof(argv[3]));

        printf("Size struct: %d\n", sizeof(newMess));

        servIP = argv[1];           /* First arg: server IP address (dotted quad) */
        echoServPort = atoi(argv[2]);  /* Use given port, if any */

        /* Create a datagram/UDP socket */
        if ((sock = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP)) < 0)
               DieWithError("socket() failed");

        /* Construct the server address structure */
        memset(&echoServAddr, 0, sizeof(echoServAddr));    /* Zero out structure */
        echoServAddr.sin_family = AF_INET;                 /* Internet addr family */
        echoServAddr.sin_addr.s_addr = inet_addr(servIP);  /* Server IP address */
        echoServAddr.sin_port   = htons(echoServPort);     /* Server port */

        int tempint = 0;

        /* Send the string to the server */
        tempint = sendto(sock, (struct TextMessage*)&newMess, (1024+sizeof(newMess)), 0, (struct sockaddr *)
         &echoServAddr, sizeof(echoServAddr)); 

        if (tempint == -1 ) {

               printf("Sent struct size: %d\n", tempint);
               DieWithError("sendto() sent a different number of bytes than expected\n");
        }

        close(sock);
        exit(0);
}


