#include<stdio.h>
#include<unistd.h>
#include<sys/socket.h>
#include<fcntl.h>
#include<netinet/in.h>
#include<string.h>
#include<time.h>
#include<sys/types.h>
#include<stdlib.h>
#include<signal.h>


#define APPR 5001
#define BUFLEN 1024

pthread_t pth;
int sock_fd;
pthread_mutex_t number;
char id[20],key[20];


void input(char *string, int len)
{
	int i = 0;
	while(i < len)
	{
		string[i] = getchar();
		if(string[i] == '\n')
		{
			string[i] = 0;
			break;
		}
		i++;
		if (i == len)
		{
			printf("超过最大长度，请重新输入\n");
			i = 0;
		}
	}
}

void *recieve()
{
	int err;
	char buf[1024];
	while(1)
	{
	err = recv(sock_fd, buf, 1024, 0);
	if(err < 0)
		exit(0);

	if(strcmp(buf, "呵呵，你。。，，wonv；/") == 0)
		pthread_exit(NULL);
	if(strcmp(buf, "fuc你说什么。。//？？】】") == 0)
		exit(0);
	printf("%s\n",buf);

	}


}
void registration()
{
	char id_key[20];
	int err;
	char buf[30];
	int i = 0;

	while(1)
	{
		printf("请输入帐号:");
		input(id_key, 20);
		
		if(err = send (sock_fd, id_key, 20, 0) < 0)
			exit(0);

		if(err = recv(sock_fd, buf, 30, 0) < 0)
			exit(0);

		printf("%s\n",buf);
		if(strcmp(buf,"用户名可用") == 0)
			break;
	}
	strncpy(id, id_key, 20);

	while(1)
	{
		printf("密码\n");
		input(id_key, 20);
		
		printf("验证密码\n");
		input(key, 20);

		if(strcmp(id_key,key) == 0)
			break;
		else
			printf("两次输入的密码不同，请重新输入！\n");
	}

	if(err = send(sock_fd, key, 20, 0) < 0)
		exit(0);

	if(err = recv(sock_fd, buf, 30, 0) < 0)
		exit(0);

	printf("%s\n",buf);
}

int verification()
{
	char id_key[20];
	int i = 0;

	while(i < 3)
	{
		printf("输入密码：");
		input(id_key, 20);
		if(strcmp(id_key,key) == 0)
			return 1;
		
		i++;
		printf("输入错误\n");
	}

	printf("你已输入三次错误密码，再见\n");
	return 0;
}


void login()
{
	char buf[30];
	int flag, key_flag;
	int err;
	
	while(1)
	{
		flag = 0;
		printf("请输入帐号：");
		input(id, 20);

		if(err = send(sock_fd, id, 20, 0) < 0)
			exit(0);

		recv(sock_fd, buf, 20, 0);

		if(strcmp(buf,"帐号输入错误") != 0)
			break;
		else
			printf("%s\n",buf);
	}

	strncpy(key, buf, 20);
	key_flag = verification();

	if(key_flag == 0)
	{
		err = send(sock_fd, "密码三次错误！", 30, 0);
		exit(0);
	}
	else
	{
		if(err = send(sock_fd, "成功登入！", 30, 0) < 0)
			exit(0);
	}

}

void signal_chat ()
{
	int err;
	char id_key[20];
	char buf[1024];
	while(1)
	{
		printf("请输入聊天的人id，输入“exit!”退出\n");
		input(id_key, 20);

		err = send(sock_fd, id_key, 20, 0);
		if(err < 0)
			exit(0);

		if(strcmp(id_key,"exit!") == 0)
		return;
		
		pthread_join(pth, NULL);

		err = recv(sock_fd, buf, 20, 0);
		if(err < 0)
			exit(0);

		pthread_create(pth, NULL, recieve, NULL);
		printf("%s\n",buf);

		if(strcmp(buf, "已连接") == 0)
		{
			while(1)
			{
				printf("请输入：(可以用exit!退出)\n");
				input(buf, 1024);
				err = send(sock_fd, buf, 1024, 0);
				if(err < 0)
					exit(0);

				if(strcmp(buf,"exit!") == 0)
					break;
				
			}

		}

	}
}


void group_chat()
{
	char buf[1024];
	int err;

	while(1)
	{
		printf("请输入：(可以用exit!退出)\n");
		input(buf, 1024);

		err = send(sock_fd, buf, 1024, 0);
		if(err < 0)
			exit(0);

		if(strcmp(buf, "exit!") == 0)
			return ;
	}
}


void subject()
{
	int flag;
	int err;
	char buf[1024];
	
	while(1)
	{
		flag = 0;
		system("clear");
		
		printf("\t\t1.注册账户\n");
		printf("\t\t2.登录账户\n");
		printf("\t\t3.退出\n");
		scanf("%s",buf);
		getchar();

		if(err = send(sock_fd, buf, 10, 0) < 0)
			exit(0);

		if(strcmp(buf, "1") == 0)
		{
			registration();
		}

		else if(strcmp(buf, "2") == 0)
		{
			login();
		}

		else if(strcmp(buf, "3") == 0)
			exit(0);

		else 
		{
			flag = 1;
			printf("请输入正确选项！");

		}

		if(flag == 0)
			pthread_create(&pth, NULL, recieve, NULL);

		while(flag == 0)
		{
			system("clear");
			printf("\t\t1.私聊\n");
			printf("\t\t2.群聊\n");
			printf("\t\t3.退出\n");

			scanf("%s",buf);
			getchar();

			if(err = send (sock_fd, buf, 10, 0) < 0)
				exit(0);


			if( strcmp(buf, "1") == 0)
				signal_chat ();

			else if(strcmp(buf, "2") == 0)
				group_chat ();

			else if(strcmp(buf, "3") == 0)
			{
				pthread_join(pth, NULL);
				break;
			}

			else 
				printf("请输入正确选项!");
		}
	}


}

int main(int argc,char *argv[])
{
	struct sockaddr_in serv_addr;
	int optval;

	signal(SIGINT, SIG_IGN);
	if(argc == 1)
	{
		printf("请输入服务器ip！！！\n");
		exit(0);
	}

	umask(0);
	mkdir("./use",0777);
	chdir("./use/");

	sock_fd = socket(AF_INET, SOCK_STREAM, 0);
	setsockopt(sock_fd, SOL_SOCKET, SO_REUSEADDR, (void *)&optval,sizeof(int));
	memset(&serv_addr, 0, sizeof(struct sockaddr));
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_port = htons(APPR);
	inet_aton(argv[1], &serv_addr.sin_addr.s_addr);
	if(connect(sock_fd, (struct sockaddr *)&serv_addr, sizeof(struct sockaddr))<0)
		perror("connect");
	subject();
}
