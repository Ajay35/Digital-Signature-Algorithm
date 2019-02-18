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
#include <thread>
#include <openssl/sha.h>
#include "util.hpp"
#define MAX_SIZE 80
#define MAX_LEN 1024

#define ll long long
#define SERVER_PORT 8080
#define QUEUE_SIZE 5
using namespace std;

int start_server_connect();
void server_loop(int);
void comm_with_client(int);
ll hexadecimalToDecimal(unsigned char *,ll); 

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

int start_server()
{
   int sfd; 
   struct sockaddr_in saddr;
   int status;

   sfd = socket(AF_INET, SOCK_STREAM, 0);
   if (sfd == -1) {
      fprintf(stderr, "Server error: unable to get socket descriptor\n");
      exit(1);
   }

   saddr.sin_family = AF_INET; 
   saddr.sin_port = htons(SERVER_PORT);  
   saddr.sin_addr.s_addr = INADDR_ANY;
   bzero(&(saddr.sin_zero),8);

   status = bind(sfd, (struct sockaddr *)&saddr, sizeof(struct sockaddr));
   if(status == -1){
      fprintf(stderr,"Server error: unable to bind to port %d\n", SERVER_PORT);
      exit(2);
   }

   status =listen(sfd,QUEUE_SIZE);
   if (status == -1) {
      fprintf(stderr, "Server error: unable to listen\n");
      exit(3);
   }

   fprintf(stderr, "Server successfully started, listening to port %hd\n", SERVER_PORT);
   return sfd;
}



void server_loop(int server_fd){
  char cs_buf[MAX_LEN];
  char sc_buf[MAX_LEN];
  int cfd;             
  struct sockaddr_in caddr;
  socklen_t size;
  
  
  while(true){
    cfd = accept(server_fd,(struct sockaddr *)&caddr,&size);
    if (cfd == -1){
      fprintf(stderr, "Server error: unable to accept request\n");
      continue;
    }
    cout<<"Connected with "<<inet_ntoa(caddr.sin_addr)<<endl;    
    std::thread t(comm_with_client,cfd);
    t.detach();
  }
}

void comm_with_client(int cfd){
  int nbytes;
  while(true){
    
    Msg send_msg,recv_msg;

    nbytes=recv(cfd,&recv_msg,sizeof(Msg), 0);
     if (nbytes == -1) {
       fprintf(stderr, "Server error: unable to receive\n");
       exit(1);
     }
     ll q=recv_msg.q;
     ll g=recv_msg.g;
     ll y1=recv_msg.y1;
     ll y2=recv_msg.y2;

     cout<<"received:"<<g<< " "<<y1<<" "<<y2<<endl; 

     nbytes=recv(cfd,&recv_msg,sizeof(Msg), 0);
     if (nbytes == -1) {
       fprintf(stderr, "Server error: unable to receive\n");
       exit(1);
     }
      cout<<"hash:"<<recv_msg.sign.c<<endl;
      cout<<"signature:"<<recv_msg.sign.s<<endl;
      cout<<"plaintext:"<<recv_msg.plaintext<<endl;
      

      ll c=hexadecimalToDecimal(recv_msg.sign.c,q);
      
      
      ll temp1=power(g,recv_msg.sign.s,q);
      ll temp1_y=modInverse(y1,q);
      ll temp13=power(temp1_y,c,q);
      ll A_d=(temp1*temp13);

      ll temp2=power(y1,recv_msg.sign.s,q);
      ll temp2_y=modInverse(y2,q);
      ll temp23=power(temp2_y,c,q);
      ll B_d=(temp2*temp23);

      string A_d_str=to_string(A_d);
      string B_d_str=to_string(B_d);
      string m(recv_msg.plaintext);
      string h=A_d_str+B_d_str+m;

      unsigned char hash[SHA_DIGEST_LENGTH];
      SHA1((const unsigned char *)h.c_str(), sizeof(h) - 1, hash);

      cout<<"received hash:"<<recv_msg.sign.c<<endl;
      cout<<"calculated hash:"<<hash<<endl;

      if(recv_msg.sign.c==hash)
          cout<<"matched"<<endl;
      else
          cout<<"failed to match"<<endl;
      break;

  }

  
}

int main(){
  int server_fd;
  server_fd = start_server();
  server_loop(server_fd);
  close(server_fd);
  return 0;
}



