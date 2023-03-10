#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <netdb.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <arpa/inet.h> 
int MAX_BUFFER=1024;
int TransSave = 0;
void Trans(int n);
void Sleep(int n);

void Trans( int n ) {
	long i, j;

	// Use CPU cycles 
	j = 0;
	for( i = 0; i < n * 100000; i++ ) {
		j += i ^ (i+1) % (i+n);
	}
	TransSave += j;
	TransSave &= 0xff;
}


// Sleep simulates pauses between transactions
// Parameter given expresses wait time in hundreds of a nanosecond.

#include <stdio.h>
#include <time.h>

void Sleep( int n ) {
	struct timespec sleep;

	// Make sure pass a valid nanosecond time to nanosleep
	if( n <= 0 || n >= 100 ) {
		n = 1;
	}

	// Sleep for less than one second
	sleep.tv_sec  = 0;
        sleep.tv_nsec = n * 10000000 + TransSave;
	if( nanosleep( &sleep, NULL ) < 0 ) {
		perror ("NanoSleep" );
	}
}


int main(int argc, char *argv[])
{
    if (argc !=2){
        printf("\n Usage: %s <port> \n",argv[0]);
        return 1;
    }

    int listenfd = 0, connfd = 0;
    struct sockaddr_in serv_addr, cli_addr; 
    int bytes;

    char sendBuff[MAX_BUFFER];
    int recvBuff[MAX_BUFFER]; // 1. pid of client, 2. task amount
    int port = atoi(argv[1]);

    time_t ticks; 

    listenfd = socket(AF_INET, SOCK_STREAM, 0);
    memset(&serv_addr, '0', sizeof(serv_addr));
    memset(sendBuff, '0', sizeof(sendBuff)); 
    memset(recvBuff, '0', sizeof(recvBuff)); 


    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    serv_addr.sin_port = htons(port); 

    bind(listenfd, (struct sockaddr*)&serv_addr, sizeof(serv_addr)); 

    listen(listenfd, 10); 

    while(1)
    {
        printf( "wait for a connection\n" );
        connfd = accept(listenfd, (struct sockaddr*)NULL, NULL); // add lock here?
        printf( "connection made\n" );

        // get message from client
        int m = read(connfd, recvBuff, 255);
        if (m<0){
            printf("\n Read message from client fails \n");
        }
        // add pid, amount into a queue ( scheduling?)


        ticks = time(NULL);
        snprintf(sendBuff, sizeof(sendBuff), "%.24s\r\n", ctime(&ticks));
        bytes = write(connfd, sendBuff, strlen(sendBuff) );
        printf( "data received: %d bytes\n", bytes );
        if( bytes != strlen(sendBuff) ) 
        {
            perror("Write failed");
            exit(1);
        }
        printf( "data received: %s\n", sendBuff );

        close(connfd);
        sleep(1);
     }
}
