#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<unistd.h>
#include<arpa/inet.h>

int main()
{
	char *ip="127.0.1.1";
	int port=5566;

	int  sock;
	struct sockaddr_in addr;
	socklen_t addr_size;
	double tempField[64][64];
	int n;

	sock=socket(AF_INET,SOCK_STREAM,0);
	if(sock<0)
	{
		perror("[-]Socket error");
		exit(1);
	}
	printf("[+]TCP server socket created.\n");

	memset(&addr,'\0',sizeof(addr));
	addr.sin_family=AF_INET;
	addr.sin_port=port;
	addr.sin_addr.s_addr=inet_addr(ip);

	connect(sock, (struct sockaddr*)&addr,sizeof(addr));
	printf("Connected to the server.\n");

	// bzero(buffer,1024);
	// strcpy(buffer,"Hello, this is client.");
	// printf("Client: %s\n",buffer);
	// send(sock,buffer,strlen(buffer),0);

	// bzero(buffer,1024);
	int T=5;
	for(int t=0;t<T;t++)
	{
		recv(sock,tempField,sizeof(tempField),0);
		printf("Recieved %d\n",t);
		// printf("%f,%f,%f,%f\n",tempField[t][0][0],tempField[t][0][1],tempField[t][1][0],tempField[t][1][1]);
		printf("%f,%f,%f,%f\n",tempField[50][0],tempField[50][30],tempField[30][50],tempField[30][50]);
		// printf("Recieved %d",t);
	}
	
	// printf("Server: %s\n",buffer);

	close(sock);
	printf("Disconnected from the server.\n");

	return 0;
}