//solving for CONTROL format CONTROL:[C]:99,3,0,2,99,1,99: <CONTROL:SRC:DV:>

//solved by 1. used global buffer for recievingrather than individual thread using function recvfrom()  2. Removed all the mutex (initially used for data integrity but fuck that) and there are no locks


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <pthread.h>
#define INFINITE 99

#include<time.h>


//Problem in matrix approach - cannot add new router ------SOLVED INCREASED MATRIX NxN N SIZE

//probelms 1 ---How can i make it send DV after every 30 sec, if not recieved in 180 sec then i will declare my router dead

//segmentation fault problems, ending with \0   

//format problems ---practice

//scanf bakchodi, eats newline character

//change other details
//problem 2 --- implementing forwarding table

//----alive
// We checked neighbour becoming aware of alive has to "restart itself" and send alive packet to its neighbours for proper implementation
// single dead->alive  , double dead->alive

//PROTOTYPES
void sendDV (void );
void *keeprecv (void );
void print_rt(void);
int min (int a, int b);
void set_routing_table();
char notochar (int no);
char Initialize_Function (char routerName);
char* itoa(int value, char* buffer, int base);


int self_socket ;
int selfdv[7][7];
int livemates[7] = {0,0,0,0,0,0,0};     //all dead assuming initially
int selfno ;                            //like 0 for a, 1 for b, 2 for c and so on

struct sockaddr_in self_details, other_details;
char sendmessage[256];
char recvmessage[256];
int changeflag =0 ;

//When we recieve a dead router signal from any router, this global flag called dead, will be used to set as 1 and router will restart with distance from it as infinite
// 1 means dead, 0 means alive , NULL or '\0' means default_value_of initialization
int dead[7] = {0,0,0,0,0,0,0};

struct routing_table
{
char destination ;
int cost;
char nexthop ;
};
struct routing_table routingtable[7] = {{'\0',99,'\0'},{'\0',99,'\0'},{'\0',99,'\0'},{'\0',99,'\0'},{'\0',99,'\0'},{'\0',99,'\0'},{'\0',99,'\0'}  } ;

///~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~Set routing table based on selfdv is properly initialized ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void set_routing_table ()
{
int i;
for (i=0 ; i<7 ; i++)
{
if (selfdv[selfno][i] > 0 && selfdv[selfno][i] < INFINITE)
{
routingtable[i].destination = notochar(i) ;   //THIS MAKES ROUTING TABLE FOR NEIGHBOUR LIKE ->  B , 3 , B (destination, cost , nexthop), other than neighbours are NULL
routingtable[i].cost = selfdv[selfno][i] ;
routingtable[i].nexthop = notochar(i) ;
}

}
}


//FUNCTION WHICH RETURN PORT NO BY CHECKING ROUTER CHAR~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
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
// MIn function
int min (int a, int b)
{
if (a<b)
return a;
if (b<=a)
return b;


}

//Initializing function of self DV
void in()
{

int i=0;
int j=0;
for (i=0 ; i<7 ; i++)
{
for(j=0;j<7;j++)
{
selfdv[i][j] = INFINITE;
}
}

}
//////////////////////////////////////////

void print_rt()
{
int i;

for(i=0 ; i<7 ; i++)
printf("Destination %c -- Cost %d -- NextHop %c\n",routingtable[i].destination , routingtable[i].cost , routingtable[i].nexthop ) ;



}

//////////////////////////////// selfno and char relation//////////////////////////////////////////////////

char notochar (int no)
{
if (no == 0 )
return ('A');
else if (no == 1 )
return ('B');
else if (no == 2 )
return ('C');
else if (no == 3 )
return ('D');
else if (no == 4 )
return ('E');
else if (no == 5 )
return ('F');
else if (no == 6 )
return ('G');
}

int chartono (char c)
{
if (c =='a' || c =='A')
return (0);

else if (c =='b' || c =='B')
return (1);

else if (c =='c' || c =='C')
return (2);

else if (c =='d' || c =='D')
return (3);

else if (c =='e' || c =='E')
return (4);

else if (c =='f' || c =='F')
return (5);

else if (c =='g' || c =='G')
return (6);

}






//#################################A thread runs below function, sending DVz to neighbours every 5 sec #########################################

void * sendDV_5sec (void)
{


time_t seconds, seconds_initial;
seconds_initial = time(NULL);
while(1)
{


   seconds = time(NULL);

if (seconds - seconds_initial >= 5)
{
printf("\n5sec expired, sending DV to neighbours\n");

sendDV();

seconds_initial = seconds;
}

}

}
//###################################### Two thread functions which take appropriate actions on outer failure and appearance ########################################3

void * dead_router_action (void* t)
{
//Neighbour number t just got declared dead bro, fuck, something must be done (livemates[] personal array for every router has already been modified so dont worry)
int to = (int)t;
//int before = selfdv[selfno][to];
dead[to]= 1;

//LOGIC IS, WHENEVR ROUTER IS DEAD, ALL ITS NEIGHBOURS MUST UPDATE ACCORDINGLY, WILL RESTART WITH THIS DEAD DETAIL, AND ASK OTHER ROUTERS TO TO THE SAME (EVERY ROUTER WILL HAVE dead flag=1)

//Send DEAD packet to all neighbours ------> DEAD4 -------- which means router no 4 is no more, say amen! amen! amen!
//send dv to neighbour 
char sendN[50] = "DEAD"; // A ]: attach char according to need
char append[7] = { '\0' } ;
char buffer [10] ={'\0'};
int i;
itoa (to ,buffer,10) ;
buffer[1] = '\0';
append[0]= buffer[0]; append[1]='\0';
strcat(sendN,append); // DEAD(to)

for(i=0; i<7 ; i++)
{
if (livemates[i] == 1)
{
//send to socket to destination router ( notochar i )
struct sockaddr_in uther_details;
uther_details.sin_family = AF_INET ;
uther_details.sin_addr.s_addr = inet_addr("127.0.0.1");
uther_details.sin_port = htons(portno(notochar(i)));

if (  sendto(self_socket , sendN , sizeof(sendN) , 0 , (struct sockaddr*)&uther_details ,sizeof(uther_details)) <= 0)
printf("\nError encountered while sending message");
else
printf("\nDEAD%d signal sent to router %c by router %c\n\n",to,notochar(i),notochar(selfno));

}
}
// Uptil no -----this router sends its neighbours DEAD(t0) declaration , so they will recieved DEAD(t0) format packet, upon which they will too restart




// Restarting own-self this very Router

in();                                            // resets all selfdv to INFINITE
Initialize_Function ( notochar(selfno)) ;  
int j;     // Initializes selfdv with neighbours' and livemates
for(j=0 ; j<7 ; j++)
{
        if (dead[j] == 1 ) 
        {
        livemates[j] = 0;
        selfdv[selfno][j] = INFINITE;
        }
 }
        for( j=0 ; j<7;j++)
        {
 routingtable[j].destination = '\0' ;
 routingtable[j].cost= INFINITE;
 routingtable[j].nexthop = '\0' ;
 }
set_routing_table();            // This initializes routing table according to given initialization + (now the router would be dead)
sendDV(); //send DV to neighbours






}




//#################################A thread runs below function, CHECKING FOR WHICH OF THE NEIGHBOURS FALLS DEAD (ROUTER FAILURE) #########################################
//waiting for control message

void * keepALIVE (void)
{

time_t seconds , seconds_initial;
//int deadflag =0;


 int flag[7] = {0,0,0,0,0,0,0};     //Initialise which neighbours were alive and loop below will check if no response from these neighbours, it setrs flag down which ultimately 0z livemates(dead)

int livemates_initial[7];
int f;
for (f=0 ; f<7 ;f++)
{
livemates_initial[f] = livemates[f];
}


seconds_initial = time(NULL);
while(1)
{  

//recvsize is always -1 or else i wud had used if recvsize > 0 , check what is at message[9] and set its flag =1 
// CONTROL:[src]:1,2,3,4,5,6

if ( recvmessage[9] == 'A')
flag[0] = 1 ;
if ( recvmessage[9] == 'B')
flag[1] = 1 ;
if (recvmessage[9] == 'C')
flag[2] = 1 ;
if ( recvmessage[9] == 'D')
flag[3] = 1 ;
if ( recvmessage[9] == 'E')
flag[4] = 1 ;
if ( recvmessage[9] == 'F')
flag[5] = 1 ;
if ( recvmessage[9] == 'G')
flag[6] = 1 ;


seconds = time(NULL);
//Broken solution - when if is runned, above flags are not receptive, ---below livemates get set to 0 (for wrong) so senDV is not sending DV TO EVEN ALIVE NEIGHBOURS ANYMORE  (both declaring each other dead)   ------- on the other hand, every router is sending every 5 seconds, so shouldnt be this problem

if (seconds - seconds_initial >= 15)
{

for (f=0 ; f<7 ;f++)
{
livemates[f] = flag[f];         //CHANGE THE STATUS OF LIVEMATES so that they are now decalred dead after 25seconds if no CONTROL MESSAGE RECIEVED, or else the should be alive
}


for (f=0 ; f<7 ;f++)         //DETECTING CHANGE IN LIVE MATES< SO THAT WE CAN DETECT DEAD ROUTER AND TAKE APPROPRIATE ACTIONS
{
if(livemates_initial[f] ==1 && livemates[f]== 0)
{printf("\n\nDeclaring neighbour %c Dead\n\n",notochar(f));
pthread_t dead;
pthread_create(&dead,NULL, dead_router_action ,(void*)f);
pthread_join(dead , NULL);
}
if(livemates_initial[f] ==0 && livemates[f]== 1) //Why not thread this action? cause too many warnings during compiling that it didnt compiled the code!!!! that was crazy btw, so had to write down proper action under this only
{
printf("\n\nNeighbour %c came back to life !\n\n",notochar(f));
dead[f] = 0   ; //Making it alive , but in this run when in recv_function , bellman algo will not run as by present situation dead=1 , in the next CONTROL statement bellman would run



//----------------------------------------------------STEP 2 - send ALIVE(to) packets to all neighbours so that they can apply bellman for this alive node rather than ignoring bellman for it
char alive_packet[10] = "ALIVE" ;
char buf[2];
itoa(f,buf,10) ;
strcat (alive_packet , buf) ;
int i;
for(i=0; i<7 ; i++)
{
if (livemates[i] == 1)
{
//send to socket to destination router ( notochar i )
struct sockaddr_in uther_details;
uther_details.sin_family = AF_INET ;
uther_details.sin_addr.s_addr = inet_addr("127.0.0.1");
uther_details.sin_port = htons(portno(notochar(i)));

if (  sendto(self_socket , alive_packet , sizeof(alive_packet) , 0 , (struct sockaddr*)&uther_details ,sizeof(uther_details)) <= 0)
printf("\nError encountered while sending message");
else
printf("\nALIVE%d signal sent to router %c by router %c\n\n",f,notochar(i),notochar(selfno));

}
}

//----//------------STEP 3                      We have to reset initial distance of that neighbour----- only this router needs to restart when neighbour found alive
// Restarting own-self this very Router

in();                                            // resets all selfdv to INFINITE
Initialize_Function ( notochar(selfno)) ;  
int j;     // Initializes selfdv with neighbours' and livemates
for(j=0 ; j<7 ; j++)
{
        if (dead[j] == 1 ) 
        {
        livemates[j] = 0;
        selfdv[selfno][j] = INFINITE;
        }
 }
        for( j=0 ; j<7;j++)
        {
 routingtable[j].destination = '\0' ;
 routingtable[j].cost= INFINITE;
 routingtable[j].nexthop = '\0' ;
 }
set_routing_table();            // This initializes routing table according to given initialization + (now the router would be dead)
sendDV(); //send DV to neighbours

}
}

for (f=0 ; f<7 ;f++)
{
livemates_initial[f] = livemates[f];     //RESET LIVEMATES_INITIAL TO DETECT NEW DEAD&ALIVE ROUTERS
}

for (f=0 ; f<7 ;f++)               //FLAG RESET AGAIN to see which routers are sending messages again
{
flag[f] =0 ;
}

seconds_initial = seconds;
}

}


}

///####################################################################################################################################################

char Initialize_Function (char routerName)
{

if ( routerName == 'A' || routerName == 'a')
{
selfdv[0][0] = 0;
selfdv[0][1] = 3;
selfdv[0][4] = 1;
selfdv[0][2] = INFINITE;
selfdv[0][3] = INFINITE;
selfdv[0][5] = INFINITE;

//Initializing live mates (not counting itself)
livemates[1] = 1;
livemates[4] = 1;


}
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
else if  (routerName == 'B'||routerName =='b')
{
selfdv[1][1] = 0;
selfdv[1][0] = 3;
selfdv[1][2] = 3;
selfdv[1][4] = 2;
selfdv[1][5] = 1;
selfdv[1][3] = INFINITE;


//Initializing live mates (not counting itself)
livemates[0] = 1;
livemates[2] = 1;
livemates[4] = 1;
livemates[5] = 1;

}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
else if  (routerName == 'C'||routerName =='c')
{
selfdv[2][2] = 0;
selfdv[2][1] = 3;
selfdv[2][5] = 1;
selfdv[2][3] = 2;
selfdv[2][0] = INFINITE;
selfdv[2][4] = INFINITE;

//Initializing live mates (not counting itself)
livemates[1] = 1;
livemates[5] = 1;
livemates[3] = 1;


}
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
else if  (routerName ==  'D'|| routerName =='d')
{
selfdv[3][3] = 0;
selfdv[3][2] = 2;
selfdv[3][5] = 3;
selfdv[3][1] = INFINITE;
selfdv[3][0] = INFINITE;
selfdv[3][4] = INFINITE;

//Initializing live mates (not counting itself)
livemates[2] = 1;
livemates[5] = 1;


}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
else if  (routerName == 'E'|| routerName =='e')
{
selfdv[4][4] = 0;
selfdv[4][0] = 1;
selfdv[4][5] = 3;
selfdv[4][1] = 2;
selfdv[4][2] = INFINITE;
selfdv[4][3] = INFINITE;

//Initializing live mates (not counting itself)
livemates[1] = 1;
livemates[5] = 1;
livemates[0] = 1;


}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
else if  (routerName == 'F'|| routerName =='f')
{
selfdv[5][5] = 0;
selfdv[5][4] = 3;
selfdv[5][2] = 1;
selfdv[5][1] = 1;
selfdv[5][3] = 3;
selfdv[5][0] = INFINITE;

//Initializing live mates (not counting itself)
livemates[1] = 1;
livemates[2] = 1;
livemates[3] = 1;
livemates[4] = 1;

}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
else {
printf ("\nkya enter kar diya bey? inside argv[1]\n"); //NO STACK SMASHING IN THIS CASE

return 1;
}

}




/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void swap(char *x, char *y) {
	char t = *x; *x = *y; *y = t;
}


// function to reverse buffer[i..j]////////////////////////////////////////////////////////////////////////////////////////////////
char* reverse(char *buffer, int i, int j)
{
	while (i < j)
		swap(&buffer[i++], &buffer[j--]);

	return buffer;
}


///////////////////////////////////////////     ASCI TO INTEGER FUNCTION ////////////////////////////////////////////////////////
int myatoi(char *str)
{
    int res = 0; // Initialize result
  
    // Iterate through all characters of input string and
    // update result
    for (int i = 0; str[i] != '\0'; ++i)
        res = res*10 + str[i] - '0';
  
    // return result.
    return res;
}



// Iterative function to implement itoa() function in C//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
char* itoa(int value, char* buffer, int base)
{
	// invalid input
	if (base < 2 || base > 32)
		return buffer;

	// consider absolute value of number
	int n = abs(value);

	int i = 0;
	while (n)
	{
		int r = n % base;

		if (r >= 10) 
			buffer[i++] = 65 + (r - 10);
		else
			buffer[i++] = 48 + r;

		n = n / base;
	}

	// if number is 0
	if (i == 0)
		buffer[i++] = '0';

	// If base is 10 and value is negative, the resulting string 
	// is preceded with a minus sign (-)
	// With any other base, value is always considered unsigned
	if (value < 0 && base == 10)
		buffer[i++] = '-';

	buffer[i] = '\0'; // null terminate string

	// reverse the string and return it
	return reverse(buffer, 0, i - 1);
}

//////////////////////////////////KEEP SENDING DV TO ALL NEIGHBOURS AFTER EVERY 5 seconds ///////////////////////////////////////////////////////////////////////////
void sendDV (void )
{

//send dv to neighbour 
char sendN[50] = "CONTROL:["; // A ]: attach char according to need
char append[7] = { '\0' } ;
char buffer [10] ={'\0'};
int i;

char router = notochar(selfno);
append[0]=router ; append[1]='\0';
strcat(sendN,append);
append[0]=']'; append[1]=':'; append[2]='\0'; 
strcat(sendN,append); //CONTROL:[SRC]:
for(i=0 ; i<7; i++)
{
itoa(selfdv[selfno][i] , buffer , 10);
//if u want to add comma, use strlen then individually add comma
strcpy(append, buffer) ;
int addcomma;
int addcoma = strlen(append);
append[addcoma] = ',';
append[addcoma + 1] = '\0';

strcat(sendN,append);

}//sendN finally is for router A -->CONTROL:[A]:0,3,99,99,1,99,99,
int addcoma = strlen(sendN);
sendN[addcoma-1] = ':';     //ending sendN with :
sendN[addcoma] = '\0'; 
// sendN now contains DV in proper format and is ready to be sent to all its neighbours -> CONTROL:[A]:0,3,99,99,1,99,99:

for(i=0; i<7 ; i++)
{
if (livemates[i] == 1)
{
//send to socket to destination router ( notochar i )
struct sockaddr_in other2_details;
other2_details.sin_family = AF_INET ;
other2_details.sin_addr.s_addr = inet_addr("127.0.0.1");
other2_details.sin_port = htons(portno(notochar(i)));

if (  sendto(self_socket , sendN , sizeof(sendN) , 0 , (struct sockaddr*)&other2_details ,sizeof(other2_details)) <= 0)
printf("\nError encountered while sending message");
else
printf("\nMessage sent to router %c by router %c\n\n",notochar(i),notochar(selfno));

}
}

}


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

int main(int argc , char**argv)
{


//Initialize DV to infinite to every other node in starting
in();

//argv[1] contains - on which router will this code run??

char routerName = argv[1][0];
printf("\nRouterName is %c\n\n",routerName);

//selfno 0 for A, 1 for B, 2 for C, 3 for D, 4 for E, 5 for F, 6 for G
selfno = chartono(routerName);

//Initializing Dv for neighbours///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
Initialize_Function( routerName ) ;


//############################################ INITIALIAZATION IS COMPLETE , BELOW BASIC CODE FOR ANY ROUTER WILL RUN ########################################################################

//############################################Creating OUR SOCKET, which will be used to recieve as well as send UDP CONTROL AND DATA packets


self_socket = socket(AF_INET,SOCK_DGRAM,IPPROTO_UDP);
self_details.sin_family = AF_INET ;
self_details.sin_addr.s_addr = inet_addr("127.0.0.1");
self_details.sin_port = htons(portno(notochar(selfno)));


//printf("\nUDP socket created with port number %d",ntohs(self_details.sin_port));

if ( bind (self_socket , (struct sockaddr*)&self_details , sizeof(self_details)) == -1)
{
printf("\nerror binding socket-port");
exit(1);
}
/*else 
{
printf("\nSocket binded to port successfully\n");
}   */


//############################### CREATE AND MAINTAIN ROUTING TABLE FOR NEIGHBOURS according to above selfdv 

set_routing_table();

//############################### SEND DV TO ALL OF ITS NEIGHBOURS

sendDV();


//#################################CREATING A THREAD, WHICH WILL SEND DV'S TO NEIGHBOUR EVERY 5 SECONDS
pthread_t thr;

int rc = pthread_create(&thr,NULL,sendDV_5sec,NULL); 
		
if (rc) { 
	printf("ERROR return code from pthread_create(): %d\n",rc);  
	} 

	
//##################################CREATING INFINITE LOOP WHICH WILL KEEP RECIEVING

pthread_t keeprecvthread;

rc = pthread_create(&keeprecvthread , NULL,(void*) keeprecv , NULL) ;

if (rc) { 
	printf("ERRORmaking keep recv thread: %d\n",rc);  
	} 

//##################################################################################################
pthread_t keepALIVEthread;

rc = pthread_create(&keepALIVEthread , NULL,(void*) keepALIVE , NULL) ;

if (rc) { 
	printf("ERRORmaking keep ALIVE thread: %d\n",rc);  
	} 
//#################################################################################################


while(1)
{


}

}

	







//Routing table (routing-outputA.txt - printing 1.Time 2.Old Table 3.DV from Node 4.New Updated table + any packet through this router.






////////////////////////////////////////////////// KEEP RECIEVING FUNCTION - A thread is passed into this function which keeps recieveing data from socket////////////////////


void *keeprecv (void )
{
//didnt used this because socket created below cannot be seen here by this function



while (1)
{

int recvsize = recvfrom(self_socket , recvmessage , sizeof(recvmessage) , 0 ,(struct sockaddr*)&other_details , sizeof(other_details) );

printf("\n\nRecieved packet\n%s\n",recvmessage);

char camefrom;
int hisdv[7] ;
int f=0;
//BUFFER recvmessage now contains the recieved data packet

if (recvmessage[0] == 'C' &&recvmessage[1] == 'O' &&recvmessage[2] == 'N' &&recvmessage[3] == 'T' &&recvmessage[4] == 'R' &&recvmessage[5] == 'O' &&recvmessage[6] == 'L' )
{
// EXTRACT INFO
camefrom = recvmessage[9];   //becasue format was CONTROL:[A]:12,12,12,12,12,12,12:


char number[3];

//dv starts from recvmessage[10] onwards
int i=12;
while( recvmessage[i] != ':'){
int n=0;


do{
number[n] = recvmessage[i];
n++; i++;

if (recvmessage[i] == ':')
break;

} while (recvmessage[i] != ',');

number[n] = '\0' ;
hisdv[f] = myatoi (number);
f++; 

if (recvmessage[i] == ':')
break;

i++;

/*int n=0;
number[n] = recvmessage[i];
i++; n++;
if (recvmessage[i] == ',')
{
hisdv[f] = myatoi (number);
f++;i++;
continue;
}
number[n] = recvmessage[i];
i++;n++;
if (recvmessage[i] == ',')
{
hisdv[f] = myatoi (number);
f++;i++;
continue;
}
*/

}

//print extract info
printf("\nExtracting info from this packet\n");
printf("Packet type CONTROL\n");
printf("Came from router ->%c\n",camefrom);

for (f=0; f<7 ;f++)
printf("%d-",hisdv[f]);

printf("\n\n");


//UPDATE neighbour dv

for (f=0; f<7 ;f++)
{
selfdv[chartono(camefrom)][f] = hisdv[f];
}

//BELLMAN AND update routing table ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
for (f=0; f<7 ;f++)
{

if( selfdv[chartono(camefrom)][f] + selfdv[selfno][chartono(camefrom)] < selfdv[selfno][f] && (dead[f] == 0) )
{

selfdv[selfno][f] =  selfdv[chartono(camefrom)][f] + selfdv[selfno][chartono(camefrom)];

//Print routing table BEFORE CHANGE

//Change the routing table
printf("\nRouting table before\n");
print_rt();

routingtable[f].destination = notochar(f) ; //destination, cost , camefrom 
routingtable[f].cost = selfdv[selfno][f] ;
routingtable[f].nexthop = camefrom ;
//Print routing table after change
printf("\nRouting table after\n");
print_rt();


//Update change flag to send dv to all neighbours
changeflag = 1;
}


}

//Belman for loop ends~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

if (changeflag == 1)
{
//senddv to all neighbourss
sendDV();
//REMOVE THE FLAG SOLDIER, so we can use the flag again
changeflag = 0;
}



}

else if (recvmessage[0] == 'D' &&recvmessage[1] == 'A' &&recvmessage[2] == 'T' &&recvmessage[3] == 'A'  )
{
// FORMAT WILL BE DATA:[SRC]:[DEST]:STRING: 
camefrom = recvmessage[6] ;

char final_dest = recvmessage[10] ;
char nexthop;
if (final_dest == notochar(selfno))
{
printf("\nDATA packet recieved at Router -> %c\n",notochar(selfno));
printf("Packet was generated by Router -> %c\nand was meant to be sent to this Router!\nPACKET RECIEVED!!",camefrom);
continue; //continues top most while loop in this function
}

//index 13 onward is our DATA string

printf("\nDATA packet recieved at Router -> %c\n",notochar(selfno));
printf("Packet was generated by Router -> %c\n",camefrom);

if( routingtable[chartono(final_dest)].cost < INFINITE )
{
nexthop = routingtable[chartono(final_dest)].nexthop;
}
else if (routingtable[chartono(final_dest)].cost == INFINITE)
{
printf("\nThis packet's destination is uncreachabale by this router,sorry,dropping this packet\n");
continue;
}

printf("This packet's final destination is Router -> %c\nSending this packet to nextr hop %c ",final_dest,nexthop);

//Sending to next hop from routing table
other_details.sin_family = AF_INET ;
other_details.sin_addr.s_addr = inet_addr("127.0.0.1");
other_details.sin_port = htons(portno(nexthop));


if (  sendto(self_socket , recvmessage , sizeof(recvmessage) , 0 , (struct sockaddr*)&other_details ,sizeof(other_details)) <= 0)
printf("\nError encountered while sending message");
else
printf("\nDATA packet forwarded to router %c by router %c\n\n",nexthop,notochar(selfno));

}

else if (recvmessage[0] == 'D' &&recvmessage[1] == 'E' &&recvmessage[2] == 'A' &&recvmessage[3] == 'D')
{
//Restarting router on recieving any dead message
// Restarting own-self this very Router

char dead_router_buff[2];
dead_router_buff[0] = recvmessage[4];
dead_router_buff[1] = '\0';
int t0 = myatoi (dead_router_buff);

//Set dead flag to keep it dead forever, even after 2-3 router's die (ohooooo mar gaye sab) , so we can keep track of dead routers even after initilization from scratch
dead[t0] = 1 ;
int j;
//Restat with same initialization EXCEPT THE DEAD ROUTER

in();                                            // resets all selfdv to INFINITE
Initialize_Function ( notochar(selfno)) ;       // Initializes selfdv with neighbours' and livemates

for(j=0 ; j<7 ; j++)
{
        if (dead[j] == 1 ) 
        {
        livemates[j] = 0;
        selfdv[selfno][j] = INFINITE;
        }
 }  
  for( j=0 ; j<7;j++)
        {
 routingtable[j].destination = '\0' ;
 routingtable[j].cost= INFINITE;
 routingtable[j].nexthop = '\0' ;
 }
set_routing_table();            // This initializes routing table according to given initialization + (now the router would be dead)
sendDV();                       //send DV to neighbours

}
else if (recvmessage[0] == 'A' &&recvmessage[1] == 'L' &&recvmessage[2] == 'I' &&recvmessage[3] == 'V' &&recvmessage[4] == 'E')
{
//
char alive_buff[2];
alive_buff[0] = recvmessage[5] ; alive_buff[1]='\0' ;
int to = myatoi (alive_buff) ;


//Make it alive so it can be used in bellman algorithm
dead[to] = 0 ;
printf("\nALIVE%d signal recieved by %c\n%c considers router%d as alive node for Bellman\n",to,notochar(selfno),notochar(selfno),to);

} 

}



}



