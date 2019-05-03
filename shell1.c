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

int buffer_temp=0;
int buff_size=0;
struct buffer{
    char *command;
    char *arg;
}buff[5];



void add_to_buffer(char *comm,char *argu)
{
	if(strcmp(comm,"history")!=0)
	{
		buff[buffer_temp].command=comm;
    		if(strlen(argu)>0)
    			buff[buffer_temp].arg=argu;
    		else
    			buff[buffer_temp].arg=NULL;
   		 buffer_temp=(buffer_temp+1)%5;
   		// printf("%s",buff[buffer_temp-1].command);
   		 if(buff_size<=5)
    			buff_size++;
    		//printf("%s %s %d",buff[buffer_temp-1].command,buff[buffer_temp-1].arg,buffer_temp-1);
	}
}

void history()
{
	
	printf("%s",buff[0].command);
	
}

void echo(char arg[])
{
	
	char out[100];
	int len=strlen(arg);
	if(strlen(arg)>100)
		printf("OUT OF BUFFER RANGE \n");
    	else if(arg[0]=='\"' && arg[len-1]=='\"')
    	{
    		int i;
    		for(i=1;i<len-1;i++)
    		{
    			printf("%c",arg[i]);
    		}
    		//printf("\n");
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
           		perror("sorry");	
        	}
        	exit(1);
	}
	else
	{
		wait(NULL);
	}
}

void cd(char arg[100])
{
	//getcwd()
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
	//printf("%s",arg);
	if(strcmp(command,"ls")==0 && strlen(arg)==0)
	{
		ls();
	}
	else if(strcmp(command,"ls")==0 && strlen(arg)!=0)
		printf("error using ls command , please check the syntax \n");

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
	    //printf("%s",buff[0].command);
		history();
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
		printf("\n%s @ %s >> ",user,working_dir);
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
			while ((ch = getchar())!='\n')
        		{
            			argument[j++] = ch;
        		}
		}
		check(command,argument);
		if(strcmp(command,"history")!=0)
		{
			printf("%s",command);
			add_to_buffer(&command[0],&argument[0]);
		}
	}
}

int main()
{
	loop();
}
