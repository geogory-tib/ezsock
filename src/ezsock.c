#include <sys/socket.h>
#include <sys/types.h>
#include <string.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
int EZSOCK_BACK_LOG = 10;
int RECV_FLAGS = 0;
int SEND_FLAGS = 0;
#define CONN_CLOSED -128
#define DRAIN_FAILURE -22
#define PANIC(msg)\
  printf("%s: %s",__func__,msg); \
  exit(EXIT_FAILURE);
static const int opt = 1;
typedef struct{
  int fd;
  struct sockaddr_in addr;
}ezsock_listener;

typedef struct{
  int fd;
  struct sockaddr_storage pinfo;
  char closed; 
}ezsock_conn;

ezsock_listener eztcp_create_listener_tcp(const char * address,short port, int flags){
  ezsock_listener ret;
  struct sockaddr_in addr;
  int code = 0;
  int socket_fd =  socket(AF_INET,SOCK_STREAM,0);
  code = setsockopt(socket_fd,SOL_SOCKET,flags,&opt,sizeof(int));
  if(code < 0){
	perror("Error Setting Socket Options");
	exit(EXIT_FAILURE);
  }
  memset(&addr,0,sizeof(addr));
  addr.sin_family = AF_INET;
  addr.sin_port = htons(port);
  code = inet_aton(address,&addr.sin_addr);
  if(code == 0){
	PANIC("Invaild Address");
  }
  code = bind(socket_fd,(struct sockaddr*)&addr,sizeof(addr));
  if (code < 0){
	perror("Error Binding Socket");
	exit(EXIT_FAILURE);
  }
  ret.fd = socket_fd;
  ret.addr = addr ;
  code = listen(ret.fd,EZSOCK_BACK_LOG);
  if(code < 0){
	perror("ez_tcp_listen");
	exit(EXIT_FAILURE);
  }
  return ret;
}

ezsock_conn ez_tcp_accecpt(ezsock_listener *sck){
  ezsock_conn ret;
  memset(&ret,0,sizeof(ezsock_conn));
  unsigned int len = sizeof(sck->addr);
  ret.fd = accept(sck->fd,(struct sockaddr*)&sck->addr,&len);
  if(ret.fd < 0){
	perror("ez_tcp_accecpt");
	return ret;
  }
  unsigned int addr_len = sizeof(ret.pinfo);
  int code = getpeername(ret.fd,(struct sockaddr*)&ret.pinfo,&addr_len);
  if(code  < 0){
	perror("ez_tcp_accecpt-geetpeername");
  }
  return ret;
}

ezsock_conn dial_tcp(const char* host, short port,int flags){
  ezsock_conn ret;
  memset(&ret.pinfo,0, sizeof(ret.pinfo));
  ret.fd = socket(AF_INET,SOCK_STREAM,0);
  if(ret.fd < 0){
	return ret;
  }

  int code = setsockopt(ret.fd,SOL_SOCKET,flags,&opt,sizeof(int));
  if(code < 0){
	perror("Error Setting Socket Options");
	exit(EXIT_FAILURE);
  }
  
  struct sockaddr_in *casted_addr = (struct sockaddr_in*)&ret.pinfo;
  casted_addr->sin_family = AF_INET;
  casted_addr->sin_port = htons(port);
  code = inet_aton(host,&casted_addr->sin_addr);
  if(code < 0){
	PANIC("INVALID ADDRESS");
  }
  code = connect(ret.fd, (struct sockaddr *)casted_addr,sizeof(*casted_addr));
  if(code < 0){
	ret.closed = code;
	return ret;
  }
  return ret;
}

int ezsock_conn_read(char *buffer,int buf_size,ezsock_conn *conn){
  if(conn->closed == CONN_CLOSED){
	return CONN_CLOSED;
  }
  int read_len = recv(conn->fd,buffer,buf_size,RECV_FLAGS);
  if(read_len < 0){
	perror("eztcp_conn_read");
  }
  return read_len; 
}

int ezsock_conn_write(char *buffer,int buf_size,ezsock_conn *conn){
  if(conn->closed == CONN_CLOSED){
	return CONN_CLOSED;
  }
  int read_len = send(conn->fd,buffer,buf_size,SEND_FLAGS);
  if(read_len < 0){
	perror("eztcp_conn_read");
  }
  return read_len; 
}


int ezsock_close(ezsock_conn *sck,unsigned int obj_len){
  if(obj_len == sizeof(ezsock_listener)){
	close(sck->fd);
	return 0;
  }
  int code = shutdown(sck->fd,SHUT_WR);
  if(code < 0){
	return code;
  }
  int n;
  char tempbuffer[256];
  for(;;){
	n = read(sck->fd,tempbuffer,sizeof(tempbuffer));
	if(n < 0){
	  perror("ezsock_close-read");
	  close(sck->fd);
	  return DRAIN_FAILURE;
	}
	if(n == 0){
	  break;
	}
  }
  close(sck->fd);
  if(obj_len == sizeof(ezsock_conn)){
	sck->closed = CONN_CLOSED;
  }
  return 0;
}


