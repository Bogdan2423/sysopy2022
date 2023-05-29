static_library:
	gcc -c bibl1.c
	ar rcs libbibl1.a bibl1.o
	gcc -c main.c
	gcc main.o libbibl1.a -o main_stat



shared_library:
	gcc -fPIC -c bibl1.c
	gcc -shared bibl1.o -o libbibl1.so
	gcc -c main.c
	gcc main.o -L. -lbibl1 -Wl,-rpath=. -o main_wspoldz



dynamic_library:
	gcc -fPIC -c bibl1.c
	gcc -shared bibl1.o -o libbibl1.so
	gcc main2.c -o main_ladow_dynam -ldl


compile_static_o3:
	gcc -c -O3 bibl1.c
	ar rcs libbibl1.a bibl1.o
	gcc -c -O3 main.c
	gcc main.o libbibl1.a -o main_stat

compile_static_o0:
	gcc -c -O0 bibl1.c
	ar rcs libbibl1.a bibl1.o
	gcc -c -O0 main.c
	gcc main.o libbibl1.a -o main_stat

compile_static_os:
	gcc -c -Os bibl1.c
	ar rcs libbibl1.a bibl1.o
	gcc -c -Os main.c
	gcc main.o libbibl1.a -o main_stat

test:

	./main_stat a 1000 c text1.txt s 2 c text2.txt s 4 c text3.txt s 453 r 2 r 4 r 453
	./main_stat a 10000000 c text1.txt s 2 s 564 s 234 s 76 c text2.txt s 4 s 2232 s 7777 s 45462 c text3.txt s 453 s 1231 s 547546 s 435345 s 234657 r 2 r 4 r 453
	./main_stat a 10000000 c text1.txt s 2 r 2 s 564 r 564 s 234 r 234 s 76 r 76 s 4 r 4 s 2232 r 2232 s 7777 r 7777 s 45462 r 45462 s 453 r 453 s 1231 r 1231 s 547546 r 547546
	./main_stat a 45345834 c text1.txt c text1.txt c text1.txt c text1.txt c text1.txt c text1.txt c text1.txt c text1.txt c text1.txt c text1.txt c text1.txt c text1.txt c text1.txt c text1.txt
	./main_wspoldz a 1000 c text1.txt s 2 c text2.txt s 4 c text3.txt s 453 r 2 r 4 r 453
	./main_wspoldz a 10000000 c text1.txt s 2 s 564 s 234 s 76 c text2.txt s 4 s 2232 s 7777 s 45462 c text3.txt s 453 s 1231 s 547546 s 435345 s 234657 r 2 r 4 r 453
	./main_wspoldz a 10000000 c text1.txt s 2 r 2 s 564 r 564 s 234 r 234 s 76 r 76 s 4 r 4 s 2232 r 2232 s 7777 r 7777 s 45462 r 45462 s 453 r 453 s 1231 r 1231 s 547546 r 547546
	./main_wspoldz a 45345834 c text1.txt c text1.txt c text1.txt c text1.txt c text1.txt c text1.txt c text1.txt c text1.txt c text1.txt c text1.txt c text1.txt c text1.txt c text1.txt c text1.txt 
	./main_ladow_dynam a 1000 c text1.txt s 2 c text2.txt s 4 c text3.txt s 453 r 2 r 4 r 453
	./main_ladow_dynam a 10000000 c text1.txt s 2 s 564 s 234 s 76 c text2.txt s 4 s 2232 s 7777 s 45462 c text3.txt s 453 s 1231 s 547546 s 435345 s 234657 r 2 r 4 r 453
	./main_ladow_dynam a 10000000 c text1.txt s 2 r 2 s 564 r 564 s 234 r 234 s 76 r 76 s 4 r 4 s 2232 r 2232 s 7777 r 7777 s 45462 r 45462 s 453 r 453 s 1231 r 1231 s 547546 r 547546
	./main_ladow_dynam a 45345834 c text1.txt c text1.txt c text1.txt c text1.txt c text1.txt c text1.txt c text1.txt c text1.txt c text1.txt c text1.txt c text1.txt c text1.txt c text1.txt c text1.txt 