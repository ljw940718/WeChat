#include"../head.h"


MYSQL mysql;
MYSQL_RES *res;
MYSQL_ROW row;

static void  StartMysql()
{

	system("clear");
	MYSQL *res = NULL;
	res = mysql_init(&mysql);
	assert(NULL != res);

	MYSQL *re = mysql_real_connect(&mysql,server_host,user_name,passwd,db_name,0,NULL,0);
	if(NULL == re)
	{
		printf("mysql start  error !\n");
	}
	else
	{
		printf("mysql start  success !\n");
	}
}

static void Creat_and_BindSocket(int port,const char *ip)
{

	sockfd = socket(AF_INET,SOCK_STREAM,0);
	udpfd = socket(AF_INET,SOCK_DGRAM,0);
	
	bzero(&seraddr,sizeof(seraddr));
	seraddr.sin_family = AF_INET;
	seraddr.sin_port = htons(port);

	bzero(&cliaddr,sizeof(cliaddr));
	cliaddr.sin_family = AF_INET;
	cliaddr.sin_port = htons(port);
	if(sockfd < 0 ||  udpfd < 0 )
	{
		printf("socket error !\n");
		exit(1);
	}
	else
		printf("socket create success !\n");
	
	if(inet_pton(AF_INET,ip,&seraddr.sin_addr)<=0)
	{
		printf("inet_pton error !\n");
		exit(1);
	}
	int res1 = bind(sockfd,(struct sockaddr *)&seraddr,sizeof(seraddr));
	int res2 = bind(udpfd,(struct sockaddr *)&seraddr,sizeof(seraddr));
	if( res1 < 0 || res2 < 0 )
	{
		printf("bind error !\n");
		exit(1);
	}
	int res = listen(sockfd,5);
	assert(res != -1);
}

static void add_event(int epfd,int fd,int state)
{
	struct epoll_event ev;
	ev.events = state;
	ev.data.fd = fd;
	epoll_ctl(epfd,EPOLL_CTL_ADD,fd,&ev);
}

static void  handle_accept(int epfd,int sockfd)
{
	int connfd;
	struct sockaddr_in cliaddr;
	int cli_length = sizeof(cliaddr);
	connfd = accept(sockfd,(struct sockaddr *)&cliaddr,&cli_length);
	if(connfd == -1)
	{
		printf("accept error !\n");
	}
	else
	{
		printf("accept a new client :%s:%d\n",inet_ntoa(cliaddr.sin_addr),cliaddr.sin_port);
		add_event(epfd,connfd,EPOLLIN);
	}
}

static void handle_recv(int epfd,int sockfd)
{
	struct user_info *user = (struct user_info *)malloc(sizeof(struct user_info));
	if(user == NULL)  exit(1);
	memset(user->username,0,20);
	memset(user->password,0,20);
	int length = sizeof(seraddr);
	int size = recv(sockfd,user,sizeof(struct user_info),0);
	if(size < 0)
	{
		printf("recv error !\n");
		close(sockfd);
		delete_event(epfd,sockfd,EPOLLIN);
	}
	else if(size == 0)
	{
		printf("disconncet the tcp link !\n");
		close(sockfd);
		delete_event(epfd,sockfd,EPOLLIN);	
	}
	else
	{
		char flag = user->flag;
		switch(flag)
		{
			case '1': ser_handle_login(sockfd,user);    break;
			case '2': ser_handle_register(sockfd,user); break;
		}
	}
	free(user);
	user = NULL;
}

void ser_handle_login(int sockfd,struct user_info *user) //server handle login  
{
	if(user != NULL)
	{
		char buff1[MAXSIZE]={0};
		snprintf(buff1,MAXSIZE-1,"select username  from user where username = '%s'",user->username);
		int flag = mysql_query(&mysql,buff1);  
		if(flag)
		{
			printf("mysql_query error !\n");
		}
		else
		{
			res = mysql_store_result(&mysql);
			if(res != NULL)
			{
				if(mysql_num_rows(res))
				{
					printf("select user table on first!\n");
					mysql_free_result(res);
					
					char buff2[MAXSIZE] = {0};
					snprintf(buff2,MAXSIZE-1,"select username,passwd  from user where username = '%s'and passwd = '%s'",user->username,user->password);
					int flag = mysql_query(&mysql,buff2);
					if(flag)
						printf("mysql-query error !\n");
					else
					{
						res = mysql_store_result(&mysql);
						if(res != NULL)
						{
							if(mysql_num_rows(res))
							{
								printf("select user table on second !\n");
								//username and password are correct 
								int size = send(sockfd,"ok",2,0);
								if(size < 0)
								{
									printf("send error !\n");
								}
								mysql_free_result(res);
							}
							else
							{
								//password error 
								int size = send(sockfd,"no",2,0);
								if( size < 0 )
									printf("send error !\n");
							}
						}
					}
				}
				else 
				{
					//username is not exist 
					int size = send(sockfd,"error",5,0);
					if(size < 0 )
					{
						printf("send error !\n");
					}
				}
			}
		}
	}	
}


void ser_handle_register(int sockfd,struct user_info *user)//  server handle register 
{
	if(user != NULL)
	{
		char buff1[MAXSIZE]={0};
		snprintf(buff1,MAXSIZE-1,"select username from user where username = '%s'",user->username);
		int flag = mysql_query(&mysql,buff1);
		if(flag)
		{
			printf("mysql_query error !\n");
		}
		else
		{
			res = mysql_store_result(&mysql);
			if(res != NULL)
			{
				if(mysql_num_rows(res))
				{
					//printf("selcet success !\n");
					//find username, user exist 
					int size = send(sockfd,"no",2,0);
					if(size < 0) 
						printf("send error !\n");
					mysql_free_result(res);
				}
				else
				{
					char buff2[MAXSIZE] = {0};
					snprintf(buff2,MAXSIZE-1,"insert into user(username,passwd) value('%s','%s')",user->username,user->password);
					int flag = mysql_query(&mysql,buff2);
					if(flag)
						printf("mysql_query error !\n");
					else
					{
						if(mysql_affected_rows(&mysql))
						{
							//printf("insert success !\n");
							//not found , insert information into usertable 
							int size = send(sockfd,"ok",2,0);
							if(size < 0)
							{
								printf("send error !\n");
							}
						}
					}
				}
			}
		}
	}
}



/*
 *handle_udp_data()   handle add、delete、modify、query
*/
static void handle_upd_data(int epfd,int udpfd)
{
	struct contact *cont = (struct contact *)malloc(sizeof(struct contact));
	if(cont != NULL)
	{
		memset(cont->sex,'\0',5);
		memset(cont->tel,'\0',20);
		memset(cont->name,'\0',20);
		int length = sizeof(cliaddr);
		int size = recvfrom(udpfd,cont,MAXSIZE,0,(struct sockaddr*)(&cliaddr),&length);
		if(size < 0 )
		{
			printf("recvfrom error !\n");
		}	
		else if(0 == size)
		{
			printf("udp connect stop!\n");
		}
		else
		{
			char flg = cont->flag;
			printf("size = %d , recvfrom flag  = %c \n",size,flg);
			switch(flg)
			{
				case '1': handle_all(udpfd,cont);               break; 
				case '2': handle_find_by_name(udpfd,cont);      break;
				case '3': handle_find_by_tel(udpfd,cont);       break;
				case '4': handle_add(udpfd,cont);				break;
				case '5': handle_modify(udpfd,cont);			break;
				case '6': handle_delete(udpfd,cont);			break;
			}
		}
	}
	free(cont);
	cont = NULL;
}

void handle_all(int udpfd,struct contact *con)  //print all contacts information
{
	if(con != NULL)
	{
		int flag = mysql_query(&mysql,"select name,tel,sex from addresslist");
		if(flag)
		{
			printf("mysql_query error !\n");
		}
		else
		{
			res = mysql_store_result(&mysql);
			if(res != NULL)
			{
				if(mysql_num_rows(res) == 0)
				{
					int size = sendto(udpfd,"no",2,0,(struct sockaddr *)(&cliaddr),sizeof(cliaddr));
					if(size < 0)
					{
						printf("sendto error !\n");
					}
				}
				else
				{
					char buff[MAXSIZE];
					while(row = mysql_fetch_row(res))
					{
						memset(buff,'\0',MAXSIZE);
						snprintf(buff,MAXSIZE-1,"%10s%15s%10s",row[0],row[1],row[2]);
					//	printf("%s\n",buff);
						int size = sendto(udpfd,buff,sizeof(buff),0,(struct sockaddr *)(&cliaddr),sizeof(cliaddr));
						if(size < 0)
						{
							printf("sendto error !\n");
							break;
						}
					}
					memset(buff,'\0',MAXSIZE);
					int size = sendto(udpfd,"ok",2,0,(struct sockaddr *)(&cliaddr),sizeof(cliaddr));
					if(size < 0)
						printf("sendto error !\n");
				}
			}
			else
			{
				printf("mysql_store_result error !\n");
			}
		}
	}
}


void handle_find_by_name(int udpfd,struct contact *con) //query by name 
{
	if(con != NULL)
	{
		char buff[MAXSIZE]={0};
		snprintf(buff,MAXSIZE-1,"select name,tel,sex from addresslist where name = '%s'",con->name);
		int flag = mysql_query(&mysql,buff);
		if(flag)
			printf("mysql_query error !\n");
		else
		{
			res = mysql_store_result(&mysql);
			if(res != NULL)
			{
				int record = mysql_num_rows(res);
				if(record != 0)
				{
					char buff2[MAXSIZE];
					while(row = mysql_fetch_row(res))
					{
						memset(buff2,'\0',MAXSIZE);
						snprintf(buff2,MAXSIZE-1,"%10s%15s%10s",row[0],row[1],row[2]);
						int size = sendto(udpfd,buff2,sizeof(buff2),0,(struct sockaddr *)(&cliaddr),sizeof(cliaddr));
						if(size < 0)
							printf("sendto error !\n");
					}	
					int size = sendto(udpfd,"ok",2,0,(struct sockaddr *)(&cliaddr),sizeof(cliaddr));
					if(size < 0)
						printf("sendto error !\n");
				}
				else
				{
					int size = sendto(udpfd,"no",2,0,(struct sockaddr *)(&cliaddr),sizeof(cliaddr));
					if(size < 0)
						printf("sendto error !\n");
				}
			}
			else
			{
				printf("mysql_store_result error !\n");
			}
		}
	}
}

void handle_find_by_tel(int udpfd,struct contact *con)   //query by number 
{
	if(con != NULL)
	{
		char buff[MAXSIZE]={0};
		snprintf(buff,MAXSIZE-1,"select name,tel,sex from addresslist where tel = '%s'",con->tel);
		int flag = mysql_query(&mysql,buff);
		if(flag)
			printf("mysql_query error !\n");
		else
		{
			res = mysql_store_result(&mysql);
			if(res != NULL)
			{
				int record = mysql_num_rows(res);
				if(record != 0)
				{
					char buff[MAXSIZE];
					while(row = mysql_fetch_row(res))
					{
						memset(buff,'\0',MAXSIZE);
						snprintf(buff,MAXSIZE-1,"%10s%15s%10s",row[0],row[1],row[2]);
						int size = sendto(udpfd,buff,sizeof(buff),0,(struct sockaddr *)(&cliaddr),sizeof(cliaddr));
						if(size < 0 )
							printf("sendto error !\n");
					}
					memset(buff,'\0',MAXSIZE);
					int size = sendto(udpfd,"ok",2,0,(struct sockaddr *)(&cliaddr),sizeof(cliaddr));
					if(size < 0)
						printf("sendto error !\n");
				}
				else
				{
					int size = sendto(udpfd,"no",2,0,(struct sockaddr*)(&cliaddr),sizeof(cliaddr));
					if(size < 0)
						printf("sendto error !\n");
				}
			}
			else
			{
				printf("mysql_store_result error!\n");
			}
		}
	}
}

void handle_add(int udpfd,struct contact *con)
{
	if(con != NULL)
	{
		printf("name =%s,tel =%s,sex =%s\n",con->name,con->tel,con->sex);
		char buff1[MAXSIZE]={0};
		snprintf(buff1,MAXSIZE-1,"select tel  from addresslist where tel = '%s'",con->tel);
		int flag = mysql_query(&mysql,buff1);
		if(flag == 0)
		{
			//printf("first ,select tel !!\n");
			res = mysql_store_result(&mysql);
			if(res != NULL)
			{
				if(mysql_num_rows(res))
				{
					mysql_free_result(res);
				//	printf("this tel has exist !\n");
					int size = sendto(udpfd,"no",2,0,(struct sockaddr*)(&cliaddr),sizeof(cliaddr));
					if(size < 0)
					{
						printf("sendto error !\n");
					}
				}
				else
				{
				//	printf("not find tel, insert !\n");
					char buff2[MAXSIZE] ={0};
					snprintf(buff2,MAXSIZE-1,"insert into addresslist(name,tel,sex) value('%s','%s','%s')",con->name,con->tel,con->sex);
					int flag = mysql_query(&mysql,buff2);
					if(flag)
						printf("inset ... mysqy_query error !\n");
					else
					{
						if(mysql_affected_rows(&mysql))
						{
				//			printf("insert success !\n");
							int size = sendto(udpfd,"ok",2,0,(struct sockaddr *)(&cliaddr),sizeof(cliaddr));
							if(size <0 )
							{
								printf("sendto error !\n");
							}
						}
						else
						{
							printf("insert error !\n");
						}
					}
				}
			}
		}
		else
		{
			printf("mysql_query error!\n");
		}
	}
}

void handle_modify(int udpfd,struct contact *con)    //handle modify contact 
{
	if(con != NULL)
	{
		char recv_tel[20]={'\0'};
		memcpy(recv_tel,con->tel,strlen(con->tel));
		//printf("recv_tel :%s\n",recv_tel);
		char buff[MAXSIZE]={0};
		snprintf(buff,MAXSIZE-1,"select name,tel,sex from addresslist where tel = '%s'",recv_tel);
		if(mysql_query(&mysql,buff))
		{
			printf("mysql_query first error !\n");
		}
		else
		{
			res = mysql_store_result(&mysql);
			if(res != NULL)
			{
				if(0 == mysql_num_rows(res))
				{
					//printf("not exist ! sendto no \n");
					int size = sendto(udpfd,"no",2,0,(struct sockaddr *)(&cliaddr),sizeof(cliaddr));
					if(size < 0)
						printf("sendto error !\n");
				}
				else
				{
					char buff2[MAXSIZE];
					while(row = mysql_fetch_row(res))
					{
						memset(buff2,'\0',MAXSIZE);
						snprintf(buff2,MAXSIZE-1,"%10s%15s%10s",row[0],row[1],row[2]);
						int size = sendto(udpfd,buff2,sizeof(buff2),0,(struct sockaddr *)(&cliaddr),sizeof(cliaddr));
						if(size < 0)
						{
							printf("sendto error !\n");
						}
					}
					mysql_free_result(res);		
					//printf("query success ! update \n");
					
					struct contact *con = (struct contact *)malloc(sizeof(struct contact));

					if(con != NULL)
                    {
						memset(con->name,'\0',20);
						memset(con->tel,'\0',20);
						memset(con->sex,'\0',5);
						int length = sizeof(cliaddr);
						int size = recvfrom(udpfd,con,sizeof(struct contact),0,(struct sockaddr *)(&cliaddr),&length);
						if(size < 0) 
							printf("recvfrom error !\n");
						else
						{
							memset(buff2,0,MAXSIZE);
							snprintf(buff2,MAXSIZE-1,"update addresslist set name  = '%s', tel = '%s',sex = '%s' where tel = '%s'",con->name,con->tel,con->sex,recv_tel);
							int flag2 = mysql_query(&mysql,buff2);
							if(flag2)
							{
								printf("mysql_query error second !\n");
							}
							else
							{
								int result = mysql_affected_rows(&mysql);
								if(result)
								{
									printf("update success !\n");
									int size = sendto(udpfd,"ok",2,0,(struct sockaddr *)(&cliaddr),sizeof(cliaddr));
									if(size < 0)
										printf("sendto error !\n");
								}
								else
								{
									printf("update faild !\n");
									int size = sendto(udpfd,"no",2,0,(struct sockaddr *)(&cliaddr),sizeof(cliaddr));
									if(size < 0)
										printf("sendto error !\n");
								}
							}
						}
					}
				}
			}
		}
	}
}

void handle_delete(int udpfd,struct contact *con)   //handle delete contact 
{
	if(con != NULL)
	{
		char buff[MAXSIZE] = {0};
		snprintf(buff,MAXSIZE,"select name  from addresslist where  name = '%s'",con->name);
		int flag = mysql_query(&mysql,buff);
		if(flag)
			printf("mysql_query error !\n");
		else
		{
			res = mysql_store_result(&mysql);
			if(res)
			{
				if(mysql_num_rows(res))
				{
					mysql_free_result(res);
					char buff2[MAXSIZE]={0};
					snprintf(buff2,MAXSIZE-1,"delete from addresslist where name = '%s'",con->name);
					int flag = mysql_query(&mysql,buff2);
					if(flag)
						printf("mysql_query error !\n");
					else
					{
						if(mysql_affected_rows(&mysql))
						{
							printf("delete success !\n");
							int size = sendto(udpfd,"ok",2,0,(struct sockaddr *)(&cliaddr),sizeof(cliaddr));
							if(size < 0)
							{
								printf("sendto error !\n");
							}
						}
						else
						{
							printf("delete error !\n");
							int size = sendto(udpfd,"no",2,0,(struct sockaddr *)(&cliaddr),sizeof(cliaddr));
							if(size < 0)
								printf("sendto error !\n");
						}
					}
				}
				else
				{
					printf("this contact is not exist !\n");
					int size = sendto(udpfd,"error",5,0,(struct sockaddr *)(&cliaddr),sizeof(cliaddr));
					if(size < 0)
						printf("sendto error !\n");
				}
			}
			else
			{
				printf("mysql_store_result error !\n");
			}
		}
	}
}


static void delete_event(int epfd,int fd,int state)  // delete events
{
	struct epoll_event ev;
	ev.events = state;
	ev.data.fd = fd;
	epoll_ctl(epfd,EPOLL_CTL_DEL,fd,&ev);
}

static void modify_event(int epfd,int fd,int state)  //modify events 
{
	struct epoll_event ev;
	ev.events = state;
	ev.data.fd = fd;
	epoll_ctl(epfd,EPOLL_CTL_MOD,fd,&ev);
}
static void handle_events(int epfd,struct epoll_event *events,int num,int sockfd)
{
	int i ;
	int fd;
	for(i=0;i<num;i++)
	{
		fd = events[i].data.fd;
		if(fd == sockfd)
		{
			handle_accept(epfd,sockfd); //handle connection 
		}
		else if(fd == udpfd)
		{
			handle_upd_data(epfd,fd); //handle  udp data
		}
		else if(events[i].events & EPOLLIN)
		{
			handle_recv(epfd,fd); //handle register and login 
		}
	}
}
static void Do_epoll()
{
	int epfd;
	struct epoll_event events[EPOLLEVENTS];
	int res;
	epfd = epoll_create(FDSIZE);
	add_event(epfd,sockfd,EPOLLIN);
	add_event(epfd,udpfd,EPOLLIN);
	while(1)
	{
		res = epoll_wait(epfd,events,EPOLLEVENTS,-1);
		handle_events(epfd,events,res,sockfd);
	}
	close(epfd);
}

int main()
{

	
	StartMysql();
	Creat_and_BindSocket(PORT,IP);
	Do_epoll();

	return 0;
}
