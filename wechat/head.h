#ifndef HEAD_H
#define HEAD_H
#include<sys/time.h>
#include<time.h>
#include<netinet/in.h>
#include<arpa/inet.h>
#include<errno.h>
#include<fcntl.h>
#include<netdb.h>
#include<signal.h>
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<sys/stat.h>
#include<unistd.h>
#include<sys/wait.h>
#include<mysql.h>
#include<assert.h>
#include<termios.h>
#include<unistd.h>
#include<sys/epoll.h>
 
const char *server_host ="localhost";
const char *user_name ="root";
const char *passwd = "luxiao";
const char *db_name = "WeChat";


struct user_info
{
	char username[20];
	char password[20];
	char flag;
};


struct contact                                                          
{
   	char name[20]; 
    char tel[20];
    char sex[5];
	char flag;    // 1,all  2, 3,find  4,add  5,modify  6,delete
};


typedef struct result
{
	MYSQL_RES *u_res;
	char buff[5];
} Query_result;

#define MAXSIZE     1024
#define EPOLLEVENTS 100
#define FDSIZE      1000
#define PORT        8000
#define IP          "127.0.0.1"
#define ECHOFLAGS   (ECHO | ECHOE |ECHOK |ECHONL)

//服务器函数列表
static void StartMysql(); //启动Mysql
static void Creat_and_BindSocket(int port,const char*ip); //创建Socket
static void add_event(int epfd,int fd,int state);//添加事件
static void  handle_accept(int epfd,int sockfd);//处理链接事件
static void handle_recv(int epfd,int sockfd);//处理Tcp数据

void ser_handle_login(int sockfd,struct user_info *user) ;
void ser_handle_register(int sockfd,struct user_info *user);


static void handle_upd_data(int epfd,int udpfd);//处理udp数据


void handle_all(int udpfd,struct contact *con);                 
void handle_find_by_name(int udpfd,struct contact *con);
void handle_find_by_tel(int udpfd,struct contact *con);
void handle_add(int udpfd,struct contact *con);
void handle_modify(int udpfd,struct contact *con);
void handle_delete(int udpfd,struct contact *con);





static void delete_event(int epfd,int fd,int state);//删除事件
static void modify_event(int epfd,int fd,int state);//改变事件
static void handle_events(int epfd,struct epoll_event *events,int     num,int sockfd);//处理事件
static void Do_epoll() ;//epoll处理高并发
static int True(char *buff);

//客户端函数列表
static void  Creat_ClientSocket(int port,const char *ip);
void ShowWelcome();
void Error1();
void Error2();
static void Choise(char ch);
static void set_disp_mode(int fd,int option);//contral display 

static void Login(int sockfd); 
void handle_login(int sockfd,struct user_info *user);

static void Register(int sockfd);
void handle_register(int sockfd,struct user_info *user);
static void Exit1();

void ShowMenus();
void ChoiseFunction(char ch);
void All(int udpfd);

//query 
void Find(int udpfd);
void find_way_view();
void query_way(char ch);
void query_by_name(int udpfd);
void send_name(int udpfd,struct contact *con);
void query_by_tel(int udpfd);
void send_tel(int sockfd,struct contact *con);
void print_attribute();//打印属性  姓名   联系方式   性别  
void Error3();


//add
void Add(int udpfd);
void add_contactor(struct contact *contactor);


//modify
void Modify(int udpfd);

//delete
void Delete(int udpfd);

//exit
void Exit2();
#endif
