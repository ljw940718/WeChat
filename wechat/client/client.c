#include"../head.h"


static void  Creat_ClientSocket(int port,const char *ip)
{

	sockfd = socket(AF_INET,SOCK_STREAM,0);
	udpfd = socket(AF_INET,SOCK_DGRAM,0);
	
	bzero(&seraddr,sizeof(seraddr));
	seraddr.sin_family = AF_INET;
	seraddr.sin_port = htons(port);

	bzero(&cliaddr,sizeof(cliaddr));
	cliaddr.sin_family = AF_INET;
	cliaddr.sin_port = htons(port);
	if(sockfd < 0 || udpfd < 0)
	{
		printf("socket error !\n");
		exit(1);
	}
	else
	{
	//	printf("socket success !\n");
		if((inet_pton(AF_INET,ip,&cliaddr.sin_addr))<=0)
		{
			printf("inet_pton error !\n");
			exit(1);
		}
		int res = connect(sockfd,(struct sockaddr*)&seraddr,sizeof(seraddr));
		assert(res != -1);
		printf("connect to server success !\n");
	}
}

static void set_disp_mode(int fd,int option)//contral display 
{
	//set_disp_mode(STDIN_FILENO,0); 
	// option   == 0,close display
	struct termios term;
	if(tcgetattr(fd,&term)==-1)
	{
		printf("error\n");
	}
	if(option)
	{
		term.c_lflag |= ECHOFLAGS;
	}
	else
	{
		term.c_lflag &= ~ECHOFLAGS;
	}
	int err = tcsetattr(fd,TCSAFLUSH,&term);
	if(err == -1 && err == EINTR)
	{
		printf("error!\n");
	}
}

void handle_login(int sockfd,struct user_info *user)
{
	if(user != NULL)
	{
		int size = send(sockfd,user,sizeof(struct user_info),0);
		if(size < 0)
		{
			printf("send error !\n");
		}
		else
		{
			char recvbuf[5] ={0};
			int length = sizeof(seraddr);
			int size = recv(sockfd,recvbuf,5,0);
			if(size < 0)
			{
				printf("recv error !\n");
			}
			else
			{
				if(strcmp(recvbuf,"ok") == 0)
				{
					system("clear");
					printf("Login in success !press enter anykey continue !\n");
					getchar();
				//	close(sockfd);
					show_view();
				}
				else if(strcmp(recvbuf,"no") == 0)
				{
					system("clear");
					printf("Login in faild !please check your password!\n");
					getchar();
					ShowWelcome();
				}
				else
				{
					system("clear");
					printf("Login faild , this user not exist ,press enter anykey continue !\n");
					getchar();
					ShowWelcome();
				}
			}
		}	
	}
}
static void Login(int sockfd)
{
	system("clear");
	char flags;
	struct user_info *user = (struct user_info *)malloc(sizeof(struct user_info));
	user->flag = '1';
	memset(user->username,0,20);
	memset(user->password,0,20);

	fflush(stdin);
	printf("please input usernsme:");
	scanf("%s",user->username);
	getchar();
	printf("please input passwd:");
	set_disp_mode(STDIN_FILENO,0);
	scanf("%s",user->password);
	getchar();
	set_disp_mode(STDIN_FILENO,1);

	printf("\n\nSure Login or not(y/n):");
	scanf("%c",&flags);
	getchar();
	switch(flags)
	{
		case 'y': handle_login(sockfd,user); break;
		case 'n': ShowWelcome(); break;
		default : Error1();      break;
	}
	free(user);
	user = NULL;
}	


void handle_register(int sockfd,struct user_info *user)
{
	if(user != NULL)
	{
		int size = send(sockfd,user,sizeof(struct user_info),0);
		if(size < 0)
		{
			printf("send error !\n");
		}
		else
		{
			char recvbuf[5]={0};
			int size = recv(sockfd,recvbuf,5,0);
			if(size <= 0)
			{
				printf("recv error !\n");
			}
			else
			{
				if(strcmp(recvbuf,"ok")== 0)
				{
					system("clear");
					printf("Register success ! press enter anykey continue \n");
					getchar();
					ShowWelcome();
				}
				else
				{
					system("clear");
					printf("Register faild ! this user existed ! press enter anykey continue\n");
					getchar();
					ShowWelcome();
				}
			}
		}
	}

}


static void Register(int sockfd)
{
	system("clear");
	char flags;
	struct user_info *user = (struct user_info *)malloc(sizeof(struct user_info));
	user->flag = '2';
    memset(user->username,0,20);
 	memset(user->password,0,20);
    fflush(stdin);

    printf("please input usernsme:");
	scanf("%s",user->username);
	getchar();
    printf("please input passwd:");
    set_disp_mode(STDIN_FILENO,0);
	scanf("%s",user->password);
	getchar();
    set_disp_mode(STDIN_FILENO,1);
	
	printf("\n\nSure Register  or not(y/n):");
	scanf("%c",&flags);
	getchar();
	switch(flags)
	{
		case 'y': handle_register(sockfd,user); break;
		case 'n': ShowWelcome();			    break;
		default : Error1();						break;
	}
	free(user);
	user = NULL;
}

void Bye()
{
	system("clear");
	puts("\n\n\t--------------------------------------------------\n");
	puts("\t+         thank you for your use ! Bye ！        +\n");
	puts("\t+                                                +\n");
	puts("\t+                     Author: Liujingwen         +\n");
	puts("\t+                      Date : 2016-12-7          +\n");
	puts("\t--------------------------------------------------\n");
	sleep(2);
	system("reset");
	exit(1);
}
static void Exit1()
{
	system("clear");
	printf("Are you sure Exit ?(y/n):");
	char flag = getchar();
	getchar();
	switch(flag)
	{
		case 'y': Bye();           break;
		case 'n': ShowWelcome();   break;
		default : Error1();        break;
	}
}


void Error1()
{
	system("clear");
	printf("input error ! please input again !\n");
	sleep(1);	
	ShowWelcome(); 
}

static void Choise(char ch)
{
	switch(ch)
	{
		case '1':	  Login(sockfd);	   break;
		case '2':     Register(sockfd);    break;
		case '3':     Exit1() ;            break;
		default :     Error1();			   break;
	}
}

void Error2()
{
	system("clear");
	printf("input error ! please input again !\n");
	sleep(1);
	ShowMenus(); 
}


void ChoiseFunction(char ch)
{
	switch(ch)
	{
		case '1': All(udpfd);			 break;
		case '2': Find(udpfd);			 break;
		case '3': Add(udpfd);			 break;
		case '4': Exit2();				 break;
		default : Error2();    		     break;
	}
}


void print_attribute()//打印属性  姓名   联系方式   性别
{
	system("clear");
	struct contact con;
	memset(con.name,'\0',20);
	memset(con.tel,'\0',20);
	memset(con.sex,'\0',20);

	strcpy(con.name,"name");
	strcpy(con.tel,"tel");
	strcpy(con.sex,"sex");
	char buff2[100] = {0} ;
	sprintf(buff2,"%10s%15s%10s",con.name,con.tel,con.sex);
	printf("%s\n",buff2);
	puts("--------------------------------------\n");
}

void All(int udpfd)
{
	struct contact *con = (struct contact *)malloc(sizeof(struct contact));
	if(con != NULL)
	{
		con->flag = '1';
		int size = sendto(udpfd,con,sizeof(struct contact),0,(struct sockaddr*)(&seraddr),sizeof(seraddr));
		if(size < 0)
		{
			printf("sendto error !\n");
		}
		else
		{
			print_attribute();
			char buff[MAXSIZE];
			while(1)
			{
				memset(buff,'\0',MAXSIZE);
				int ser_length = sizeof(seraddr);
				int size = recvfrom(udpfd,buff,MAXSIZE,0,(struct sockaddr*)(&seraddr),&ser_length);
				if(size < 0)
				{
					printf("recvfrom error !\n");
				}	
				else
				{
					if(strcmp(buff,"no") == 0)
					{
						system("clear");
						printf("this is a blank addresslist !press enter anykey continue !\n");
						getchar();
						ShowMenus();
						break;
					}
					else if(strcmp(buff,"ok") == 0)
					{
						printf("\n\n  over ! ");
						break;
					}
					else
					{
						printf("%s\n",buff);
					}
				}
			}
			printf("press enter anykey continue !\n");
			getchar();
			handle_option(udpfd);
		}
	}
	free(con);
	con =  NULL;
}

void handle_option(int udpfd)
{
	show_option();
}

void show_option()
{
	system("clear");
	fflush(stdin);
	puts("\t---------------------------------------\n");
	puts("\t*               options               *\n");
	puts("\t---------------------------------------\n");
	puts("\t*       1-Midify_info_by_tel          *\n");
	puts("\t*       2-Delete_contact              *\n");
	puts("\t*       3-Back_previous_page          *\n");
	puts("\t---------------------------------------\n");

	printf("\n\t\tplsase input your choise[_]\b\b");
	char ch = getchar();
	getchar();
	handle_choise(ch);
}

void handle_choise(char ch)
{
	switch(ch)
	{
		case '1':	Modify(udpfd);		 	 break;
		case '2':	Delete(udpfd);			 break;
		case '3':   back();					 break;
		default :   handle_option(udpfd);	 break;
	}
}

void Modify(int udpfd)
{
	system("clear");
	fflush(stdin);
	struct contact *con = (struct contact *)malloc(sizeof(struct contact));
	if(con == NULL) exit(1);	
	con->flag = '5';
	memset(con->name,'\0',20);
	memset(con->tel,'\0',20);
	memset(con->sex,'\0',5);

	printf("please input contact tel :");
	scanf("%s",con->tel);
	int size = sendto(udpfd,con,sizeof(struct contact),0,(struct sockaddr *)(&seraddr),sizeof(seraddr));
	if(size < 0)
		printf("sendto error !\n");
	while(1)
	{
		char buff[MAXSIZE]= {'\0'};
		int length = sizeof(seraddr);
		
		size = recvfrom(udpfd,buff,MAXSIZE,0,(struct sockaddr *)(&seraddr),&length);
		if(size < 0) 
			printf("recvfrom error !\n");
		else
		{
			if(strcmp(buff,"no") == 0)
			{
				system("clear");
				printf("this contact is not exist !\n");
				sleep(2);
				show_option();
				break;
			}
			else
			{
				print_attribute();
				printf("%s\n",buff);

				memset(con->tel,'\0',20);
				fflush(stdin);
				printf("plsase input contact name:");
				scanf("%s",con->name);
				getchar();
				printf("please input contact  tel:");
				scanf("%s",con->tel);
				getchar();
				printf("please input contact sex:");
				scanf("%s",con->sex);
				getchar();

				printf("\n\nSure modify this contact intomation ?(y/n):");
				char ch = getchar();
				getchar();
				submit_modify(con,ch);	
				break;
			}
		}
	}
}

void submit_modify(struct contact *con,char ch)
{
	if( con != NULL)
	{
		switch(ch)
		{
			case 'y':	modify_info(con);			break; 
			case 'n':   submit_cancel();			break;
			default :   submit_cancel();			break;
		}
	}
}

void submit_cancel()
{
	int size = sendto(udpfd,"no",2,0,(struct sockaddr *)(&seraddr),sizeof(seraddr));
	if(size < 0)
	{
		printf("sendto no error !\n");
	}
	show_option();
}
void modify_info(struct contact *con)
{
	if( con != NULL)
	{
		int size1 = sendto(udpfd,"yes",3,0,(struct sockaddr *)(&seraddr),sizeof(seraddr));
		if(size1 < 0)
		{
			printf("sendto server yes error !\n");
		}
		int size2 = sendto(udpfd,con,sizeof(struct contact),0,(struct sockaddr*)(&seraddr),sizeof(seraddr));
		if(size2 < 0)
			printf("sendto error !\n");
		else
		{
			char buff[5] = {'\0'};
			int length = sizeof(seraddr);
			int size = recvfrom(udpfd,buff,5,0,(struct sockaddr *)(&seraddr),&length);
			if(size < 0)
				printf("recvfrom error !\n");
			else
			{
				if(strcmp(buff,"ok") == 0)
				{
					system("clear");
					printf("information modify success !press enter anykey continue\n");
					getchar();
					ShowMenus();
				}
				else
				{
					system("clear");
					printf("information modify faild ！ press enter anykey continue\n");
					getchar();
					ShowMenus();
				}
			}
		}

	}
}

void Delete(int udpfd)
{
	fflush(stdin);
    struct contact *con = (struct contact *)malloc(sizeof(struct contact));
	if(con != NULL)
	{
		system("clear");
		con->flag = '6';
		memset(con->name,'\0',20);
		printf("please input contact name :");
		scanf("%s",con->name);
		getchar();
		printf("\n\nSure delete this contact ?(y/n):");
		char ch = getchar();
		getchar();
		delete_option(ch,con);
	}
	free(con);
	con = NULL;
}

void delete_option(char ch,struct contact *con)
{
	if(con != NULL)
	{
		switch(ch)
		{
			case 'y':	sure_delete(con);	break;
			case 'n':	show_option();		break;
			default :	show_option();		break;
		}
	}
}

void sure_delete(struct contact *con)
{
	if(con != NULL)
	{
		int size = sendto(udpfd,con,sizeof(struct contact),0,(struct sockaddr *)(&seraddr),sizeof(seraddr));
		if(size < 0)
			printf("sendto error !\n");
		else
		{
			char buff[5]={'\0'};
			int length = sizeof(seraddr);
			int size = recvfrom(udpfd,buff,5,0,(struct sockaddr*)(&seraddr),&length);	
			if(size < 0)
				printf("recvfrom error !\n");
			else
			{
				if(strcmp(buff,"ok") == 0)
				{
					system("clear");
					printf("the contact delete success ! press enter anykey continue !\n ");
					getchar();
					ShowMenus();
				}
				else if(strcmp(buff,"no") == 0)
				{
					system("clear");
					printf("delete faild !press enter anykey continue !\n");
					getchar();
					ShowMenus();
				}
				else
				{
					system("clear");
					printf("this contact is not exist ! press enter anykey continue !\n");
					getchar();
					ShowMenus();
				}
			}	
		}
	}
}

void Find(int udpfd)
{
	find_way_view();
}	
void query_by_name(int udpfd)
{
	fflush(stdin);
	struct contact *con = (struct contact *)malloc(sizeof(struct contact));
	if(con != NULL)
	{
		memset(con->name,'\0',20);
		con->flag = '2';
		system("clear");
		printf("please input the contact name :");
		scanf("%s",con->name);
		getchar();
		printf("\n\nsure query or not(y/n)");
		char ch = getchar();
		getchar();
		handle_name(ch,con);
	}
	free(con);
	con = NULL;
}

void handle_name(char ch,struct contact *con)
{
	if(con != NULL)
	{
		switch(ch)
		{
             case 'y':  send_name(udpfd,con);            break;
             case 'n':  find_way_view();                 break;
             default :  find_way_view() ;                break;             
         }
	}
}
void pre_continue()
{
	printf("please enter anykey continue !\n");
	getchar();
	find_way_view();
}

void send_name(int udpfd,struct contact *con)
{	
	if(con != NULL)
	{
		int size = sendto(udpfd,con,sizeof(struct contact),0,(struct sockaddr*)(&seraddr),sizeof(seraddr));
		if(size < 0)
		{
			printf("sendto error !\n");
		}
		else
		{
			print_attribute();
			char buff[MAXSIZE];
			int length = sizeof(seraddr);
			while(1)
			{
				memset(buff,'\0',MAXSIZE);
				int size = recvfrom(udpfd,buff,MAXSIZE,0,(struct sockaddr *)(&seraddr),&length);
				if(size < 0)
				{
					printf("recvfrom error !\n");
				}
				else
				{
					if(strcmp(buff,"no") == 0)
					{
						printf("this contact is not exist !");
						break;
					}
					else if(strcmp(buff,"ok") == 0)
					{
						printf("\n\n   over ! ");
						break;
					}
					else
					{
						printf("%s\n",buff);
					}
				}
			}
			pre_continue();
		}
	}
}


void query_by_tel(int udpfd)
{
	fflush(stdin);
	struct contact *con = (struct contact *)malloc(sizeof(struct contact));
	if(con != NULL)
	{
		memset(con->tel,0,20);
		con->flag = '3';
		system("clear");
		fflush(stdin);
		printf("please input the contact tel :");
		scanf("%s",con->tel);
		getchar();
		printf("\n\nsure query or not(y/n):");
		char ch = getchar();
		getchar();
		handle_tel(ch,con);
	}
	free(con);
	con = NULL;
}

void handle_tel(char ch,struct contact *con)
{
	if(con != NULL)
	{
		switch(ch)
		{
			case 'y': send_tel(udpfd,con); break;
		    case 'n': find_way_view();     break;
		    default : find_way_view();     break;
		}
	}
}


void send_tel(int udpfd,struct contact *con)
{
	if(con != NULL)
	{
		int size = sendto(udpfd,con,sizeof(struct contact),0,(struct sockaddr*)(&seraddr),sizeof(seraddr));
		if(size < 0)
			printf("sendto error !\n");
		else
		{
			print_attribute();
			char buff[MAXSIZE];
			int ser_length = sizeof(seraddr);
			while(1)
			{
				memset(buff,'\0',MAXSIZE);
				int size = recvfrom(udpfd,buff,MAXSIZE,0,(struct sockaddr*)(&seraddr),&ser_length);
				if(size < 0)
				{
					printf("recvfrom error !\n");
				}
				else
				{
					if(strcmp(buff,"no") == 0)
					{
						printf("this menber not exist !");		
						break;
					}
					else if(strcmp(buff,"ok") == 0)
					{
						printf("\n\n  over ! ");
						break;
					}
					else
					{
						printf("%s\n",buff);
					}
				}
			}
			pre_continue();
		}
	}
}


void back()
{
	ShowMenus();
}

void Error3()
{
	system("clear");
	printf("input error ,please input again !press enter anykey continue !\n");
	getchar();
	find_way_view();
}
void query_way(char ch)
{
	switch(ch)
	{
		case '1': query_by_name(udpfd);break;
		case '2': query_by_tel(udpfd); break;
		case '3': back();              break;
		default : Error3();            break;
	}
}
void find_way_view()
{
	system("clear");
	while(1)
	{
		puts("\t-------------------------------------------\n");
		puts("\t*         welcome to addresslist          *\n");
		puts("\t-------------------------------------------\n");
		puts("\t*            1. query_by_name             *\n");
		puts("\t*            2. query_by_tel              *\n");
		puts("\t*            3. back                      *\n");
		puts("\t-------------------------------------------\n");

		printf("\t\tplease input your choise[_]\b\b");
		fflush(stdin);
		char ch = getchar();
		getchar();
		query_way(ch);	
		break;
	}
}

void add_contactor(struct contact *contactor)
{
	if(contactor != NULL)
	{
		printf("name = %s\n",contactor->name);
		int size = sendto(udpfd,contactor,sizeof(struct contact),0,(struct sockaddr*)(&seraddr),sizeof(seraddr));
		if(size < 0)
		{
			printf("sendto error !\n");
		}
		else
		{
			printf("size = %d\n",size);
			char recvbuf[5];
			memset(recvbuf,0,5);
			int length = sizeof(cliaddr);
			int size1 = recvfrom(udpfd,recvbuf,5,0,(struct sockaddr *)(&cliaddr),&length);
			if(size1 < 0)
			{
				printf("recvfrom error\n");
			}
			else
			{
				if(strcmp(recvbuf,"ok") == 0)
				{
					system("clear");
					printf("add contact success ! press enter anykey continue !\n");
					getchar();
					ShowMenus();
				}
				else
				{
					system("clear");
					printf("add contact faild !this tel has existed , press enter anykey continue !\n");
					getchar();
					ShowMenus();
				}
			}	
		}
	}	
	
}

void Add(int udpfd)
{
	system("clear");
	fflush(stdin);
	char flags;
	struct contact *con = (struct contact *)malloc(sizeof(struct contact));
	if(con != NULL)
	{
		memset(con->sex,'\0',5);
		memset(con->tel,'\0',20);
		memset(con->name,'\0',20);
		
		con->flag = '4';     
		printf("please input name:");
		scanf("%s",con->name);
		getchar();
		printf("please input Tel:");
		scanf("%s",con->tel);
		getchar();
		printf("please input sex:");
		scanf("%s",con->sex);
		getchar();
		printf("\n\nSave or not ?(y/n) : ");
		scanf("%c",&flags);
		getchar();
		switch(flags)
		{
			case 'y':  add_contactor(con);  break;
			case 'n':  ShowMenus();			 break;
			default :  Error2();			 break;
		} 
	}
	free(con);
	con = NULL;
}


void Exit2()
{
	fflush(stdin);
	system("clear");
	printf("\n\nSure exit or not(y/n):");
	char ch = getchar();
	getchar();
	switch(ch)
	{
		case 'y':Bye();			  break;
		case 'n': ShowMenus();    break;
		default : Error2();       break;
	}
}

void ShowWelcome()
{
	while(1)
	{	
		fflush(stdin);
		system("clear");
		puts("\t--------------------------------------------\n"); 
		puts("\t*            welcom  to  you  !            *\n");
		puts("\t--------------------------------------------\n");
		puts("\t*               1.Login                    *\n");
		puts("\t*               2.Register                 *\n");
		puts("\t*               3.Exit                     *\n");
		puts("\t--------------------------------------------\n");
		printf("\t\tplease input your choise :[_]\b\b");
		fflush(stdin);
		char ch = getchar();
		getchar();
		Choise(ch);
		break;
	}
}
void show_view()
{
	while(1)
	{
		system("clear");
		fflush(stdin);
		puts("\t-----------------------------------\n");
		puts("\t+             Options             +\n");
		puts("\t-----------------------------------\n");
		puts("\t+        1- Addresslist           +\n");
		puts("\t+        2- Wechat                +\n");
		puts("\t+        3- Exit                  +\n");
		puts("\t-----------------------------------\n");
		printf("\t\tplease input your choise[_]\b\b");
		char ch = getchar();
		getchar();
		switch(ch)
		{
			case '1':  ShowMenus();  break;
			case '2':  WeChat();     break;
			case '3':  Exit2();      break;
			default :  Error0();     break;
		}
	}
}
void WeChat()
{
	printf("waiting . . . . . . \n");
	show_view();
}
void ShowMenus()
{
	while(1)
	{
		fflush(stdin);
		system("clear");
		puts("\t-----------------------------------------\n");
		puts("\t*       welcom  to  addresslist!        *\n");
		puts("\t-----------------------------------------\n");
		puts("\t*          1.Print_contact              *\n");
		puts("\t*          2.Find_contact               *\n");
		puts("\t*          3.Add_contact                *\n");
		puts("\t*          4.Exit_system                *\n");
		puts("\t-----------------------------------------\n");
		printf("\t\tplease input your choise :[_]\b\b");
		fflush(stdin);
		char ch = getchar();
		getchar();
		ChoiseFunction(ch);
		break;
	}
}

void Error0()
{
	system("clear");
	printf("enter  error ！please enter again !\n");
	getchar();
	show_view();
}

int main()
{

	Creat_ClientSocket(PORT,IP);
	sleep(2);
	ShowWelcome();

	return 0;
}


