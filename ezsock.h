#ifndef EZSOCK_H
#define EZSOCK_H
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
extern int EZSOCK_BACK_LOG;
extern int RECV_FLAGS;
extern int SEND_FLAGS;

typedef struct{
  int fd;
  struct sockaddr_in addr;
}ezsock_listener;

typedef struct{
  int fd;
  struct sockaddr_storage pinfo;
  char closed; 
}ezsock_conn;

ezsock_listener eztcp_create_listener_tcp(const char * address,short port, int flags); 

ezsock_conn ez_tcp_accecpt(ezsock_listener *sck);

ezsock_conn dial_tcp(const char* host, short port,int flags);

int ezsock_conn_write(char *buffer,int buf_size,ezsock_conn *conn);

int ezsock_conn_read(char *buffer,int buf_size,ezsock_conn *conn);

int ezsock_close(ezsock_conn *sck,unsigned int obj_len);

#endif
