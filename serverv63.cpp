#include<iostream>
#include<sys/types.h>
#include<sys/socket.h>
#include<stdlib.h>
#include<arpa/inet.h>
#include<string.h>
#include<unistd.h>
#include<netdb.h>
#include<map>
#include<list>
#include <utility> 
#include<random>
#include<string>
#include<sstream>

#define MAX 50
#define MAX_PEERS 2

#define GETADDR "getaddr"
#define GETPEER "getpeer"
#define POLL "poll"
#define GETCON "getcon"
#define END "end"
#define TEST "t"

using namespace std;

string separator = "_";
string sockaddrToString(sockaddr_storage sock)
{
  char str[50];
  int port;
  char numeric_addr[INET6_ADDRSTRLEN];
  if(sock.ss_family == AF_INET)
    {
       inet_ntop(sock.ss_family,&(((struct sockaddr_in*)&sock)->sin_addr),numeric_addr,sizeof numeric_addr);
      port = ntohs(((struct sockaddr_in*)&sock)->sin_port);
    }
  else if(sock.ss_family == AF_INET6)
    {
      inet_ntop(sock.ss_family,&(((struct sockaddr_in6*)&sock)->sin6_addr),numeric_addr,sizeof numeric_addr);
      port = ntohs(((struct sockaddr_in6*)&sock)->sin6_port);
    }
  sprintf(str,"%s:%d",numeric_addr,port);
  string s(str);
  return(s);
}

void split(string msg, string* msg1, string* msg2, char c)
{
  stringstream ss(msg);
  getline(ss,*msg1,c);
  getline(ss,*msg2,c);
}

unsigned int peersn=0;
unsigned int peerstop=0;
string peers[MAX_PEERS];
map <string , list<string> > duties;

void perror(string s)
{
  cout<<s<<endl;
  exit(0);
}

void addtopeers(string s)
{
  for(int i=0;i<peersn;++i)
    if(peers[i]==s)
      return;
  peers[peerstop]=s;
  peerstop=(peerstop+1)%MAX_PEERS;
  if(peersn<MAX_PEERS)
    peersn++;
}

void getaddr(int sockfd , sockaddr_storage clien_addr)
{
  int n;
  string s;
  s=sockaddrToString(clien_addr);
  if((n=sendto(sockfd,s.c_str(),s.length(),0,(sockaddr *)&clien_addr,sizeof(clien_addr)))<0)
    perror("sending error");
}

void getpeer(int sockfd , sockaddr_storage clien_addr)
{
  char buffer[MAX];
  int n;
  string s, s1;
  bzero(buffer,MAX);
  s=sockaddrToString(clien_addr);
  if(peersn>1||peers[0]!=s)
    {
      int i=rand()%peersn;
      if(peers[i]==s)
	i=(i+1)%peersn;
      s1 = peers[i];
    }
  else
    s1 = END;
  if((n=sendto(sockfd,s1.c_str(),s1.length(),0,(sockaddr *)&clien_addr,sizeof(clien_addr)))<0)
    perror("sending error");
  addtopeers(s);
}
void poll(int sockfd, sockaddr_storage clien_addr)
{
  char buffer[MAX];
  int n;
  string s, s1="";
  
  s=sockaddrToString(clien_addr);
  map <string, list<string> > :: iterator it1;
  it1 = duties.find(s);
  
  if(it1 != duties.end())
    {
      for(list< string > ::iterator it2 = (it1->second).begin();it2!=(it1->second).end();++it2)
	{
	  s1 += *it2;
	  s1 += separator;
     	}
      duties.erase(it1);
    }
  s1 += END;
  if((n=sendto(sockfd,s1.c_str(),s1.length(),0,(sockaddr *)&clien_addr,sizeof(clien_addr)))<0)
    perror("sending error");
}

void getcon(int sockfd, sockaddr_storage clien_addr, string addr)
{
  char buffer[MAX];
  int n;
  string s;
  s=sockaddrToString(clien_addr);
  duties[addr].push_back(s);
  strcpy(buffer,END);
  if((n=sendto(sockfd,buffer,strlen(buffer),0,(sockaddr *)&clien_addr,sizeof(clien_addr)))<0)
    perror("sending error");  
}

int main(int argc,char* argv[])
{
  char c = separator[0];
  int sockfd;
  int port;
  int n;
  socklen_t clielen;
  sockaddr_in serv_addr;
  sockaddr_storage clien_addr;
  char msg[MAX];
  string msg1;
  string msg2;
  if(argc<2)
    {
      cout<<"Usage : ./name port"<<endl;
      exit(0);
    }
  port=atoi(argv[1]);
  if((sockfd=socket(AF_INET,SOCK_DGRAM,0))<0)
    perror("error in socket creation");
  bzero((char*)&serv_addr,sizeof(serv_addr));
  serv_addr.sin_family=AF_INET;
  serv_addr.sin_port=htons(port);
  serv_addr.sin_addr.s_addr=INADDR_ANY;
  clielen = sizeof clien_addr;
  if(bind(sockfd,(sockaddr *) &serv_addr,sizeof(serv_addr))<0)
    perror("bining error");
  while(1)
    {
      bzero((char*)&clien_addr,sizeof(clien_addr));
      if((n=recvfrom(sockfd,msg,sizeof(msg),0,(struct sockaddr *)&clien_addr,&clielen))<0)
	perror("receiving error");
      msg[n]='\0';
      split(string(msg),&msg1,&msg2,c);
      if(msg1==GETADDR)
	getaddr(sockfd,clien_addr);
      if(msg1==GETPEER)
	getpeer(sockfd,clien_addr);
      else if(msg1==POLL)
	poll(sockfd,clien_addr);
      else if(msg1==GETCON)
	getcon(sockfd,clien_addr,msg2);
      else
	cout<<msg1<<"\n";
    }
  close(sockfd);
  return 0;
}
