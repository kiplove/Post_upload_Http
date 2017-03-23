#ifndef _HTTPED_H
#define _HTTPED_H

#include <stdio.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <ctype.h>
#include <strings.h>
#include <string.h>
#include <sys/stat.h>
#include <pthread.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <sys/epoll.h>
#include <fcntl.h>
#include <errno.h>
#define ISspace(x) isspace((int)(x))
#define STDIN  0
#define STDOUT 1
#define STDERR 2
//读取一行
int get_line(int,char *,int);
void accept_request(void *);   
void execute_cgi(int,const char *,const char *,const char *); 
void serve_file(int,const char *); 
void headers(int,const char *); 
void cat(int,FILE *); 
int startup(u_short *); 

#endif
