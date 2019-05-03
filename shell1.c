#include<dirent.h>
#include<stdlib.h>
#include<stdio.h>
#include<unistd.h>
#include<string.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <sys/types.h>
#define path_max 1000
#define p_s(a) printf("%s",a)
#define scs(a) scanf("%s",a)

void check(char command[10],char arg[100]);
int buffer_temp=0;
int buff_size=0;


struct buffer{
    char command[10];
    char arg[100]    ;
}buff[5];

void add_to_buffer(char comm[10],char argu[100])
{
    strcpy(buff[buffer_temp].command,comm)    ;
    if(strlen(argu)>0)
    strcpy(buff[buffer_temp].arg,argu);
        else
            strcpy(buff[buffer_temp].arg,"");
        buffer_temp=(buffer_temp+1)%5;
   // printf("%s",buff[buffer_temp-1].command);
    if(buff_size<5)
    buff_size++;
}

void calander()
{	// prints calender
	system("cal");
}

void history()
{

    int fd[2];
    pipe(fd);
    pid_t f=fork();

     if(f==0)
    {

        int select;
        close(fd[0]);
        int i;
        for(i=0;i<buff_size;i++)
        {

            printf("\n#%d : %s %s",i+1,buff[i].command,buff[i].arg);

        }
         printf("\nSelect any one process to run else press 0\n");
         scanf("%d",&select);
         write(fd[1],&select,sizeof(select));
         close(fd[1]);
        exit(0);
    }
    else if(f>0)
    {
       int selected;
        close(fd[1]);
        wait(NULL);
        read(fd[0],&selected,sizeof(selected));
        close(fd[0]);
        check(buff[selected-1].command,buff[selected-1].arg);
    }
}


void echo(char arg[])
{

    char out[100];
    int len=strlen(arg);
    if(strlen(arg)>100)printf("OUT OF BUFFER RANGE \n");
    else if(arg[0]=='\"' && arg[len-1]=='\"')
    {
        int i;
        for(i=1;i<len-1;i++)
        {
            printf("%c",arg[i]);
        }
    }
    else
    {
        perror("ERROR : Use \" \" To print");
    }
}


void makedir(char *arg)
{
    pid_t f=fork();
    if(f==0)
    {
        if(mkdir(arg, 0777) == -1)
        {
           perror("soor");
        }
        exit(1);
    }
    else
    {
        wait(NULL);
    }
}


void touchIt(char* arg1)
{	
	pid_t f=fork();
	if(f==0)
	{
        FILE *fptr1; 
		fptr1 = fopen(arg1, "w"); 
		if (fptr1 == NULL) 
    	{	printf("Cannot touch %s",arg1); 
        	exit(0); 
    	} 
  
		// if(mkdir(arg, 0777) == -1)
		// {
		// 	perror("sorry");	
		// }
		fclose(fptr1);
		exit(1);
	}
	else
	{
		wait(NULL);
	}

}


void cd(char arg[100])
{
    if(chdir(arg)!=0)
        perror("Cannot change the directory");
}

void ls()
{
    pid_t f=fork();
    int stat;

    if(f==0)
    {
        char dirname[10];
        DIR*p;
        struct dirent *d;
        p=opendir(".");
        if(p==NULL)
        {
            exit(-1);
        }
        while(d=readdir(p))
        {
            if(strcmp(d->d_name, ".") == 0 || strcmp(d->d_name, "..") == 0)
                continue;
            printf("%s ",d->d_name);
        }
        exit(1);
    }

    else
    {
        wait(&stat);
        if (WIFEXITED(stat))
        {
            if(WEXITSTATUS(stat)!=1)printf("NO files Found");
        }
    }
}


void check(char command[10],char arg[100])
{
    if(strcmp(command,"ls")==0 && strlen(arg)==0)
    {
        ls();
    }
    /*else if(strcmp(command,"ls")==0 && strlen(arg)!=0)
        printf("error using ls command , please check the syntax \n");
*/
    if(strcmp(command,"cd")==0 && strlen(arg)!=0 && arg!="")
    {
        cd(arg);
    }

    if(strcmp(command,"mkdir")==0 && strlen(arg)!=0 && arg!="")
    {
        makedir(arg);
    }

    if(strcmp(command,"echo")==0 && strlen(arg)!=0 && arg!="")
    {
        echo(arg);
    }

    if(strcmp(command,"history")==0 && strlen(arg)==0)
    {
        history();
    }
    if(strcmp(command,"cal")==0 && strlen(arg)==0)
	{
		calander();
	}
    if(strcmp(command,"touch")==0 && strlen(arg)!=0 && arg!="")
	{
		touchIt(arg);
	}
}


void loop(void)
{
    char *user=getenv("USER");
    char working_dir[path_max];
    while(1)
    {
         getcwd(working_dir,path_max);
         char command[10]="";
         char argument[100]="";
         char ch;
         int i=0,j=0,sp=0;
        printf("\n%s @ %s >>",user,working_dir);
        while ((ch = getchar())!='\n')
         {
            if(ch==' ')
            {
                sp=1;
                break;
            }
            command[i++]=ch;
        }
        if(strcmp(command,"exit")==0)break;
        if(sp)
        {
            while((ch=getchar())!='\n')
            {
                argument[j++]=ch;
            }
        }
        check(command,argument);
        if(strcmp(command,"history")!=0)
        add_to_buffer(command,argument);
    }
}


int main()
{

    loop();
    return 0;
}

