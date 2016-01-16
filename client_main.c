#include <stdio.h> 
#include <sys/socket.h>  
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <errno.h>
#include <string.h> 
#include <stdlib.h>   

//Defines     
#define SIZE 80

char sbuffer[SIZE],rbuffer[SIZE];//send and receive buffers   
int n,bytes;//counters   
int ns,ns_data;//sockets, ns is a new socket (on 1221) and ns_data is a new socket for data (1220)   
struct sockaddr_in remoteaddr, remoteaddr_data;   
int port_connect=0;   
   
int main(int argc, char *argv[]) {   
    struct sockaddr_in localaddr;//local address structure   
    struct sockaddr_in local_data_addr;//local address struct for data connection   
    struct sockaddr_in local_data_addr_port;//local address struct for port connection to the client   
    int s,s_data,s_data_port;//welcome socket and welcome socket for data connection,and port connection for connect to client   
    int addrlen;//address lenght variable   
    int sy_error=1; // use for indicate Syntax error   

   memset(&localaddr,0,sizeof(localaddr));//clear localaddr   
   memset(&local_data_addr,0,sizeof(local_data_addr));//clear local_data_addr   
   memset(&local_data_addr_port,0,sizeof(local_data_addr_port));//clear local_data_addr_port   


//SOCKETS (notice that there are three welcome sockets, one for control, one for data,another one for port connection   
    s = socket(AF_INET, SOCK_STREAM, 0);   
    s_data = socket(AF_INET, SOCK_STREAM, 0);   
    s_data_port = socket(AF_INET, SOCK_STREAM, 0);   
    if (s <0){   
        printf("socket failed\n");   
    }   

   localaddr.sin_family = AF_INET;   
   local_data_addr.sin_family = AF_INET;   
   if (argc == 2) 
   		localaddr.sin_port = htons((u_short)atoi(argv[1]));   
   else 
   		localaddr.sin_port = htons(1221);  
   localaddr.sin_addr.s_addr = INADDR_ANY;
      
   local_data_addr.sin_port = htons(1220);   
   local_data_addr.sin_addr.s_addr = INADDR_ANY;

   if(connect(s,(struct sockaddr *)&localaddr,sizeof(localaddr)) != 0) {
		perror("\nError in connecting to main socket");
		return 0;
	}
	printf("Successfully connected to main socket.\n");
	printf("Supported commands are: USER, PASS, SYST, PASV, PORT, LIST, RETR, QUIT.\n");

  fflush(stdout);
	bytes = recv(s,rbuffer,SIZE,0);
	printf("%s",rbuffer);
	// bytes = recv(s,rbuffer,SIZE,0);
	// printf("%s\n",rbuffer);

	// sprintf(sbuffer,"USER \r\n");   
 //    bytes = send(s, sbuffer, strlen(sbuffer), 0);  

	while(1) {

    fflush(stdout);
  	memset(sbuffer, 0, SIZE);
  	

		n = 0;

		printf("\nftp< ");
		while((sbuffer[n++]=getchar())!='\n');

    if(strncmp(sbuffer,"USER",4)  && strncmp(sbuffer,"PASS",4)    &&  strncmp(sbuffer,"SYST",4)
            &&strncmp(sbuffer,"PORT",4)   && strncmp(sbuffer,"PASV",4)    &&  strncmp(sbuffer,"RETR",4)
            &&strncmp(sbuffer,"LIST",4)  &&  strncmp(sbuffer,"NLST",4)    &&  strncmp(sbuffer,"QUIT",4))
    {
            bytes = send(s, sbuffer, strlen(sbuffer), 0);
            recv(s,rbuffer,SIZE,0);
            printf("%s\n",rbuffer );
    }

    memset(rbuffer, 0, SIZE);

		//USER   
        if (strncmp(sbuffer,"USER",4)==0){   
            bytes = send(s, sbuffer, strlen(sbuffer), 0);  
            recv(s,rbuffer,SIZE,0);
            printf("%s\n",rbuffer );
            sy_error=0;   
        }  

	    //PASS     
   		if (strncmp(sbuffer,"PASS",4)==0)  {   
            bytes = send(s, sbuffer, strlen(sbuffer), 0);  
            recv(s,rbuffer,SIZE,0);
            printf("%s\n",rbuffer );
            sy_error=0;   
        } 

        //SYST         
        if (strncmp(sbuffer,"SYST",4)==0)  {   
            bytes = send(s, sbuffer, strlen(sbuffer), 0);  
            recv(s,rbuffer,SIZE,0);
            printf("%s\n",rbuffer );
            sy_error=0;  
        }  

        //PASV 
  	    if (strncmp(sbuffer,"PASV",4)==0)  {   
              int conect_port[2];  
              int remote_ip[4], port_dec;   
              char ip_char[40];   

              bytes = send(s, sbuffer, strlen(sbuffer), 0);  
              recv(s,rbuffer,SIZE,0);
              if(strncmp(rbuffer,"332",3)!=0) {
              printf("%s",rbuffer );

              //translate the ip and port from character to decimal number   
              sscanf(rbuffer,"227 Passive Mode (%d,%d,%d,%d,%d,%d)",&remote_ip[0],&remote_ip[1],&remote_ip[2],&remote_ip[3],&conect_port[0],&conect_port[1]);   
              sprintf(ip_char, "%d.%d.%d.%d", remote_ip[0], remote_ip[1], remote_ip[2],remote_ip[3]); 
            //  printf("The ip is %s\n",ip_char);
            //  printf("port0 %d, port1 %d\n",conect_port[0],conect_port[1]);
              //change the format of port to be one decimal number   
              port_dec=conect_port[0];   
              port_dec=port_dec << 8;   
              port_dec=port_dec+conect_port[1];   
              printf("Connected to data port %d\n",port_dec); 
              local_data_addr.sin_port=htons(port_dec);   

              memset(rbuffer, 0, SIZE);

              if(connect(s_data,(struct sockaddr *)&local_data_addr,sizeof(local_data_addr)) == -1) {
  				      perror("\nError in connecting to data socket");
  				      return 0;
  			     }

      			recv(s_data,rbuffer,SIZE,0);
            printf("%s",rbuffer );
          }else
      			printf("%s",rbuffer );

            sy_error=0;   
  	    }

	    //QUIT   
                          
       if (strncmp(sbuffer,"QUIT",4)==0)  {   
            bytes = send(s, sbuffer, strlen(sbuffer), 0);  
            recv(s,rbuffer,SIZE,0);
            printf("%s\n",rbuffer );
            return 0;
         }   

                 

	}
	return 0;
}
      