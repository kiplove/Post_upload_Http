#include "httped.h"
#include "err_code.h"
#define SERVER_STRING "Server: http/1.0\r\n"
//读取一行
int get_line(int sock,char *buf,int size)
{
    int i=0;
    char c='\0';
    int n;
    while((i<size-1)&&(c!='\n'))
    {   
        n=recv(sock,&c,1,0);
        if(n>0)
        {   
            if(c=='\r')
            {   
                n=recv(sock,&c,1,MSG_PEEK);
                if((n>0)&&(c=='\n'))
                    recv(sock,&c,1,0);
                else
                    c='\n';
            }   
            buf[i]=c;
            i++;
        }   
        else
            c='\n';
    }   
    buf[i]='\0';
    return(i);
}
//启动socket,开启监听
int startup(u_short *port)
{	
	int fd=0;
	struct sockaddr_in name;

	fd=socket(PF_INET,SOCK_STREAM,0);
	if(fd==-1)
		error_die("socket");
	
	memset(&name,0,sizeof(name));
	name.sin_family=AF_INET;
	name.sin_port=htons(*port);
	name.sin_addr.s_addr=htonl(INADDR_ANY);

	if(bind(fd,(struct sockaddr *)&name,sizeof(name))<0)
		error_die("bind");
	if(*port==0)
	{
		socklen_t namelen=sizeof(name);
		if(getsockname(fd,(struct sockaddr *)&name,&namelen)==-1)
			error_die("getsockname");
		*port=ntohs(name.sin_port);
	}
	if(listen(fd,5)<0)
		error_die("listen");
	return(fd);
}

//接收请求，分发处理方式
void accept_request(void *arg)
{
	int client=*(int *)arg;
	char buf[1024];
	size_t numchars;
	char method[255];
	char url[255];
	char path[512];
	int cgi=0;
	struct stat st;
	size_t i=0,j=0;

	char *query_string=NULL;
	//从客户端获取请求行，存放在buf区
	numchars=get_line(client,buf,sizeof(buf));

	//获取请求方法
	while(!ISspace(buf[i])&&(i<sizeof(method)-1))
	{
		method[i]=buf[i];
		i++;
	}
	j=i;
	method[i]='\0';
	
	//只处理post与get方法
	if(strcasecmp(method,"GET")&&strcasecmp(method,"POST"))
	{
		unimplemented(client);
		return;
	}
	//post方法，标记cgi处理
	if(strcasecmp(method,"POST")==0)
		cgi=1;
	
	//获取uri
	i=0;
	while(ISspace(buf[j])&&(j<numchars)) //去除请求行中间的空格
		j++;
	while(!ISspace(buf[j])&&(i<sizeof(url)-1)&&(j<numchars))
	{
		url[i]=buf[j];
		i++;j++;
	}	
	url[i]='\0';
	
  	//使用get方法时，获取uri上'?'之后的参数，保存在query_string指针上
	if(strcasecmp(method,"GET")==0)
	{
		query_string=url;
		while((*query_string!='?')&&(*query_string!='\0'))
			query_string++;
		if(*query_string=='?')
		{
			cgi=1;
			*query_string='\0'; //读取目录
			query_string++;     //读取参数
		}
	}
	
	//指定本地存储地址
	sprintf(path,"your address%s",url);
	//默认主页
	if(path[strlen(path)-1]=='/')
		strcat(path,"index.html");

	//访问地址无效，丢弃请求头
	if(stat(path,&st)==-1)
	{
		while((numchars>0)&&strcmp("\n",buf))
			numchars=get_line(client,buf,sizeof(buf));
		not_found(client);
	}
	else
	{
		//访问是一个目录，就添加"/index.html"作为默认地址
		if((st.st_mode & S_IFMT)==S_IFDIR)
			strcat(path,"/base.html");
		//访问可执行文件，则标记cgi
		if((st.st_mode & S_IXUSR)||(st.st_mode & S_IXGRP)||(st.st_mode & S_IXOTH))
			cgi=1;
		if(!cgi)
			serve_file(client,path);
		else
		{	
			//fprintf(stdout,"path=%s\n",*path);
			execute_cgi(client,path,method,query_string);
		}	
	}
	close(client);
}
//静态处理，响应报头
void headers(int client,const char *filename)
{
    char buf[1024];
    (void)filename;

    strcpy(buf,"HTTP/1.0 200 OK\r\n");
    send(client,buf,strlen(buf),0);
    strcpy(buf,SERVER_STRING);
    send(client,buf,strlen(buf),0);
    strcpy(buf,"Content-Type: text/html\r\n");
    send(client,buf,strlen(buf),0);
    strcpy(buf,"\r\n");
    send(client,buf,strlen(buf),0);
}
//静态处理
void serve_file(int client,const char *filename)
{
	FILE *resource=NULL;
	int numchars=1;
	char buf[1024];
	buf[0]='A';buf[1]='\0';
	while((numchars>0)&&strcmp("\n",buf))
		numchars=get_line(client,buf,sizeof(buf));

	resource=fopen(filename,"r");
	if(resource==NULL)
		not_found(client);
	else
	{
		headers(client,filename);
		cat(client,resource);
	}
	fclose(resource);

}
//发送文件内容
void cat(int client,FILE *resource)
{
	char buf[1024];
	fgets(buf,sizeof(buf),resource);
	while(!feof(resource))
	{
		send(client,buf,strlen(buf),0);
		fgets(buf,sizeof(buf),resource);
	}	
}

//处理cgi，path执行文件地址，method方法，query_string参数
void execute_cgi(int client,const char *path,const char *method,const char *query_string)
{
	char buf[1024];
	int cgi_output[2];
	int cgi_input[2];
	pid_t pid;
	int status;
	int numchars=1;
	int i;
	char c;
	int content_length=-1;

	buf[0]='A';buf[1]='\0';
	//获取请求头
	if(strcasecmp(method,"GET")==0)
		while((numchars>0)&&strcmp("\n",buf))
			numchars=get_line(client,buf,sizeof(buf));
	else if(strcasecmp(method,"POST")==0)
	{
		numchars=get_line(client,buf,sizeof(buf));
		while((numchars>0)&&strcmp("\n",buf))
		{
			char tem=buf[15];
			buf[15]='\0';
			if(strcasecmp(buf,"Content-Length:")==0)
			{	
				content_length=atoi(&(buf[16]));            //获取请求体长度
			}
			numchars=get_line(client,buf,sizeof(buf));
		}
		if(content_length==-1)
		{
			bad_request(client);
			return;
		}
	}
	else
	{
	}
	
	if(pipe(cgi_output)<0)
	{
		cannot_execute(client);
		return;
	}
	if(pipe(cgi_input)<0)
	{
		cannot_execute(client);
		return;
	}
	if((pid=fork())<0)
	{
		cannot_execute(client);
		return;
	}
	
	sprintf(buf,"HTTP/1.0 200 OK\r\n");
	send(client,buf,strlen(buf),0);

	if(pid==0)
	{
		char meth_env[255];
		char query_env[255];
		char length_env[255];
		char type_env[255];

		dup2(cgi_output[1],STDOUT);
		dup2(cgi_input[0],STDIN);
		close(cgi_output[0]);
		close(cgi_input[1]);

		//设置使用方法的环境变量
		sprintf(meth_env,"REQUEST_METHOD=%s",method);
		putenv(meth_env);
		if(strcasecmp(method,"GET")==0)
		{
			sprintf(query_env,"QUERY_STRING=%s",query_string);
			putenv(query_env);
		}
		else
		{
			//在post方法要设置CONTENT_LENGTH
			sprintf(length_env,"CONTENT_LENGTH=%d",content_length);
			putenv(length_env);

		}
		execl(path,NULL);
		exit(0);
	}
	else
	{
		close(cgi_output[1]);
		close(cgi_input[0]);
		if(strcasecmp(method,"POST")==0)
			for(i=0;i<content_length;i++)
			{
				recv(client,&c,1,0); 
				write(cgi_input[1],&c,1); //标准输入到服务器 
			}	
		while(read(cgi_output[0],&c,1)>0) //标准输出到客户端
			send(client,&c,1,0);       	

		close(cgi_output[0]);
		close(cgi_input[1]);
		waitpid(pid,&status,0);
	}
}


	
