ver1:
	gcc -c main.c
	gcc main.o -o main -lrt -lpthread
	gcc -c cook.c
	gcc cook.o -o cook -lrt -lpthread
	gcc -c deliver.c
	gcc deliver.o -o deliver -lrt -lpthread

clean:
	rm *o
	rm main
	rm cook
	rm deliver
