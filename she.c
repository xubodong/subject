#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include<time.h>
#include<sys/stat.h>
#include<sys/types.h>
#include<unistd.h>
#include<linux/limits.h>
#include<dirent.h>
#include<grp.h>
#include<pwd.h>
#include<errno.h>

#define PARAM_NONE 0
#define PARAM_A    1
#define PARAM_L    2                                                 
#define PARAM_R    4
#define MAXROWLEN 100 

int     g_leave_len = MAXROWLEN;
int     g_maxlen;

void my_err(const char *err_string, int line)
{
    fprintf(stderr, "line:%d", line);
    perror(err_string);
    exit(1);
}

void display_attribute (struct stat buf,char *name)
{
    char buf_time[32];
    struct passwd *psd;
    struct group *grp;
    int i;
    if(S_ISLNK(buf.st_mode)){
    printf("l");
}   else if(S_ISREG(buf.st_mode)){
    printf("-");
}   else if(S_ISDIR(buf.st_mode)){
    printf("d");
}   else if(S_ISCHR(buf.st_mode)){
    printf("c");
}   else if (S_ISBLK(buf.st_mode)){
    printf("b");
}   else if(S_ISFIFO(buf.st_mode)){
    printf("f");
}   else if(S_ISSOCK(buf.st_mode)){
    printf("s");
}
    for(i=512;i>1;)
{
    if(buf.st_mode & (i=i/2))
    printf("r");
    else
    printf("-");
    if(buf.st_mode & (i=i/2))
    printf("w");
    else
    printf("-");
    if(buf.st_mode & (i=i/2))
    printf("x");
    else
    printf("-");
}

    printf("   ");

    psd = getpwuid (buf.st_uid);
    grp = getgrgid (buf.st_gid);
    printf("%-4d",buf.st_nlink);
    printf("%-8s",psd->pw_name);
    printf("%-8s",grp->gr_name);
    printf("  ");
    printf("%-6d",buf.st_size);
    strcpy(buf_time,ctime(&buf.st_mtime));
    buf_time[strlen(buf_time)-1]=0;
    printf("%s",buf_time);
    
}

void display_single(char *name)
{
    int i, len;
    
    if(g_leave_len < g_maxlen){
    printf("\n");
    g_leave_len = MAXROWLEN;
}
    len = strlen (name);
    len = g_maxlen - len;
    printf("%-s",name);
    
    for(i=0; i<len; i++){
    printf(" ");
}

    printf("  ");
     g_leave_len-=(g_maxlen + 2);
}




void display (int flag, char * pathname)
{
    int           i,j;
    struct stat   buf;
    char          name[4096 + 1];
    for(i=0,j=0; i<strlen(pathname); i++)
    {
         if(pathname[i]=='/')
    {       j=0;
             continue;
    } 
        name[j++] = pathname [i];
    }
    name[j]='\0';
    if(lstat (pathname,&buf)==-1){
    my_err("stat",1);
    }
    if(flag>=4)
     flag-=4;
    switch (flag)
    {
       case PARAM_NONE:
          if(name[0]!='.')
               display_single (name); break;
       case PARAM_A :
          display_single(name);break;
       case PARAM_L:
          if(name[0]!='.')
          {
              display_attribute(buf, name);
              printf(" %-s\n",name);
           }
            break;
       case PARAM_A + PARAM_L:
           display_attribute(buf, name); 
          printf(" %-s\n",name);

       default :
          break;
        }
}

    void qqsort(char (*s)[PATH_MAX+1], int left, int right)
      {
            int i,j,k,l;
            char temp[1024],c[1024];
            if(left>right)
                 return ;
             i = left;
             j = right;
             strcpy(temp,s[i]);
             while(i<j)
             {
               while(strcmp(s[j],temp)>=0&&j>i)
                     j--;
               while(strcmp(s[i],temp)<=0&&j>i)
                       i++;
                if(j>i)
                 {
                   strcpy(c,s[i]);
                   strcpy(s[i],s[j]);
                   strcpy(s[j],c);
                  }
             }
                 strcpy(s[left],s[j]);
                 strcpy(s[j],temp);
                 qqsort(s,j+1,right);
                 qqsort(s,left,j-1);
      }

      void display_dir(int flag_param, char *path)
      {
            DIR               *dir;
            struct dirent     *ptr;
            int               count = 0;      
            char              filenames[256][4096+1],temp[4096 + 1];
            struct stat       buf;
             dir = opendir (path);    
            if(dir == NULL)
             my_err("opendir",1);
            while((ptr = readdir(dir)) != NULL)
              {
                     if( g_maxlen < strlen (ptr->d_name))
                          g_maxlen = strlen (ptr->d_name);
                           count++;
             }
        closedir(dir); 

        if(count>256)
          my_err("too many files under this dir ",2);
        int i, j, k, len = strlen(path);
          char cs[4096];
         dir = opendir (path);
         for(i = 0; i<count; i++)
         {
           ptr = readdir(dir);
            if(ptr == NULL)
           my_err("readdir",1);
            strncpy(filenames[i],path,len);
            filenames[i][len]='/';
            filenames[i][len+1]=0;
            strcat(filenames[i],ptr->d_name);
            filenames[i][len + strlen(ptr -> d_name)+1]=0;
                }
         
         qqsort(filenames,0,count-1);


         for(i=0;i<count; i++)
           display(flag_param,filenames[i]);
           if((flag_param & PARAM_L)==0)
             printf("\n");
         closedir(dir);
       dir=opendir(path);
         for(i=0;i<count; i++)
            
{           
                   
           for(j=0, k=0;filenames[i][j] && k<g_maxlen;j++)
            {  
               if(filenames[i][j]=='/')
                {
                    k=0;
                    continue;
                }
                
               cs[k++]=filenames[i][j];
} 
              cs[k] = 0;
            lstat(filenames[i],&buf);
               if(S_ISDIR(buf.st_mode)&&(flag_param & PARAM_R)&&strcmp(cs,".")&&strcmp(cs,".."))
               {  
                      g_maxlen=100; 
                        
                    if((flag_param & PARAM_A) || cs[0]!='.')
                   {     
                   printf("%s:\n",filenames[i]);
                  display_dir(flag_param,filenames[i]);
                          printf("%d\n",g_maxlen);
}
                }
}
           closedir(dir);
}
        int main (int argc, char **argv)
         {
              int i, j, k, num;
              char path[PATH_MAX+1];
              char param[32];
              int flag_param = PARAM_NONE;
              struct stat  buf;
              j = 0; 
              num = 0;
              for(i=1; i<argc; i++)
               {
                  if(argv[i][0] == '-')
                   {
                     for(k=1; k < strlen (argv[i]); k++, j++)
                     { 
                       param[j] = argv[i][k];
                       }
                      num++;
                    }
               } 
              for(i=0; i<j; i++)
                {
                    if(param[i] == 'a')
                     {
                         flag_param |= PARAM_A; 
                         continue;
                     }
                    if(param[i] == 'l')
                       {
                          flag_param |= PARAM_L;
                          continue;
                        }
                     if(param[i] =='r')
                        {
                           flag_param |= PARAM_R;
                         }
                      else
                         printf("errno");
                }
                param[j]=0;
                if((num+1)==argc)
                {
                   getcwd(path,PATH_MAX+1);
                   display_dir (flag_param,path);
                   return 0;
               } 
                
                 i=1;
                do{
                   if(argv[i][0] == '-'){
                        i++;
                        continue;                
                         
                    }else{
                       strcpy(path , argv[i]);
                 if(stat(path , &buf) == -1)
                     my_err("stat",1);
                 if(S_ISDIR(buf.st_mode)){
                  if(path[strlen(argv[i])-1] != '/')
                      {
                       path[strlen(argv[i])] = '/';
                      path[strlen(argv[i])+1] = 0;
                    }else
                    path[strlen(argv[i])] = 0;

                       display_dir(flag_param,path);
                       i++;
                    }
                      else{
                      display(flag_param,path);
                       i++;
}
}
}while(i<argc);
return 0;
}
