#include<stdio.h>
#include<unistd.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<sys/types.h>
#include<string.h>
#include<stdlib.h>
#include<fcntl.h>
#include<signal.h>
#include<time.h>
#include <signal.h>
#include<net/if.h>
#include<sys/ioctl.h>
#include<arpa/inet.h>

#define MAXLINE 20
#define APPR 5001 
#define BUFLEN 1024

struct state_t
{
	char id[20];
	char key[20];
	int conn;
	char state;
	
}state[100];

int number;
int sock_fd;
int logining_number;

int registration (int conn_fd, char id[20])
{
	int flag;
	int err;
	int i;
	int fd;
	char key[20];
	int position;

	do
	{
		i = 0;
		flag = 1;
		err = recv(conn_fd, id, 20, 0);
		if(err <0)
		{
			close(conn_fd);
			pthread_exit(NULL);
		}
		

		pthread_mutex_lock(&number);
		while(i <= logining_number)
		{
			if(strcmp(id, state[i].id) == 0)
			{
				flag = 0;
				break;
			}
			i++;
		}
		pthread_mutex_unlock(&number);

		if(flag == 0)
		{
			err = send (conn_fd, "用户名存在", 30, 0);
			if(err < 0)
			{	
				close(conn_fd);
				pthread_exit(NULL);
			}
		}
		else if(flag == 1)
		{
			err = send (conn_fd, "用户名可用", 30, 0);
			if(err < 0)
			{
				close(conn_fd);
				pthread_exit(NULL);
			}
		}
	}while (flag == 0);

	err = recv(conn_fd, key, 20, 0);
	if(err < 0)
	{	
		close(conn_fd);
		pthread_exit(NULL);
	}

	pthread_mutex_lock(&number);
	logining_number++;
	position = logining_number;
	strcpy(state[logining_number].key, key);
	strcpy(state[logining_number].id, id);
	state[logining_number].state = 1;
	state[logining_number].conn = conn_fd;
	pthread_mutex_unlock(&number);

	fd = open("server.c", O_WRONLY|O_APPEND);
	write(fd, id, 20);
	write(fd, key, 20);

	err = send(conn_fd, "成功创建帐号", 30, 0);
	if(err < 0)
	{
		close(conn_fd);
		state[position].state = 0;
		pthread_exit(NULL);
	}

	return position;

}

int login(int conn_fd, char id[20])
{
	char key[20];
	char buf[1024];
	int position;
	int flag;
	int err;

	while(1)
	{
		position = 0;
		flag = 0;

		err = recv(conn_fd, id, 20, 0);
		if(err < 0)
		{	
			close(conn_fd);
			pthread_exit(NULL);
		}
		

		while(position <= logining_number)
		{	

			if(strcmp(id, state[position].id) == 0)
			{
				flag = 1;
				break;
			}
			position++;
		}
		
		if(flag == 1)
		{
			err = send(conn_fd, state[position].key, 20, 0);
			if(err < 0)
			{
				close(conn_fd);
				pthread_exit(NULL);
			}
			break;
		}

		else 
		{
			err = send(conn_fd, "帐号输入错误", 20, 0);	
			if(err < 0)
			{
				close(conn_fd);
				pthread_exit(NULL);
			}
		}
	}
	err = recv(conn_fd, buf, 30, 0);
	if(err < 0)
	{	
		close(conn_fd);
		pthread_exit(NULL);
	}
	
	if(strcmp(buf,"密码三次错误！") == 0)
	{
		close(conn_fd);
		pthread_exit(NULL);
	}

	else
	{
		if(state[position].state == 1 )
		{	
			err = send(state[position].conn, "你的帐号被他人登录！", 1024, 0);
			if(err < 0)
			{
				close(conn_fd);
				pthread_exit(NULL);
			}

			err = send(state[position].conn, "fuc你说什么。。//？？】】", 1024, 0);
			if(err < 0)
			{
				close(conn_fd);
				pthread_exit(NULL);
			}

		}

		pthread_mutex_lock(&number);
		state[position].state = 1;
		state[position].conn = conn_fd;
		pthread_mutex_unlock(&number);
		return position;
	}
}

void  chatting(int conn_fd, int signal, int position)
{
	char buf[1024];
	int err;
	time_t timer;
	char *string;
	char buf2[1024];
	char id_key[43];
	int fd1,fd2;
	
	mkdir("/home/administrator/聊天记录", 0600);
	chdir("/home/administrator/聊天记录");

	strncpy(id_key, state[signal].id, 20);
	strcat(id_key, state[position].id);
	strcat(id_key, ".c");
	fd1 = open(id_key, O_CREAT|O_WRONLY, 0600);

	strncpy(id_key, state[position].id, 20);
	strcat(id_key, state[signal].id);
	strcat(id_key, ".c");
	fd2 = open(id_key, O_CREAT|O_WRONLY, 0600);

	while(1)
	{
		err = recv(conn_fd, buf, 1024, 0);
		
		time(&timer);
		string = ctime(&timer);
		strcpy(buf2, string);
		strcat(buf2, state[position].id);
		strcat(buf2, ":");
		strcat(buf2, buf);
		strcat(buf2, "\n");
		
		
	
		if(err < 0)
		{
			close(conn_fd);
			state[position].state = 0;
			pthread_exit(NULL);
			
		}

		if(strcmp(buf,"exit!") == 0)
		{
			close(fd1);
			close(fd2);
			chdir("/home/administrator/erv/");
			return;
		}

		err = send(state[signal].conn, buf2, 1024, 0);

		write(fd1, buf2, strlen(buf2));
		write(fd2, buf2, strlen(buf2));

		if(err < 0)
		{
			close(conn_fd);
			state[position].state = 0;
			pthread_exit(NULL);
		}
	}

}



void signal_chat (int conn_fd, int position)
{
	char id_key[20];
	char buf[BUFLEN];
	int signal = 0;
	int flag = 0;
	int err;

	while(flag == 0)
	{
		signal = 0;

		err = recv(conn_fd, id_key, 20, 0);
		if(err < 0)
		{
			close(conn_fd);
			state[position].state = 0;
			pthread_exit(NULL);
		}
		if(strcmp(id_key,"exit!") == 0)
			return;

		while(signal <= logining_number)
		{
			if(strcmp(state[signal].id, id_key) == 0)
			{
				flag = 1;
				break;
			}
			signal++;
		}

		err = send(conn_fd, "呵呵，你。。，，wonv；/", 1024, 0);
		if(err < 0)
		{
			close(conn_fd);
			state[position].state = 0;
			pthread_exit(NULL);
		}


		if(flag == 1)
		{
			if(state[signal].state == 1)
			{	
				err = send(conn_fd, "已连接", 20, 0);
				if(err < 0)
				{
					close(conn_fd);
					state[position].state = 0;
					pthread_exit(NULL);
				}

				chatting(conn_fd,signal,position);
				flag = 0;
			}
			else if(state[signal].state == 0)
			{
				send(conn_fd, "对方不在", 20, 0);
				if(err < 0)
				{
					close(conn_fd);
					state[position].state = 0;
					pthread_exit(NULL);
				}
				flag = 0;
			}
		}

		else
		{
			err = send(conn_fd, "帐号错误", 20, 0);
			if(err < 0)
			{	
				close(conn_fd);
				state[position].state = 0;
				pthread_exit(NULL);
			}
		}
	}
}


void group_chat (int conn_fd, int position, char id[20])
{
	int i = 0;
	char buf[1024];
	int err;
	time_t timer;
	char *string;
	char buf2[1024];
	int fd1, fd2;
	char id_key[43];

	mkdir("/home/administrator/聊天记录", 0600);
	chdir("/home/administrator/聊天记录");

	while(1)
	{
		err = recv(conn_fd, buf, 1024, 0);

		time(&timer);
		string = ctime(&timer);
		strcpy(buf2, string);
		strcat(buf2, id);
		strcat(buf2, ":");
		strcat(buf2, buf);
		strcat(buf2, "\n");

		if(err < 0)
		{	
			close(conn_fd);
			state[position].state = 0;
			pthread_exit(NULL);
		}

		if(strcmp(buf,"exit!") == 0)
		{
			chdir("/home/administrator/erv/");
			return ;
		}

		for(i = 0; i <= logining_number; i++)
		{
			if(state[i].state == 1 && strcmp(state[i].id, id) != 0)
			{
				strncpy(id_key, state[i].id, 20);
				strcat(id_key, state[position].id);
				strcat(id_key, ".c");
				fd1 = open(id_key, O_CREAT|O_WRONLY, 0600);

				strncpy(id_key, state[position].id, 20);
				strcat(id_key, state[i].id);
				strcat(id_key, ".c");
				fd2 = open(id_key, O_CREAT|O_WRONLY, 0600);

				err = send(state[i].conn, buf2, 1024, 0);

				write(fd1, buf2, strlen(buf2));
				write(fd2, buf2, strlen(buf2));

				if(err < 0)
				{
					close(conn_fd);
					state[position].state = 0;
					pthread_exit(NULL);
				}
				close(fd1);
				close(fd2);
			}
		}
	}
}


void *subject( void *fd )
{
	int err;
	char buf[BUFLEN];
	int conn_fd;
	int flag;
	int position;
	char id[20];

	conn_fd = *(int *)fd;

	while(1)
	{
		flag = 1;

		if(err = recv (conn_fd, buf, 10, 0) < 0)
		{
			close(conn_fd);
			pthread_exit (NULL);
		}

		if(strcmp(buf,"1") == 0)
			position = registration(conn_fd, id);
	
		else if(strcmp("2",buf) == 0)
			position = login(conn_fd, id);

		else if(strcmp("3",buf) == 0)
		{	
			close(conn_fd);
			pthread_exit(NULL);
		}

		else 
			flag = 0;

		if(flag)
		while(1)
		{
			if(err = recv (conn_fd, buf, 10, 0) < 0)
			{
				close(conn_fd);
		    	state[position].state = 0;
				pthread_exit(NULL);
			}

			if(strcmp(buf, "1") == 0)
				signal_chat (conn_fd, position );

			else if(strcmp(buf, "2") == 0)
				group_chat (conn_fd, position, id);
	
			else if(strcmp(buf, "3") == 0)
			{
				state[position].state = 0;

				err = send(conn_fd, "呵呵，你。。，，wonv；/", 1024, 0);
				if(err < 0)
				{
					close(conn_fd);
					pthread_exit(NULL);
				}

				break;
			}
		}
	}
		
}


main()
{
	int cli_len;
	int conn_fd, fd;
	int optval;
	struct sockaddr_in sin;
	struct ifreq ifr;
	struct sockaddr_in  serv_addr, clon_addr;
    char id_key[20];
	pthread_t pth;


	signal(SIGPIPE, SIG_IGN);
	signal(SIGCHLD, SIG_IGN);
	umask(0);

	mkdir("/home/administrator/erv",0600);
	chdir("/home/administrator/erv/");

	if((sock_fd = socket(AF_INET, SOCK_STREAM, 0))<0)
		perror("socket");

	optval = 1;
	if(setsockopt(sock_fd, SOL_SOCKET, SO_REUSEADDR, (void *)&optval, sizeof(int))<0)
		perror("setsockopt");
	printf("完成套接字设置\n");

	memset(&serv_addr,0,sizeof(struct sockaddr));
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_port = htons (APPR);
	serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);

	if(bind(sock_fd, (struct sockaddr *)&serv_addr, sizeof(struct sockaddr)) < 0)
		perror("bind");
	printf("绑定端口完成\n");

	if(listen(sock_fd, MAXLINE) < 0)
		perror("listen");

	cli_len = sizeof(struct sockaddr);
	fd = open("server.c",O_CREAT|O_RDONLY,0600);
	logining_number = 0;

	while(read(fd, state[logining_number].id, 20))
	{
		read(fd, state[logining_number].key, 20);
		state[logining_number].state = 0;
		logining_number++;
	}
	logining_number--;
	pthread_mutex_init(&number);

	while(1)
	{
		printf("等待客户端连接\n");
		conn_fd = accept(sock_fd, (struct sockaddr *)&clon_addr, &cli_len);

		if(conn_fd < 0)
			perror("accept");

		pthread_create(&pth, NULL, (void *)subject, (void *)&conn_fd);
		strcpy(ifr.ifr_name, "eth0");
		ioctl(conn_fd, SIOCGIFADDR, &ifr);
		memcpy(&sin, &ifr.ifr_addr, sizeof(sin));
		printf("客户端已连接，ip是：%s\n", inet_ntoa(sin.sin_addr));

	}

}
