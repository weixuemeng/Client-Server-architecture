#include <sys/socket.h>
#include <sys/types.h>
#include <sys/signal.h>
#include <sys/resource.h>
#include <pthread.h>
#include <netinet/in.h>
#include <netdb.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <arpa/inet.h> 
#include <sys/wait.h>
int MAX_BUFFER=1024;

int main(int argc, char *argv[])
{   
    if(argc != 3)
    {
        printf("\n Usage: %s <port> <ip of server> \n",argv[0]);
        return 1;
    } 

    int sockfd = 0, n = 0;
    int sendBuff[MAX_BUFFER];
    char recvBuff[MAX_BUFFER];
    struct sockaddr_in serv_addr; 
    pid_t pid = getpid();
    // log file 
    FILE *fd;
    char *file_name = (char*) malloc(30*sizeof(char));
    sprintf(file_name, "client.%d.log",pid);
    fd = fopen(file_name,"w");
    
    memset(sendBuff, '0',sizeof(sendBuff));
    memset(recvBuff, '0',sizeof(recvBuff));

    if((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        printf("\n Error : Could not create socket \n");
        return 1;
    } 

    memset(&serv_addr, '0', sizeof(serv_addr)); 
    serv_addr.sin_family = AF_INET; 
    serv_addr.sin_port = htons(atoi(argv[1])); 

    if(inet_pton(AF_INET, argv[2], &serv_addr.sin_addr)<=0)
    {
        printf("\n inet_pton error occured\n");
        return 1;
    } 
    
    int quit = 0;
    while (quit == 0){
        int i;
        char buf[4];
        i = scanf("%s",buf);
        if ( i==EOF){
            quit = 1;
            break;
        }
        char temp[5] = "";
        for ( int i = 1; i<4; i++){
            strcat(temp, &buf[i]);
        }
        int amount= atoi(temp);

        // create connection
        if( connect(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
        {
            printf("\n Error : Connect Failed \n");
            return 1;
        } 

        //write to socket (send)
        sendBuff[0] = pid;
        sendBuff[1] = amount;
        write(sockfd, sendBuff, 255);

        // write to file
        
        // wait for respond
        for( ; ; )
        {
            n = read(sockfd, recvBuff, sizeof(recvBuff)-1);
            printf( "N=%d\n", n );
            if( n <= 0 ) break;
            recvBuff[n] = 0;
            if(fputs(recvBuff, stdout) == EOF)
            {
                printf("\n Error : Fputs error\n");
            }
        }
        // read error
        if(n < 0)
        {
            printf("\n Read error \n");
        } 
    }

    close(fd);

    return 0;
}

