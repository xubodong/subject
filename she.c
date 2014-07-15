#include<stdio.h>
#include<fcntl.h>
#include<sys/types.h>
#include<sys/stat.h>
#include<errno.h>
#include<string.h>
int main(int argc , char *argv[])
{
int fd,len,i;
char ch[30] = "o world !";
fd =  open("ww.txt",O_CREAT|O_WRONLY,0777);
perror("open");
len = lseek(fd,0,SEEK_END);
perror("lseek");
write(fd,ch,strlen(ch));
perror("write");
}
