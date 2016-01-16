//To connect use an FTP client. The port is CONTROL_PORT. To connect locally issue the following command:   
// #ftp 127.0.0.1 CONTROL_PORT   
   
//Includes   
#include <stdio.h> 
#include <sys/socket.h>  
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>  
#include <stdlib.h>   

//Defines   
#define CONTROL_PORT 1221
#define DATA_PORT 1220
#define SIZE 80
   
char sbuffer[SIZE],rbuffer[SIZE],userinfo[10];//send and receive buffers   
int n,bytes;//counters   
int ns,ns_data;//sockets, ns is a new socket (on CONTROL_PORT) and ns_data is a new socket for data (DATA_PORT)   
struct sockaddr_in remoteaddr, remoteaddr_data;   
int port_connect=0;   
   
//MAIN   
int main(int argc, char *argv[]) {   
    struct sockaddr_in localaddr;//local address structure   
    struct sockaddr_in local_data_addr;//local address struct for data connection   
    struct sockaddr_in local_data_addr_port;//local address struct for port connection to the client   
    int s,s_data,s_data_port;//welcome socket and welcome socket for data connection,and port connection for connect to client   
    int addrlen;//address lenght variable   
    int sy_error=1, user_ok=0, pass_ok=0; // use for indicate Syntax error, User and password authentication

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
    if (argc == 2) localaddr.sin_port = htons((u_short)atoi(argv[1]));   
    else localaddr.sin_port = htons(CONTROL_PORT);   
   localaddr.sin_addr.s_addr = INADDR_ANY;   
      
    local_data_addr.sin_port = htons(DATA_PORT);   
    local_data_addr.sin_addr.s_addr = INADDR_ANY;   
//BIND (bind uses ports CONTROL_PORT and DATA_PORT by default)   
    if (bind(s,(struct sockaddr *)(&localaddr),sizeof(localaddr)) < 0) {   
        printf("Bind failed!\n");    
    }   
    if (bind(s_data,(struct sockaddr *)(&local_data_addr),sizeof(local_data_addr)) <0) {   
        printf("Bind failed!\n");    
    }   
   
//INFINITE LOOP   
    while (1) { // while loop 1   
//LISTEN   
       listen(s,5);   
       listen(s_data,5);    
        addrlen = sizeof(remoteaddr);   
//ACCEPT main connection (control connection)   
        ns = accept(s,(struct sockaddr *)(&remoteaddr),&addrlen);   
       // if (ns == INVALID_SOCKET) break;   
        
        memset(sbuffer, 0, SIZE);
        user_ok=0; pass_ok=0;

        printf("connected to      %s at port %d \n",inet_ntoa(remoteaddr.sin_addr),ntohs(localaddr.sin_port));   

//Respond with welcome message, FTP client requires those   
        sprintf(sbuffer,"200 Welcome \r\n");   
        bytes = send(ns, sbuffer, strlen(sbuffer), 0);   
        sprintf(sbuffer,"530 Log in \r\n");   
        bytes = send(ns, sbuffer, strlen(sbuffer), 0);  

//INFINITE LOOP   
        while (1) // while loop 2   
        {   
            n = 0;

            memset(rbuffer, 0, sizeof(rbuffer));
            memset(userinfo, 0, sizeof(userinfo));
            //printf("userinfo main %s\n",userinfo);
             
            while (1)  // while loop 3   
            {   
//RECEIVE   
                bytes = recv(ns, &rbuffer[n], 1, 0);   
                if ((bytes < 0) || (bytes == 0)) break;   
                if (rbuffer[n] == '\n') { /*end on LF*/   
                    rbuffer[n] = '\0';   
                    break; }    
                if (rbuffer[n] != '\r') n++; /*ignore CR's*/   
            } // end of while loop 3    
            if ((bytes < 0) || (bytes == 0)) break;   
            printf("#The Server receives:# '%s' from client \n", rbuffer);  

      //COMMAND CHECKING

        if(strncmp(rbuffer,"USER",4)  && strncmp(rbuffer,"PASS",4)    &&  strncmp(rbuffer,"SYST",4)
                &&strncmp(rbuffer,"PORT",4)   && strncmp(rbuffer,"PASV",4)    &&  strncmp(rbuffer,"RETR",4)
                &&strncmp(rbuffer,"LIST",4)  &&  strncmp(rbuffer,"NLST",4)    &&  strncmp(rbuffer,"QUIT",4))
        {
            sy_error = 1; //user_ok=1; pass_ok=1;    
        } else {
            sy_error = 0; //user_ok=0; pass_ok=0; 
        }
            
   
//THE FTP COMMANDS HERE   
   
   
//USER   
            if (strncmp(rbuffer,"USER",4)==0){   
                printf("Logging in.\n"); 
                sscanf(rbuffer+5,"%s",userinfo);
                printf("userinfo %s\n",userinfo);
                if(strncmp(userinfo,"batch1",6) == 0) {
                  printf("USER \"batch1\" accepted. Asking for password...\n");
                  sprintf(sbuffer,"331 Enter password... \r\n");
                  user_ok=1;
                }
                else {
                  printf("USER match failed...\n");
                  sprintf(sbuffer,"530 User doesn't exist \r\n");
                  user_ok = 0;
                }    
                bytes = send(ns, sbuffer, strlen(sbuffer), 0);   
               // sy_error=0;   
            }   
//PASS     
         if (strncmp(rbuffer,"PASS",4)==0)  {   
                printf("Checking password... \n");   
                sscanf(rbuffer+5,"%s",userinfo);
                printf("userinfo pass %s\n",userinfo);
                if(strncmp(userinfo,"batch1",6) == 0) {
                  printf("PASS for USER \"batch1\" accepted.\n");
                  sprintf(sbuffer,"230 Logged in \r\n");
                  pass_ok=1;
                }
                else {
                  printf("PASS match failed...\n");
                  sprintf(sbuffer,"530 Password incorrect \r\n");
                  pass_ok = 0;
                }    
                bytes = send(ns, sbuffer, strlen(sbuffer), 0);   
               // sy_error=0;      
            } 

        if(user_ok == 1 && pass_ok == 1) {   //Allowing only authenticated persons to go ahead

//SYST         
           if (strncmp(rbuffer,"SYST",4)==0)  {   
                printf("Information about the system \n");   
                sprintf(sbuffer, "215 UNIX Type: L8 \r\n");   
                bytes = send(ns, sbuffer, strlen(sbuffer), 0);   
               // sy_error=0;   
               }   
                      
//PASV (need to change the ip address)  
             if (strncmp(rbuffer,"PASV",4)==0)  {   
                 printf("Passive mode \n");   
               //227 has a strange format, for IP 127.0.0.1 and port DATA_PORT   
               //the format, where a1.a2.a3.a4 is the IP address and p1*256+p2 is the port number.So it is!     
                 sprintf(sbuffer, "227 Passive Mode (%d,%d,%d,%d,%d,%d)\r\n",127,0,0,1,(DATA_PORT>>8),(DATA_PORT & 0x00FF));   
                 bytes = send(ns, sbuffer, strlen(sbuffer), 0);   
               //open a new connection on port DATA_PORT   
                 ns_data = accept(s_data,(struct sockaddr *)(&remoteaddr_data),&addrlen);   
                 printf("connected to      %s at port %d \n",inet_ntoa(remoteaddr_data.sin_addr),ntohs(local_data_addr.sin_port));
                 sprintf(sbuffer, "225 Passive Mode (%d,%d,%d,%d,%d,%d) \r\n",127,0,0,1,(DATA_PORT>>8),(DATA_PORT & 0x00FF));   
                 bytes = send(ns_data, sbuffer, strlen(sbuffer), 0);   
                // sy_error=0;   
               }   
   
//PORT    
//Specifies client IP and port to which the server should connect for the next file transfer.   
            if (strncmp(rbuffer,"PORT",4)==0) {   
            int conect_port[2];   
            int remote_ip[4], port_dec;   
            char ip_char[40];   
            port_connect=1;   
            printf("Using port mode, the client is waiting for connection \n");   
            //translate the ip and port from character to decimal number   
            sscanf(rbuffer, "PORT %d,%d,%d,%d,%d,%d",&remote_ip[0],&remote_ip[1],&remote_ip[2],&remote_ip[3],&conect_port[0],&conect_port[1]);   
            sprintf(ip_char, "%d.%d.%d.%d", remote_ip[0], remote_ip[1], remote_ip[2],remote_ip[3]);    
            local_data_addr_port.sin_family=AF_INET;       
            //change the format of port to be one decimal number   
            local_data_addr_port.sin_addr.s_addr=inet_addr(ip_char);   
            port_dec=conect_port[0];   
            port_dec=port_dec << 8;   
            port_dec=port_dec+conect_port[1];   
            printf("The client IP is %s, connect port is %d\n",ip_char,port_dec);   
            local_data_addr_port.sin_port=htons(port_dec);   
            if (connect(s_data_port, (struct sockaddr *)&local_data_addr_port,(int) sizeof(struct sockaddr)) != 0){   
                printf("try to connect to %s at port %d\n",inet_ntoa(local_data_addr_port.sin_addr),ntohs(local_data_addr_port.sin_port));   
                sprintf(sbuffer, "425 Can't open a port connection \r\n");   
                bytes = send(ns, sbuffer, strlen(sbuffer), 0);   
                close(s_data_port);}   
            else {   
                sprintf(sbuffer, "200 The server successful connect at port %d \r\n",port_dec);   
                bytes = send(ns, sbuffer, strlen(sbuffer), 0);   
                printf("Successful connected to client,ready to transfer.\n");}   
               
            }   
   
//LIST   
                         
             if (strncmp(rbuffer,"LIST",4)==0)  {   
                 printf("Equivalent to dir \n");   
                 system("dir > tmp.txt");    
                 FILE *fin=fopen("tmp.txt","r");   
                 sprintf(sbuffer, "125 Transfering... \r\n");   
                 bytes = send(ns, sbuffer, strlen(sbuffer), 0);   
                 char temp_buffer[100];   
                 while (!feof(fin)){   
                fgets(temp_buffer,98,fin);   
                sprintf(sbuffer,"%s \r\n",temp_buffer);   
                  if (port_connect==0) send(ns_data, sbuffer, strlen(sbuffer), 0);   
                    else send(s_data_port, sbuffer, strlen(sbuffer), 0);   
                 }   
                fclose(fin);   
                 sprintf(sbuffer, "226 Transfer completed... \r\n");   
                 bytes = send(ns, sbuffer, strlen(sbuffer), 0);   
                 system("del tmp.txt");   
               //CLOSE the ns_data SOCKET or data port SOCKET   
                if(port_connect==0)    
                  {   
                      close(ns_data);   
                      sprintf(sbuffer,"226 Close the data socket... \r\n");   
                     bytes = send(ns, sbuffer, strlen(sbuffer), 0);   
                      ns_data = socket(AF_INET, SOCK_STREAM, 0);   
                   }   
                else   
                  {    
                   close(s_data_port);               
                   sprintf(sbuffer,"226 Close the port connection... \r\n");   
                   bytes = send(ns, sbuffer, strlen(sbuffer), 0);   
                   s_data_port = socket(AF_INET, SOCK_STREAM, 0);   
                  }                    
                //sy_error=0;   
              }   
//RETR   
          if (strncmp(rbuffer,"RETR",4)==0)  {   
             printf("RETR mode.\r\n");   
             sprintf(sbuffer, "150 RETR file. Transfering....\r\n");    
             bytes = send(ns, sbuffer, strlen(sbuffer), 0);    
            int i=5,k=0;   
            char filename[10],ch[50];   
            // identify the filename from rbuffer after the word "RETR "   
            while(1)    
            {   
                   
                filename[k]=rbuffer[i];   
             if (rbuffer[i] == '\0') {    
                    filename[k] = '\0';   
                    break; }    
                  i++;   
                  k++;}   
                     
             char *p_filename=filename;        
             FILE *fp=fopen(p_filename,"r");   
            if (fp==NULL)   
             { sprintf(sbuffer, "Sorry, cannot open %s. Please try again.\n",filename);   
               bytes = send(ns, sbuffer, strlen(sbuffer), 0);      
               }   
                
             else   
              {   
               printf("The file %s found,ready to transfer.\n",filename);   
                  
              while (!feof(fp))   
                {   
                 fgets(ch,100,fp);   
                    sprintf(sbuffer,"%s \r\n",ch);   
                    if (port_connect==0) send(ns_data, sbuffer, strlen(sbuffer), 0);   
                      else send(s_data_port, sbuffer, strlen(sbuffer), 0);     
                 }   
              fclose(fp);   
              sprintf(sbuffer, "226 Transfer completed... \r\n");   
                  bytes = send(ns, sbuffer, strlen(sbuffer), 0);   
                  }   
                //CLOSE the ns_data SOCKET or data port SOCKET   
                if(port_connect==0)    
                  {   
                      close(ns_data);   
                      sprintf(sbuffer,"226 Close the data socket... \r\n");   
                     bytes = send(ns, sbuffer, strlen(sbuffer), 0);   
                      ns_data = socket(AF_INET, SOCK_STREAM, 0);   
                   }   
                else   
                  {    
                   close(s_data_port);               
                   sprintf(sbuffer,"226 Close the port connection... \r\n");   
                   bytes = send(ns, sbuffer, strlen(sbuffer), 0);   
                   s_data_port = socket(AF_INET, SOCK_STREAM, 0);   
                  }                    
                sy_error=0;  
              }   

         //ACCESS
          }else if ((user_ok == 0 && pass_ok == 0)) {
            printf("Unauthorised access...\n");
            sprintf(sbuffer, "332 Please log in.\n");   
            bytes = send(ns, sbuffer, strlen(sbuffer), 0); 
            //sy_error = 0; 
          }

          //QUIT   
                          
               if (strncmp(rbuffer,"QUIT",4)==0)  {   
                  printf("quitting..  \n");   
                  sprintf(sbuffer, "221 Logging Out... \r\n");   
                  bytes = send(ns, sbuffer, strlen(sbuffer), 0);   
                 sy_error=0;   
                 break;   
                 }  
   
//Syntax error   
           if (sy_error==1) {   
              printf("command unrecognized, non-implemented!\n");   
              sprintf(sbuffer, "500 Syntax error. \n");   
              bytes = send(ns, sbuffer, strlen(sbuffer), 0);       
               }   
           
        } // end of while loop 2   
   
      //CLOSE CONTROL SOCKET   
        close(ns);    
        printf("disconnected from %s at port %d, close control socket.\n",inet_ntoa(remoteaddr.sin_addr),ntohs(localaddr.sin_port));   
    
 } // end of while loop 1   
//CLOSE WELCOME SOCKET   
    close(s);   
    printf("Welcome sockets close");   
return 0;   
}   

