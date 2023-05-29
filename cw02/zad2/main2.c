#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h> 
#include <time.h>

int main(int argc, char *argv[]){
    clock_t timer;
    timer=clock();
	char* filename=malloc(20*sizeof(char));
	char* c;
	if (argc!=3){
		printf("Bledne argumenty\n");
        exit(1);
	}
	else{
		c=argv[1];
		filename=argv[2];
    }
	int f1;
	f1=open(filename,O_RDONLY);

	int linecount=0;
    int charcount=0;
    int charinline=0;
	char a;
	while(read(f1,&a,1)==1){
        if (a==*c){
            if (charinline==0){
					charinline=1;
                    linecount++;
				}
            charcount++;
        }
        else if(a=='\n'){
            charinline=0;
        }
	}
    timer=clock()-timer;
    printf("Character count: %d\nLine count: %d\n",charcount,linecount);
    printf("Execution time: %f s\n",(double)timer/CLOCKS_PER_SEC);
	return 0;
}