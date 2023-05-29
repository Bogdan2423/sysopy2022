#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <math.h>
#include <sys/times.h>
#include <time.h>

int W;
int H;
int **image;
int **negative;
int thread_num;

void load_image(char *filename)
{
    FILE *f = fopen(filename, "r");
    if (f == NULL)
    {
        fprintf(stderr, "Error opening file \n");
        exit(1);
    }
    char buffer[128];
    fgets(buffer, 128, f);
    do
        fgets(buffer, 128, f);
    while (buffer[0] == '#');
    char *w;
    char *h;
    char *buffer2 = buffer;
    
    w = strtok_r(buffer2, " \t\r\n", &buffer2);
    h = strtok_r(buffer2, " \t\r\n", &buffer2);
    W = atoi(w);
    H = atoi(h);
    fgets(buffer, 128, f);

    image = calloc(H, sizeof(int *));
    negative = calloc(H, sizeof(int *));
    for (int i = 0; i < H; i++){
        image[i] = calloc(W, sizeof(int));
        negative[i] = calloc(W, sizeof(int));
    }
    
    char *line = calloc(H, sizeof(char));
    size_t len = 0;
    char *val;
    int row = 0;
    int col = 0;
    while (getline(&line, &len, f))
    {
        buffer2 = line;
        while (val = strtok_r(buffer2, " \t\r\n", &buffer2)){
            image[row][col] = atoi(val);
            col++;
            if (col==W){
                col = 0;
                row++;
                if (row==H){
                    fclose(f);
                    return;
                }
            }
        }
    }
}

void *numbers(void *i)
{
    struct timespec start, end;
    clock_gettime(CLOCK_REALTIME, &start);

    int index = *((int *) i);
    int size = 255 / thread_num;
    int low = index*size;
    int high = (index+1)*size;
    for (int row = 0; row < H; row++)
        for (int col = 0; col < W; col++)
            if (image[row][col] >= low && image[row][col] < high)
                negative[row][col] = 255 - image[row][col];

    clock_gettime(CLOCK_REALTIME, &end);
    double *time = malloc(sizeof(double));
    *time = (end.tv_sec - start.tv_sec) * 1000000 + (end.tv_nsec - start.tv_nsec) / 1000.0;
    printf("Thread %d time: %f\n", index, *time);
    return (void *)time;

}

void *block(void *i)
{
    struct timespec start, end;
    clock_gettime(CLOCK_REALTIME, &start);

    int index = *((int *) i);
    int size = floor((float)W / thread_num);
    int low = index*size;
    int high = (index+1)*size;
    for (int col = low; col < high; col++)
        for (int row = 0; row < H; row++)
            negative[row][col] = 255 - image[row][col];

    clock_gettime(CLOCK_REALTIME, &end);
    double *time = malloc(sizeof(double));
    *time = (end.tv_sec - start.tv_sec) * 1000000 + (end.tv_nsec - start.tv_nsec) / 1000.0;
    printf("Thread %d time: %f\n", index, *time);
    return (void *)time;

}

void save_negative(char *output_file)
{
    FILE *f = fopen(output_file, "w");
    fprintf(f, "P2\n%d %d\n255\n",W, H);
    for (int i = 0; i < H; i++){
        for (int j = 0; j < W; j++)
            fprintf(f, "%d ", negative[i][j]);
        fprintf(f, "\n");
    }
    fclose(f);
}
int main(int argc, char *argv[])
{
    if (argc != 5){
        printf("Usage: main [thread num] [mode] [input file name] [output file name]\n");
        return -1;
    }

    thread_num = atoi(argv[1]);
    char *mode = argv[2];
    char *input_file = argv[3];
    char *output_file = argv[4];

    load_image(input_file);

    struct timespec start, end;
    clock_gettime(CLOCK_REALTIME, &start);

    pthread_t *thread_ids = calloc(thread_num, sizeof(pthread_t));
    int *indices = malloc(thread_num*sizeof(int));
    for (int i = 0; i < thread_num; i++)
    {  
        indices[i] = i;
        if (strcmp(mode, "numbers") == 0)
            pthread_create(&thread_ids[i], NULL, numbers, &indices[i]);
        else if (strcmp(mode, "block") == 0)
            pthread_create(&thread_ids[i], NULL, block, &indices[i]);
        else{
            printf("Wrong mode\n");
            return -1;
        }
    }

    for (int i = 0; i<thread_num; i++){
        pthread_join(thread_ids[i], NULL);
    }

    clock_gettime(CLOCK_REALTIME, &end);
    double time = (end.tv_sec - start.tv_sec) * 1000000 + (end.tv_nsec - start.tv_nsec) / 1000.0;
    printf("Total time: %.3f microseconds\n", time);

    save_negative(output_file);

    for (int i = 0; i < H; i++){
        free(image[i]);
        free(negative[i]);
    }  
    free(image);
    free(negative);
    return 0;
}
