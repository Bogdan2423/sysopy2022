#include <stdio.h>
#include <stdlib.h>
#include <time.h>

int main(int argc, char *argv[]){
	clock_t timer;
    	timer=clock();
	char* filename1=malloc(20*sizeof(char));
	char* filename2=malloc(20*sizeof(char));
	if (argc!=3){
		printf("Podaj nazwy plikow:\n");
		scanf("%s", filename1);
		scanf("%s", filename2);
	}
	else{
		filename1=argv[1];
		filename2=argv[2];
	}
	
	FILE *f1;
	if ((f1=fopen(filename1, "r"))==NULL){
		printf ("Bledna nazwa pliku\n");
		exit(1);
	}
	FILE *f2;
	if ((f2=fopen(filename2, "w"))==NULL){
		printf ("Bledna nazwa pliku\n");
		exit(1);
	}
	char* line=malloc(100*sizeof(char));
	int emptyline=1;
	int i=0;
	while(fgets(line, 100, f1)){
		while (line[i]){
			if (line[i]!=' '){
				if (line[i+1] && line[i]!='\n'){
					emptyline=0;
				}
			}
			i++;
		}
		if (emptyline==0){
			fputs(line, f2);
		}
		emptyline=1;
		i=0;
	}
	timer=clock()-timer;
	printf("Execution time: %f s\n",(double)timer/CLOCKS_PER_SEC);
	fclose(f1);
	fclose(f2);
	return 0;
}
