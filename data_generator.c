#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <stdio.h>
#include <errno.h>
#include <unistd.h>
#include <stdlib.h>

//Problems faced 
//This code just runs once, because after another loop , scanf disturbs things up

int portno( char c)
{
if (c =='a' || c =='A')
return 10000;

else if (c =='b' || c =='B')
return 10001;

else if (c =='c' || c =='C')
return 10002;

else if (c =='d' || c =='D')
return 10003;

else if (c =='e' || c =='E')
return 10004;

else if (c =='f' || c =='F')
return 10005;

else if (c =='g' || c =='G')
return 10006;
}

int main(int argc,char **argv)
{


struct sockaddr_in self_details, other_details;

char recvmessage[1024];

//r2 details


/*
self_details.sin_family = AF_INET ;
self_details.sin_addr.s_addr = inet_addr("127.0.0.1");
self_details.sin_port = htons(10000);
printf("\nself_socket details added");
*/

int self_socket =  socket(AF_INET,SOCK_DGRAM,IPPROTO_UDP);


/*if ( bind (self_socket , (struct sockaddr*)&self_details , sizeof(self_details)) == -1)
{
printf("\nerror binding socket-port");
exit(1);
}
else 
{
printf("\nSocket binded to port successfully\n");
}    

NOTE- FAIRL IMPORTANT THAT FOR UDP ITS CONNECTION LESS, SO NO NEED TO BIND IF U R SENDING ONLY, SIMPLY CREATE A SOCKET AND send to that socket with correct address
 */ 




printf("\nDestination router?\n");
char dest;
scanf("%c",&dest);
printf("\nDest is %c\n",dest);

printf("\nSource router?\n(we will send your message to this router attached with destination address)\n");

char source;                             // scanf eats it up---> explanation I learnt https://stackoverflow.com/questions/20139415/problems-with-scanf 
scanf(" %c",&source);
printf("\nSource is %c\n",source);

char message[30];
printf("\nEnter messgae to send to your source router %c...\n(format of packet will be DATA:[source]:[destination]:message)\n",source);
scanf("%s",message);

char sendmessage[1024] = "DATA:[";
char append[5] ;
append[0] = source ; append[1] ='\0';
strcat (sendmessage,append);   //DATA:[source
append[0] = ']' ; append[1] =':'; append[2] ='['; append[3] ='\0';
strcat (sendmessage,append); //DATA:[source]:[
append[0] = dest ; append[1] =']'; append[2] =':'; append[3] ='\0';
strcat (sendmessage,append); 
strcat (sendmessage,message); 




printf("Sending this message to router source %c :%s",source,sendmessage);

other_details.sin_family = AF_INET ;
other_details.sin_addr.s_addr = inet_addr("127.0.0.1");
other_details.sin_port = htons(portno(source));

if (  sendto(self_socket , sendmessage , sizeof(sendmessage) , 0 , (struct sockaddr*)&other_details ,sizeof(other_details)) <= 0)
printf("\nError encountered while sending message");
else
printf("\nMessage sent to router source %c\n",source);





}

