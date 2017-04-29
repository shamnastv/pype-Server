#include<iostream>
#include<sys/types.h>
#include<sys/socket.h>
#include<stdlib.h>
#include<arpa/inet.h>
#include<string.h>
#include<unistd.h>
#include<netdb.h>
#include<semaphore.h>
#include<pthread.h>
#include<signal.h>
#include<map>
#include<list>
#include <utility> 
#include<random>
#include<string>

#define MAX 50
#define MAX_PEERS 2

#define INIT "a"
#define POLL "b"
#define GETCON "c"
#define GETMYADDR "d"
#define NOTHING "e"
#define NEXT "f"
#define TEST "t"
using namespace std;

struct Threadargs{
  int sockfd;
  sockaddr_in addr;
};
typedef struct Threadargs Targs;

struct Netaddress{
  char ip[INET_ADDRSTRLEN];
  int port;
};
typedef struct Netaddress Netaddr;

string sockaddrToString(sockaddr_in sock)
{
  char str[22];
  sprintf(str,"%s:%d",inet_ntoa(sock.sin_addr),ntohs(sock.sin_port));
  string s(str);
  return(s);
}

int peersn=0;
int peerstop=0;
string peers[MAX_PEERS];
map <string , list<string> > duties;

//list < pair<sockaddr_in,list<sockaddr_in> > > duties;

void perror(string s)
{
  cout<<s<<endl;
  exit(0);
}

void init(Targs* targs)
{
  int sockfd=targs->sockfd;
  char buffer[MAX];
  int n;
  string s;
  bzero(buffer,MAX);
  s=sockaddrToString(targs->addr);
  if((n=write(sockfd,s.c_str(),s.length()))<0)
    perror("error while writing");
  if((n=read(sockfd,buffer,MAX))<0)
    perror("error while reading");
  buffer[n]='\0';
  if(strcmp(buffer,NEXT)==0)
    {
      if(peersn>0)
	{
	  int i=rand()%peersn;
	  duties[peers[i]].push_back(s);
	  if((n=write(sockfd,peers[i].c_str(),peers[i].length()))<0)
	    perror("error while writing");
	}
      else
	{
	  strcpy(buffer,NOTHING);
	  if((n=write(sockfd,buffer,strlen(buffer)))<0)
	    perror("error while writing");
	}
    }
  else
    {
      strcpy(buffer,NEXT);
      if((n=write(sockfd,buffer,strlen(buffer)))<0)
	perror("error while writing");
    }
  peers[peerstop]=s;
  peerstop=(peerstop+1)%MAX_PEERS;
  if(peersn<MAX_PEERS)
    peersn++;
}
void poll(Targs* targs)
{
  char buffer[MAX];
  int n;
  string s;
  int sockfd=targs->sockfd;
  
  s=sockaddrToString(targs->addr);
  map <string, list<string> > :: iterator it1;
  it1 = duties.find(s);
  
  if(it1 == duties.end())
    {
      strcpy(buffer,NOTHING);
      if((n=write(sockfd,buffer,strlen(buffer)))<0)
	perror("error while writing");
    }
  else
    {
      for(list< string > ::iterator it2 = (it1->second).begin();it2!=(it1->second).end();++it2)
	{
	  if((n=write(sockfd,it2->c_str(),it2->length()))<0)
	    perror("error while writing");
	  if((n=read(sockfd,buffer,MAX))<0)
	    perror("error while reading");
	  buffer[n]='\0';
	  if(strcmp(buffer,NEXT)!=0)
	    perror("buffer is not NEXT");
     	}
      duties.erase(it1);
      strcpy(buffer,NOTHING);
      if((n=write(sockfd,buffer,strlen(buffer)))<0)
	perror("error while writing");
    }
}

void getcon(Targs* targs)
{
  char buffer[MAX];
  int n;
  string s;
  int sockfd=targs->sockfd;

  s=sockaddrToString(targs->addr);
  strcpy(buffer,NEXT);
  if((n=write(sockfd,buffer,strlen(buffer)))<0)
    perror("error while writing");
  if((n=read(sockfd,buffer,MAX))<0)
    perror("error while reading");
  buffer[n]='\0';
  string addr(buffer);
  duties[buffer].push_back(s);
  strcpy(buffer,NEXT);
  if((n=write(sockfd,buffer,strlen(buffer)))<0)
    perror("error while writing");
}
void noidea(Targs* targs)
{
  char buffer[MAX];
  int n;
}

void *threadmain(void *targ)
{
  Targs *targs=(Targs*) targ;
  pthread_detach(pthread_self());
  int sockfd=targs->sockfd;
  char buffer[MAX];
  int n;
  while(1)
    {
      bzero(buffer,MAX);
      if((n=read(sockfd,buffer,MAX))<0)
	perror("error while reading");
      if(n==0)
	continue;
      buffer[n]='\0';
      if(strcmp(buffer,INIT)==0)
	{
	  cout<<"invoke init"<<endl;
	  init(targs);
	}
      else if(strcmp(buffer,POLL)==0)
	{
	  cout<<"invoke poll"<<endl;
	  poll(targs);
	}
      else if(strcmp(buffer,GETCON)==0)
	{
	  cout<<"invoke getcon"<<endl;
	  getcon(targs);
	}
      else
	{
	  cout<<"start"<<buffer<<"end"<<endl;
	  if((n=write(sockfd,buffer,strlen(buffer)))<0)
	    perror("error while writing");
	}
      //else
      //noidea(targs);
    }
  close(sockfd);
}
int main(int argc,char* argv[])
{
  int sockfd,newsockfd;
  int port;
  socklen_t clielen;
  sockaddr_in serv_addr,clien_addr;
  hostent* server;
  Targs *targs;
  if(argc<2)
    {
      cout<<"Usage : ./name port"<<endl;
      exit(0);
    }
  port=atoi(argv[1]);
  if((sockfd=socket(AF_INET,SOCK_STREAM,0))<0)
    perror("error in socket creation");
  bzero((char*)&serv_addr,sizeof(serv_addr));
  serv_addr.sin_family=AF_INET;
  serv_addr.sin_port=htons(port);
  serv_addr.sin_addr.s_addr=INADDR_ANY;
  if(bind(sockfd,(sockaddr *) &serv_addr,sizeof(serv_addr))<0)
    perror("bining error");
  clielen=sizeof(clien_addr);
  listen(sockfd,5);
  pthread_t tid;
  while(1)
    {
      if((newsockfd=accept(sockfd,(sockaddr *)&clien_addr,&clielen))<0)
	perror("error on accept");
      targs=(Targs *)malloc(sizeof(Targs));
      targs->sockfd=newsockfd;
      targs->addr=(sockaddr_in)clien_addr;
      if(pthread_create(&tid,NULL,threadmain,(void *)targs)!=0)
	perror("error creating thread");
    }
  close(sockfd);
  return 0;
}
