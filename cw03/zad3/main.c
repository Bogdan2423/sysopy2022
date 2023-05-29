#include <sys/types.h>
#include <sys/stat.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/sysmacros.h>
#include <dirent.h>
#include <string.h>
#include <unistd.h> 
#include <sys/wait.h>


void enterdir(const char *path, char* str, int maxdepth, int currdepth){
    pid_t pid1;
    pid1=fork();
    if(pid1==0){
        DIR *d=opendir(path);
        if (d==NULL){
            return;
        }

        struct dirent *rdir = readdir(d);
        struct stat sb;
        char *tempstring;

        while (rdir!=NULL){
            if (strcmp(rdir->d_name,".")&&strcmp(rdir->d_name,"..")){
                tempstring=calloc(128,sizeof(char));
                strcpy(tempstring,path);
                strcat(tempstring,"/");
                strcat(tempstring,rdir->d_name);
                stat(tempstring, &sb);
                if (((sb.st_mode & S_IFMT)==S_IFREG)&&strlen(rdir->d_name)>=4){
                    if(strstr(rdir->d_name, ".txt")){
                        FILE *f;
                        if ((f=fopen(tempstring, "r"))==NULL){
                            printf ("Cant open file: %s\n",rdir->d_name);
                            exit(1);
                        }
                        char* line=malloc(100*sizeof(char));
                        while (fgets(line, 100, f)) {
                            if (strstr(line, str)) {
                                printf("Proces %d found string in file: %s\n",getpid(),tempstring);
                                break;
                            }
                        }
                        free(line);
                        fclose(f);
                    }
                }
                
                if ((sb.st_mode & S_IFMT)==S_IFDIR && currdepth<maxdepth){
                    enterdir(tempstring,str,maxdepth,currdepth+1);
                }
                free(tempstring);
            }
            rdir=readdir(d);
        }
        closedir(d);
        exit(0);
    }
}


int main(int argc, char *argv[]){
	char* path=malloc(40*sizeof(char));
    char* str=malloc(256*sizeof(char));
    int maxdepth;
	if (argc!=4){
		printf("Wrong arguments\n");
        exit(1);
	}
	else{
		path=argv[1];
        str=argv[2];
        maxdepth=atoi(argv[3]);
    }


    enterdir(path,str,maxdepth,0);
	return 0;
}