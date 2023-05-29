#define _XOPEN_SOURCE 500
#include <sys/types.h>
#include <sys/stat.h>
#include <stdint.h>
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/sysmacros.h>
#include <dirent.h>
#include <string.h>
#include <ftw.h>

int counters[7];

char* getfilename(char *path){
    int arrsize=sizeof(path)/sizeof(char);
    int namelen=0;
    int i=arrsize-1;
    while(path[i]!='/' && i>0){
        namelen++;
        i--;
    }
    char* filename=malloc(namelen*sizeof(char));
    i=0;
    for (int j=arrsize-1-namelen; j<arrsize; j++){
        filename[i]=path[j];
        i++;
    }
    return filename;
}

int enterdir(const char *path, const struct stat *sb,
             int tflag, struct FTW *ftwbuf){
    char actualpath [200];
    char *abspath;
    abspath = realpath(path, actualpath);
    char *tempstring;
    printf("%s\n",path);
    char pathcpy[200];
    strcpy(pathcpy,path);
    char* filename=getfilename(pathcpy);
    if (strcmp(filename,".")&&strcmp(filename,"..")){
        
        printf("File name: %s\n",filename);
        
        tempstring=malloc(100*sizeof(char));
        strcpy(tempstring,abspath);
        tempstring=strcat(tempstring,"/");
        tempstring=strcat(tempstring,filename);

        printf("Absolute path: %s\n",tempstring);
        printf("File type: ");

        switch (sb->st_mode & S_IFMT) {
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
        printf("Link count: %ju\n", (uintmax_t) sb->st_nlink);

        printf("File size: %jd bytes\n",
                (intmax_t) sb->st_size);

        printf("Last file access: %s", ctime(&sb->st_atime));
        printf("Last file modification: %s", ctime(&sb->st_mtime));
        free(tempstring);
        }
    return 0;
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

    nftw(path, enterdir, 20 ,0);
    printf("Regular files: %d \nDirectories: %d \nCharacter devices: %d\nBlock devices: %d\nFIFO: %d\nSymbolic links: %d\nSockets: %d\n",
    counters[4],counters[1],counters[6],counters[0],counters[2],counters[3],counters[5]);

	return 0;
}