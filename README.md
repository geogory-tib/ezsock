# Ez Socket A simple socket abstraction

This is a super simple library to make working with sockets in c less of a pain
This library only works on POSIX systems so it will not work natively on windows   
I plan on making this more like golang's net package as it's one of the most
ergonomic


### Echo Server Example
```
#include <stdio.h>
#include <stdlib.h>
#include "ezsock.h"
#include <sys/socket.h> // this is needed for the socket flags.
#define ENTRY_MESSAGE "Type anything and I should repeat it!\n\r"
int main(void){
  char buffer[4096];
  ezsock_listener listener = eztcp_create_listener_tcp("127.0.0.1",8080,SO_REUSEADDR);
  ezsock_conn conn = ez_tcp_accecpt(&listener);
  ezsock_conn_write(ENTRY_MESSAGE,sizeof(ENTRY_MESSAGE),&conn);
  for(;;){
	int n = ezsock_conn_read(buffer,sizeof(buffer),&conn);
	n = ezsock_conn_write(buffer,n,&conn);
	if(n == 0){
	  break;
	}
	
  }
  int code = ezsock_close(&conn,sizeof(conn));
  code = ezsock_close((ezsock_conn *)&listener,sizeof(listener));
  if(code < 0){
	perror("something bad happened");
	return EXIT_FAILURE;
  }
  return 0;
}
```
### Echo Server Client Example

```
#include <stdio.h>
#include <stdlib.h>
#include "ezsock.h"
#include <sys/socket.h>
#include <unistd.h>
#include <string.h>
int main(void){
  char buffer[4096];
  memset(buffer,0,sizeof(buffer));
  ezsock_conn conn = dial_tcp("127.0.0.1",8080,SO_REUSEADDR);
  int n =  ezsock_conn_read(buffer,sizeof(buffer),&conn);
  write(1,buffer,n);
  for(;;){
	n = read(0,buffer,sizeof(buffer));
	ezsock_conn_write(buffer,n,&conn);
	n = ezsock_conn_read(buffer,sizeof(buffer),&conn);
	if(n == 0){
	 break;
	}
	write(1,buffer,n);
  }
  ezsock_close(&conn,sizeof(conn));
}
```

## Planned Features

* Unix Domain Socket Support
* UDP Support
* C++ Wrapper
* Buffered Connections

