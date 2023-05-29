#include <sys/types.h>
#include <sys/stat.h>
#include <stdint.h>
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/sysmacros.h>
#include <dirent.h>
#include <string.h>

int counters[7];

void enterdir(char* path){
    DIR *d=opendir(path);
    if (d==NULL){
        return;
    }
    struct dirent *rdir = readdir(d);
    struct stat sb;
    char actualpath [PATH_MAX+1];
    char *abspath;
    abspath = realpath(path, actualpath);
    char *tempstring;

    while (rdir!=NULL){
        if (strcmp((char*)rdir->d_name,".")&&strcmp((char*)rdir->d_name,"..")){
            printf("File name: %s\n",rdir->d_name);
            
            tempstring=malloc(100*sizeof(char));
            strcpy(tempstring,abspath);
            tempstring=strcat(tempstring,"/");
            tempstring=strcat(tempstring,rdir->d_name);

            printf("Absolute path: %s\n",tempstring);
            stat(tempstring, &sb);
            printf("File type: ");

            switch (sb.st_mode & S_IFMT) {
                case S_IFBLK: 
                    printf("block device\n");
                    counters[0]++;
                    break;
                case S_IFDIR:
                    printf("directory\n");
                    counters[1]++;
                    break;
                case S_IFIFO:
                    printf("FIFO\n");
                    counters[2]++;
                    break;
                case S_IFLNK:
                    printf("slink\n");  
                    counters[3]++;
                    break;
                case S_IFREG: 
                    printf("regular file\n");  
                    counters[4]++;
                    break;
                case S_IFSOCK: 
                    printf("socket\n");    
                    counters[5]++;
                    break;
                case S_IFCHR: 
                    printf("character device\n");    
                    counters[6]++;
                    break;
                default:       
                    printf("other\n");
                    break;
            }
            printf("Link count: %ju\n", (uintmax_t) sb.st_nlink);

            printf("File size: %jd bytes\n",
                    (intmax_t) sb.st_size);

            printf("Last file access: %s", ctime(&sb.st_atime));
            printf("Last file modification: %s", ctime(&sb.st_mtime));
            if ((sb.st_mode & S_IFMT)==S_IFDIR){
                enterdir(tempstring);
            }
            free(tempstring);
        }
        rdir=readdir(d);
    }
    closedir(d);
}


int main(int argc, char *argv[]){
	char* path=malloc(40*sizeof(char));
	if (argc!=2){
		printf("Bledne argumenty\n");
        exit(1);
	}
	else{
		path=argv[1];
    }
    
    for (int i; i<7; i++){
        counters[i]=0;
    }

    enterdir(path);
    printf("Regular files: %d \nDirectories: %d \nCharacter devices: %d\nBlock devices: %d\nFIFO: %d\nSymbolic links: %d\nSockets: %d\n",
    counters[4],counters[1],counters[6],counters[0],counters[2],counters[3],counters[5]);

	return 0;
}