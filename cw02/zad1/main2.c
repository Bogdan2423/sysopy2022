#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>
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
	int f1,f2;
	f1=open(filename1,O_RDONLY);
	f2=open(filename2,O_WRONLY|O_CREAT,S_IRUSR|S_IWUSR);

	char* line=malloc(1024*sizeof(char));
	int emptyline=1;
	int i=0;
	int j=0;
	char c;
	while(read(f1,&c,1)==1){
		line[i]=c;
		i++;
		if(i>0){
			if(line[i-1]=='\n'){
				i=0;
				j=0;
				while(!(line[j]=='\n')){
					if(line[j]!=' '){
						emptyline=0;
						break;
					}
					j++;
				}
				if (emptyline==0){
					j=0;
					while(line[j]!='\n'){
						write(f2,&line[j],1);
						j++;
					}
					write(f2,"\n",1);
				}
				emptyline=1;
			}
		}

	}
	timer=clock()-timer;
    	printf("Execution time: %f s\n",(double)timer/CLOCKS_PER_SEC);
	return 0;
}
