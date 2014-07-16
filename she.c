#include<stdio.h>
#include<string.h>
#include<sys/types.h>
#include<sys/stat.h>
#include<fcntl.h>
#include<unistd.h>
#include<dirent.h>
int main(int argv,char *argc[])
{
char a[100];
DIR *fp;
struct dirent *p1;
struct stat buf;
fp = opendir(argc[1]);
p1 = readdir(fp);
printf("%s\n",p1->d_name);
strcpy(a,argc[1]);
strcat(a,"/");
strcat(a,p1->d_name);
stat(a,&buf);
printf("%ld\n",buf.st_size);
printf("%s\n",ctime(&buf.st_ctime));
}
