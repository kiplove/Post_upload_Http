#ifndef _ERR_CODE_H
#define _ERR_CODE_H
#define SERVER_STRING "Server: jdbhttpd/0.1.0\r\n"
//输出错误字符串
void error_die(const char *sc)
{
    perror(sc);
    exit(1);
}
//定义http错误 500
void cannot_execute(int client)
{
    char buf[1024];
    sprintf(buf,"HTTP/1.0 500 Internal Server Error\r\n");
    send(client,buf,strlen(buf),0);
    sprintf(buf,"Content-type:text/html\r\n");
    send(client,buf,strlen(buf),0);
    sprintf(buf,"\r\n");
    send(client,buf,strlen(buf),0);
    sprintf(buf,"<P>Error prohibited CGI execution.\r\n");
    send(client,buf,strlen(buf),0);
}
//定义http400 错误
void bad_request(int client)
{
    char buf[1024];
    sprintf(buf,"HTTP/1.0 400 BAD REQUEST\r\n");
    send(client,buf,strlen(buf),0);
    sprintf(buf,"Content-type:text/html\r\n");
    send(client,buf,strlen(buf),0);
    sprintf(buf,"\r\n");
    send(client,buf,strlen(buf),0);
    sprintf(buf,"<P>Your browser sent a bad request,");
    send(client,buf,strlen(buf),0);
    sprintf(buf,"such as a POST without a Content-Length.\r\n");
    send(client,buf,sizeof(buf),0);
}
//定义http404 错误
void not_found(int client)
{
    char buf[1024];
    sprintf(buf,"HTTP/1.0 404 NOT FOUND\r\n");
    send(client,buf,strlen(buf),0);
    sprintf(buf,SERVER_STRING);
    send(client,buf,strlen(buf),0);
    sprintf(buf,"Content_Type:text/html\r\n");
    send(client,buf,strlen(buf),0);
    sprintf(buf,"\r\n");
    send(client,buf,strlen(buf),0);
    sprintf(buf,"<HTML><HEAD><TITLE>Not FOUND</TITLE>\r\n");
    send(client,buf,strlen(buf),0);
    sprintf(buf,"<BODY><P>The server could not fulfill\r\n");
    send(client,buf,strlen(buf),0);
    sprintf(buf,"your request because the resource specified\r\n");
    send(client,buf,strlen(buf),0);
    sprintf(buf,"is unavailable or nonexistent.\r\n");
    send(client,buf,strlen(buf),0);
    sprintf(buf,"</BODY></HTML>\r\n");
    send(client,buf,strlen(buf),0);
}
//定义http501 错误
void unimplemented(int client)
{
    char buf[1024];
    sprintf(buf,"HTTP/1.0 501 Method Not Implemented\r\n");
    send(client,buf,strlen(buf),0);
    sprintf(buf,SERVER_STRING);
    send(client,buf,strlen(buf),0);
    sprintf(buf,"Content_Type:text/html\r\n");
    send(client,buf,strlen(buf),0);
    sprintf(buf,"\r\n");
    send(client,buf,strlen(buf),0);
    sprintf(buf,"<HTML><HEAD><TITLE>Method Not Implemented\r\n");
    send(client,buf,strlen(buf),0);
    sprintf(buf,"</TITLE></HEAD>\r\n");
    send(client,buf,strlen(buf),0);
    sprintf(buf,"<BODY><P>HTTP request method not supported.\r\n");
    send(client,buf,strlen(buf),0);
    sprintf(buf,"</BODY></HTML>\r\n");
    send(client,buf,strlen(buf),0);
}
#endif
