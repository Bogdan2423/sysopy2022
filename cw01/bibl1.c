#include <stdlib.h>
#include <stdio.h>
#include "bibl1.h"

void* allocate_array(int n) {
	void **arr = calloc(n, sizeof(void*));
	return arr;
}

void word_count(char* filename) {
    FILE *input;
    input = fopen(filename, "r");
    if (input == NULL) {
        printf("Bledna nazwa pliku\n");
        exit(0);
    }

    int ch_num = 0;
    int word_num = 1;
    int line_num = 1;

    char ch = fgetc(input);
    while (ch != EOF) {
        if (ch != ' ' && ch != '\n')
            ch_num++;
        if (ch == ' ')
            word_num++;
        if (ch == '\n') {
            word_num++;
            line_num++;
        }
        ch = fgetc(input);
    }
    fclose(input);

    FILE *res;
    res = fopen("result.txt", "w");
    fprintf(res,"%i %i %i", ch_num, word_num, line_num);
    fclose(input);
}

int save_to_array(void* arr[], int index){
    FILE *input;
    input = fopen("result.txt", "r");
    if (input == NULL) {
        printf("Plik nie istnieje\n");
        exit(0);
    }
    fseek(input, 0, SEEK_END);
    int size = ftell(input)+1;
    fseek(input, 0, SEEK_SET);
    void *ptr=calloc(1,size*sizeof(char));
    fgets(ptr, size, input);
    fclose(input);
    arr[index]=ptr;
}

void clear_index(void* arr[], int index){
    free(arr[index]);
}