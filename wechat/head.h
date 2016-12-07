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

int sockfd;   //tcpfd                                   
int udpfd;    //udpfd
struct sockaddr_in cliaddr,seraddr;


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
static void  Creat_ClientSocket(int port,const char *ip); //创建socket
void show_view();         //选项窗口，addresslist or  WeChat
void Error0()  ;          //处理 show_view() 错误输入
void ShowWelcome();
void Error1();            //处理Show_welcome() 错误输入 
void Error2();			  //处理ShowMenus() 错误输入
void Error3();            //处理find_way_view() 错误输入

static void Choise(char ch);  // 选择登陆、注册、退出
static void set_disp_mode(int fd,int option);// 控制密码回显

static void Login(int sockfd);    
void handle_login(int sockfd,struct user_info *user);

static void Register(int sockfd);
void handle_register(int sockfd,struct user_info *user);
static void Exit1();       // 退出欢迎界面
void  Bye();               //退出系统时打印信息

void ShowMenus();          // addresslist 菜单界面
void ChoiseFunction(char ch);   

void print_attribute();//打印属性  姓名   联系方式   性别  
void All(int udpfd);      //打印所有联系人

//query 
void Find(int udpfd);
void pre_continue() ;              //按任意键返回查询方式
void find_way_view();              //查询方式界面
void query_way(char ch);           //查询方式
void query_by_name(int udpfd);     //姓名查询
void handle_name(char ch,struct contact *con); //

void send_name(int udpfd,struct contact *con);
void query_by_tel(int udpfd);
void handle_tel(char ch,struct contact *con);
void send_tel(int sockfd,struct contact *con);



//add
void Add(int udpfd);
void add_contactor(struct contact *contactor);

//modify
void Modify(int udpfd);                 
void handle_option(int udpfd);          //处理修改删除 
void show_option();                     //修改、删除界面
void handle_choise(char ch);            //选择修改、删除

void submit_modify(struct contact *con,char flag); // 提交修改信息
void submit_cancel();                              //取消提交
void modify_info(struct contact *con);    //将信息发送到服务器 
void back();                              //返回menus界面


//delete
void Delete(int udpfd);
void delete_option(char ch,struct contact *con);
void sure_delete(struct contact *con);
//exit
void Exit2();               //menus界面退出




void WeChat();                     //进入微信







#endif
