#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h>
#include <time.h>
#include<signal.h> 

#define MAXCOUNT 20
#define MAXNUMBER 50

void *thread_matrix_creator();
void *thread_matrix_sum();
void handle_sigint(int sig); 

char number[MAXNUMBER];
int **matrix; 

pthread_mutex_t lock_matrix_sum; // matrix oluşturmak için ama matrixi toplamadan çnce kitlemek için
pthread_mutex_t lock_matrix_main; // main ile matrix toplam arsından kilitleri kontrol için
pthread_mutex_t lock_sum; // toplam fonksiyonu için
pthread_mutex_t lock_main; // main için 

pthread_t tm; // matrix thread
pthread_t ts; // sum thread

int main(){
    // allocate memory for matrix
    matrix = (int **)malloc(sizeof(int *)*MAXCOUNT);
    for(int i = 0; i < MAXCOUNT; i++)
    {
        matrix[i] = (int *)malloc(sizeof(int)*MAXCOUNT);
    }
    signal(SIGINT, handle_sigint);
    
    pthread_mutex_init(&lock_matrix_main, NULL);
    pthread_mutex_init(&lock_matrix_sum, NULL);
    pthread_mutex_init(&lock_sum, NULL);
    pthread_mutex_init(&lock_main, NULL);
    
    if (pthread_create(&tm, NULL, thread_matrix_creator, NULL) != 0) {
            perror("thread couldnt create");
            exit(1);
    }
    if (pthread_create(&ts, NULL, thread_matrix_sum, NULL) != 0) {
        perror("thread couldn't create");
        exit(1);
    }

    pthread_mutex_lock(&lock_matrix_main);
    pthread_mutex_lock(&lock_sum);

    while(1){
        pthread_mutex_lock(&lock_main);
        printf("Please give a number :\n");
        fgets(number,MAXNUMBER,stdin);
        if(atoi(number) <= 0){
            printf("PROGRAM HAS FINISHED. GOODBYE FELLAS\n");
            pthread_kill(tm,SIGKILL);
            pthread_kill(ts,SIGKILL);
            exit(0);
        }
        printf("%d MATRIX HAS CREATED\n",atoi(number));
        pthread_mutex_unlock(&lock_matrix_main);
    }
    
    return 0;
}

void *thread_matrix_creator(){
    while(1){
        printf("MATRIXS ARE PRINTIG : \n");
        pthread_mutex_lock(&lock_matrix_main);
         // Initialization, should only be called once
        srand(time(NULL));
        for(int j=0;j<atoi(number);j++){
            pthread_mutex_lock(&lock_matrix_sum);
            
            for (int i = 0; i < 20; i++)
            {
                for (int j = 0; j < 20; j++)
                {
                    *(*(matrix+i)+j) = rand()% MAXNUMBER;
                }
            }
            
            for (int i = 0; i < 20; i++)
            {
                for (int j = 0; j < 20; j++)
                {
                    printf("%d ",*(*(matrix+i)+j));
                }
                printf("\n");
            }
            pthread_mutex_unlock(&lock_sum);
        }
        pthread_mutex_unlock(&lock_main);
    }
    return NULL;
}

void *thread_matrix_sum()
{
    while(1)
    {
        pthread_mutex_lock(&lock_sum);
        int sum = 0;
        printf("--------SUM OF MATRIX ELEMENTS-----------\n");
        for (int i = 0; i < 20; i++)
        {
            for (int j = 0; j < 20; j++)
            {
                sum += *(*(matrix+i)+j);
            }
        }
        printf("******    %d    *****\n",sum);
        pthread_mutex_unlock(&lock_matrix_sum);
    }
    return NULL;
}

void handle_sigint(int sig) 
{ 
    signal(SIGTSTP,handle_sigint);
    pthread_kill(tm,SIGKILL);
    pthread_kill(ts,SIGKILL);
    printf("Threads have finished %d\n", sig); 
    exit(0);
} 