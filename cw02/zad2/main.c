#include <stdio.h>
#include <stdlib.h>
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
	
    FILE *f1;
	if ((f1=fopen(filename, "r"))==NULL){
		printf ("Bledna nazwa pliku\n");
		exit(1);
	}

    int linecount=0;
    int charcount=0;
    int charinline=0;
    int i=0;
    char a = fgetc(f1);
	while(a != EOF){
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
        a = fgetc(f1);
	}
    timer=clock()-timer;
    printf("Character count: %d\nLine count: %d\n",charcount,linecount);
    printf("Execution time: %f s\n",(double)timer/CLOCKS_PER_SEC);
	fclose(f1);
	return 0;
}