#include<dirent.h>
#include<stdlib.h>
#include<stdio.h>  
#include<unistd.h>  
#include<string.h>
#include<time.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <sys/types.h> 
#define path_max 1000
#define p_s(a) printf("%s",a)
#define scs(a) scanf("%s",a)

int buffer_temp=0;
int buff_size=0;
struct buffer
{
    char *command;
    char *arg1;
    char *arg2;
}buff[5];



void add_to_buffer(char *comm,char *arg1,char *arg2)
{
	if(strcmp(comm,"history")!=0)
	{
		buff[buffer_temp].command=comm;
    		if(strlen(arg1)>0)
    		{
    			buff[buffer_temp].arg1=arg1;
    			if(strlen(arg2)>0)
	    		{
	    			buff[buffer_temp].arg2=arg2;
	    		}
	    		else
	    		{
	    			buff[buffer_temp].arg2=NULL;
	    		}
    		}
    			
    		else
    		{
    			buff[buffer_temp].arg1=NULL;
    		}
   		 buffer_temp=(buffer_temp+1)%5;
   		// printf("%s",buff[buffer_temp-1].command);
   		 if(buff_size<=5)
    			buff_size++;
    		//printf("%s %s %d",buff[buffer_temp-1].command,buff[buffer_temp-1].arg,buffer_temp-1);
	}
}

void open(char arg1[],char arg2[])
{
	if(strlen(arg2)==0)
		if(!fork())
	    		execlp(arg1,arg1,NULL, NULL);
	else if(!fork())
    		execlp(arg1,arg1,arg2, NULL);
}

void curtime()
{
	time_t t;
	printf("%s",ctime(&t));
}

void rm(char arg[])
{
	if (remove(arg) == 0) 
	    printf("Deleted successfully"); 
	else
	    printf("Unable to delete the file"); 
	  
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


void check(char command[10],char arg1[100],char arg2[100])
{
	//printf("%s",arg);
	if(strcmp(command,"ls")==0 && strlen(arg1)==0)
	{
		ls();
	}
	else if(strcmp(command,"ls")==0 && strlen(arg1)!=0)
		printf("error using ls command , please check the syntax \n");

	if(strcmp(command,"cd")==0 && strlen(arg1)!=0 && arg1!="")
	{
		cd(arg1);
	}
	if(strcmp(command,"mkdir")==0 && strlen(arg1)!=0 && arg1!="")
	{
		makedir(arg1);
	}
	if(strcmp(command,"echo")==0 && strlen(arg1)!=0 && arg1!="")
	{
		echo(arg1);
	}
	if(strcmp(command,"history")==0 && strlen(arg1)==0)
	{
	    //printf("%s",buff[0].command);
		history();
	}
	if(strcmp(command,"rm")==0 && arg1!="")
	{
		rm(arg1);
	}
	if(strcmp(command,"time")==0 && strlen(arg1)==0)
	{
		curtime();
	}
	if(strcmp(command,"open")==0)
	{
		open(arg1,arg2);
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
		char argument1[100]="";
		char argument2[100]="";
		char ch;
		int i=0,j=0,sp=0;
		printf("\n%s @ %s >> ",user,working_dir);
	    	while ((ch = getchar())!='\n')
         	{
			if(ch==' ')
           		{ 
				sp=1; 
				if(strcmp(command,"open")==0)
				{
					while ((ch = getchar())!='\n')
				 	{
						if(ch==' ')
				   		{ 
							sp++;
							if(sp==2) 
								break;
						}  
					}
				}
				break;
			}
 	        	command[i++]=ch;   
		}
		if(strcmp(command,"exit")==0)break;
		if(sp==1)
		{
			while ((ch = getchar())!='\n')
        		{
            			argument1[j++] = ch;
        		}
		}
		if(sp==2)
		{
			while ((ch = getchar())!=' ')
        		{
            			argument1[j++] = ch;
        		}
        		while ((ch = getchar())!='\n')
        		{
            			argument2[j++] = ch;
        		}
		}
		check(command,argument1,argument2);
		if(strcmp(command,"history")!=0)
		{
			printf("%s",command);
			add_to_buffer(&command[0],&argument1[0],&argument2[0]);
		}
	}
}

int main()
{
	loop();
}
