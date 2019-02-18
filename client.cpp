#include<bits/stdc++.h>
#include <bits/stdc++.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <string>
#include <unistd.h>
#include <arpa/inet.h>
#include <time.h>
#include <limits>
#include <math.h>
#include <random>
#include "util.hpp"
#include <openssl/sha.h>
#include <openssl/md5.h>

#define MAX_SIZE 80
#define MAX_LEN 1024
#define ll long long
#define DEFAULT_SERVER "127.0.0.1"
#define SERVICE_PORT 8080
using namespace std;

int server_connect(char *);
void communicate_with_server (int,ll);


typedef struct {
int opcode; 
int s_addr; 
int d_addr; 
} Hdr;

typedef struct {
  unsigned char c[SHA_DIGEST_LENGTH]; 
  ll s; 
} Signature;

typedef struct {
Hdr hdr; 
long q; 
long g; 
long y1, y2; 
char plaintext[MAX_LEN];
Signature sign; 
int ver_status;
int dummy;
} Msg;



ll hexadecimalToDecimal(unsigned char hexVal[],ll mod) 
{    
    int len = 20; 
    ll base = 1; 
      
    ll dec_val = 0; 
      
    for (int i=len-1; i>=0; i--){    
       
        if (hexVal[i]>='0' && hexVal[i]<='9'){ 
            dec_val += ((hexVal[i] - 48)*base)%mod;
            base =( base * 16)%mod; 
        }
        else if (hexVal[i]>='a' && hexVal[i]<='f'){ 
            dec_val += ((hexVal[i]-87)*base)%mod; 
            base = (base*16)%mod;
        } 
    } 
      
    return dec_val; 
} 


int server_connect(char *sip)
{
   int client_fd;
   struct sockaddr_in saddr;
   int status;

   
   client_fd=socket(AF_INET,SOCK_STREAM,0);
   if(client_fd==-1){
      fprintf (stderr, "*** Client error: unable to get socket descriptor\n");
      exit(1);
   }

   saddr.sin_family = AF_INET; 
   saddr.sin_port = htons(SERVICE_PORT);
   saddr.sin_addr.s_addr = inet_addr(sip);
   bzero(&(saddr.sin_zero),8);
   status = connect(client_fd,(struct sockaddr *)&saddr, sizeof(struct sockaddr));
   if (status == -1) {
      fprintf(stderr, "*** Client error: unable to connect to server\n");
      exit(1);
   }
   fprintf(stderr, "Connected to server\n");
   return client_fd;
}

void communicate_with_server(int cfd){

   char buffer[MAX_LEN];
   Msg send_msg;
   string m="110101010";
  /* Key generation phase */
   
    ll q=get_prime();
    cout<<"Prime number:"<<q<<endl;
    ll g=get_primitive_root(q);
    ll a=rand()%(q-1)+1;
    ll y1=power(g,a,q);
    ll y2=power(y1,a,q);
    cout<<"Public key:"<<"{"<<g<<", "<<y1<<", "<<y2<<"}"<<endl;

    
    /* Publish Public keys  (sending to server)*/
    Msg send_msg1;
    send_msg1.hdr.opcode=10;
    send_msg1.y1=y1;
    send_msg1.y2=y2;
    send_msg1.q=q;
    send_msg1.g=g;
    int n=send(cfd,&send_msg1,sizeof(send_msg1),0);
    if(n<=0){
      cout<<"Sending failed"<<endl;
      exit(1);
    }

  /* Signature Generation phase */

    ll r=rand()%(q-1)+1;
    ll A=power(g,r,q);
    ll B=power(y1,r,q);
    
    
    send_msg.hdr.opcode=20;


    string A_str=to_string(A);
    string B_str=to_string(B);
    
    string h=A_str+B_str+m;


    unsigned char hash[SHA_DIGEST_LENGTH];
    SHA1((const unsigned char *)h.c_str(), sizeof(h) - 1, hash);
    ll c=hexadecimalToDecimal(hash,q);

    ll s =((a*c)+r )%(q-1);
    Signature sign;
    sign.s=s;
    memcpy(sign.c,hash, sizeof sign.c);
    send_msg.sign=sign;
    strcpy(send_msg.plaintext,m.c_str());


    n=send(cfd,&send_msg,sizeof(send_msg),0);
    if(n<=0){
      cout<<"signature sending failed"<<endl;
      exit(1);
    }
  /* status verified signature result  */
}

int main(int argc, char* argv[]){

  char cs_buf[MAX_LEN];
  char sc_buf[MAX_LEN];
  char server_ip[16];
  int client_fd;  
  
  strcpy(server_ip,(argc==2)?argv[1]:DEFAULT_SERVER);
  client_fd=server_connect(server_ip);
  communicate_with_server(client_fd);
  close(client_fd);
  return 0;
}
