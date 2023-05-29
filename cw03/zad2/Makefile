ver1:
	gcc -c main.c
	gcc main.o -o main

test1:
	./main 0.000000001 1
	./main 0.000000001 2
	./main 0.000000001 4
	./main 0.000000001 8
	./main 0.0000000001 1
	./main 0.0000000001 2
	./main 0.0000000001 4
	./main 0.0000000001 8

clean:
	rm *o
	rm main
	rm w*.txt
