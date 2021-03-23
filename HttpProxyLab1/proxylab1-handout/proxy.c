
#include <arpa/inet.h>
#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/socket.h>
#include <netdb.h>
#include <signal.h>
#include "csapp.h"
#include "sbuf.h"
// #include "csapp.h"

#define HTTP_REQUEST_MAX_SIZE 4096
#define HOSTNAME_MAX_SIZE 512
#define PORT_MAX_SIZE 6
#define URI_MAX_SIZE 4096
#define METHOD_SIZE 32

#define BUF_SIZE 500

/* Recommended max cache and object sizes */
#define MAX_CACHE_SIZE 1049000
#define MAX_OBJECT_SIZE 102400

#define	MAXLINE	 8192  /* Max text line length */
#define MAXBUF   8192  /* Max I/O buffer size */
#define LISTENQ  1024  /* Second argument to listen() */

// sbuf_t buf;


/* You won't lose style points for including this long line in your code */
static const char *user_agent_hdr = "User-Agent: Mozilla/5.0 (X11; Linux x86_64; rv:10.0.3) Gecko/20120305 Firefox/10.0.3\r\n";

int is_complete_request(const char *request) {
	int len = strlen(request);
	if(len < 4) return 0;
	char c1[2];
	char c2[2];
	char c3[2];
	char c4[2];
	sprintf(c1, "%c", request[len-1]);
	sprintf(c2, "%c", request[len-2]);
	sprintf(c3, "%c", request[len-3]);
	sprintf(c4, "%c", request[len-4]);
	if(strcmp(c1, "\n")) return 0;
	if(strcmp(c2, "\r")) return 0;
	if(strcmp(c3, "\n")) return 0;
	if(strcmp(c4, "\r")) return 0;
	return 1;
}

int parse_request(const char *request, char *method,
	char *hostname, char *port, char *uri) {
		// printf("%s", request);
	//method
	strcpy(method, request);
	strtok(method, " ");

    char ip[100];
    int p = 80;
	char pString[100];
    char page[100];
	if(sscanf(request, "GET http://%99[^:]:%99d/%99[^\n]", ip, &p, page)==3);
	else if(sscanf(request, "GET http://%99[^/]/%99[^\n]", ip, page)==2);

	strcpy(hostname, ip);
	sprintf(pString, "%d", p);
	strcpy(port, pString);
	strcpy(uri, page);

	return is_complete_request(request);
}

int main(int argc, char *argv[])
{
	
    int listenfd, connfd;
    char hostname[MAXLINE], port[MAXLINE], method[METHOD_SIZE], uri[URI_MAX_SIZE];
    socklen_t clientlen;
    struct sockaddr_storage clientaddr;

	/* Check command line args */
    if (argc < 2) {
		fprintf(stderr, "usage: %s <port>\n", argv[0]);
		exit(1);
    }

	// sbuf_init(&buf, BUF_SIZE);
    listenfd = Open_listenfd(argv[1]);
    while (1) {
		clientlen = sizeof(clientaddr);
		connfd = Accept(listenfd, (SA *)&clientaddr, &clientlen);
		Getnameinfo((SA *) &clientaddr, clientlen, hostname, MAXLINE, 
					port, MAXLINE, 0);
		printf("Accepted connection from (%s, %s)\n", hostname, port);
		// doit(connfd); 
		int index = 0;
		int nread = 0;
		char buf[MAXBUF] = "";
		for(;;){
			nread = read(connfd, buf + index, MAX_OBJECT_SIZE);
			index += nread;
			if(nread == 0)break;
			if(index < 4)continue;
			if(!memcmp(buf+index - 4, "\r\n\r\n", 4))break;
		}
		printf("%s", buf);
		if(parse_request(buf, method, hostname, port, uri)){
			printf("req method: %s\n", method);
			printf("req1 hostname: %s\n", hostname);
			printf("req1 port: %s\n", port);
			printf("req1 uri: /%s\n", uri);
			struct addrinfo hints, *result, *rp;
			int sfd, s;
			
			memset(&hints, 0, sizeof(struct addrinfo));
			hints.ai_family = AF_INET;
			hints.ai_socktype = SOCK_STREAM;
			hints.ai_flags = 0;
			hints.ai_protocol = 0;
			s = getaddrinfo(hostname, port, &hints, &result);
			if (s != 0) {
				fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(s));
				exit(EXIT_FAILURE);
			}
			for(rp = result; rp != NULL; rp = rp->ai_next){
				sfd = socket(rp->ai_family, rp->ai_socktype, rp->ai_protocol);
				if(sfd == -1) continue;
				if(connect(sfd, rp->ai_addr, rp->ai_addrlen) != -1){
					printf("Success connecting to server\n");
					break;
				} else {
					printf("failure");
				}
				// close(sfd);
			}
			if (rp == NULL) {               /* No address succeeded */
				fprintf(stderr, "Could not connect\n");
				exit(EXIT_FAILURE);
			}

			freeaddrinfo(result);           /* No longer needed */


			// if (write(sfd, buf, BUF_SIZE) != BUF_SIZE) {
			// 	fprintf(stderr, "partial/failed write\n");
			// 	exit(EXIT_FAILURE);f
			// }
			// printf("Sent %d bytes to server\n", BUF_SIZE);
			char message[1024];
			strcpy(message, method);
			strcat(message, " /");
			strcat(message, uri);
			strcat(message, "\r\n");
			strcat(message, user_agent_hdr);
			strcat(message, "Connection: close\r\n");
			strcat(message, "Proxy-Connection: close\r\n");
			strcat(message, "\r\n\r\n");

			printf("this is being sent as a message to tiny: \n%s\n", message);
			int numBytesRead = strlen(message);
			int numSentBytes = 0;
			int numReceivedBytes = 0;
			do{
				numReceivedBytes = write(sfd, message + numSentBytes, numBytesRead - numSentBytes);
				if(numReceivedBytes <= -1){
					printf("Error Sending bytes\n");
				}
				numSentBytes += numReceivedBytes;
			}while(numSentBytes < numBytesRead);
			printf("here\n");
			// int returnedBytes = 0;
			numBytesRead = 0;
			char *sbuf = calloc(MAX_OBJECT_SIZE, sizeof(char));
			do{
				nread = read(sfd, sbuf + numBytesRead, MAX_OBJECT_SIZE);
				if (nread == -1) {
					break;
				}
				numBytesRead += nread;
			}while(nread != 0);
			// printf("%s\n", sbuf);

			numSentBytes = 0;
			numReceivedBytes = 0;
			do{
				numReceivedBytes = write(connfd, sbuf + numSentBytes, numBytesRead - numSentBytes);
				if(numReceivedBytes == -1){
					printf("Error Sending bytes\n");
				}
				numSentBytes += numReceivedBytes;
			}while(numSentBytes < numBytesRead);
			printf("%d\n", numSentBytes);

			close(sfd);
		}

		// sbuf_insert(&buf, connfd);
		close(connfd);
    }
}
