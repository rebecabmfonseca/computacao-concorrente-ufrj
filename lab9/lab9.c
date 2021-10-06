#include<stdio.h>
#include<stdlib.h>
#include<pthread.h>
#include<semaphore.h>

#define NUM_PRODUTORES 5
#define NUM_CONSUMIDORES 5
#define TAMANHO_BUFFER 10
#define DEBUG 1

sem_t bufferCheio, bufferVazio, semProd, semCons;

int buffer[TAMANHO_BUFFER];

void insere(int value, int id){
    
    if(DEBUG) printf("P[%d] chegou na funcao insere\n", id);
    static int index = 0;
    sem_wait(&bufferVazio);
    sem_wait(&semProd);
    if(DEBUG) printf("P[%d] index=%d, value=%d\n", id, index, value);
    buffer[index]  = value;
    index = (index+1)%TAMANHO_BUFFER;
    if(DEBUG) printf("P[%d] produziu \n", id);
    sem_post(&semProd);
    sem_post(&bufferCheio);
}

void* removendo(int id){
    if(DEBUG) printf("C[%d] chegou na funcao removendo\n",id);

    int* result = (int *) malloc(sizeof(int) * TAMANHO_BUFFER);
    sem_wait(&semCons);

    for(int i = 0; i < TAMANHO_BUFFER; i++){
        sem_wait(&bufferCheio);
        result[i] = buffer[i];
        sem_post(&bufferVazio);
    }
    if(DEBUG) printf("C[%d] consumiu\n", id);

    sem_post(&semCons);
}

void* produtor(void* args){
    int id = (long long int) args;
    int value = 0;
    while(1){
        if(DEBUG) printf("P[%d] na funcao produtor\n", id);
        value = rand() %100;
        insere(value, id);
        if(DEBUG) sleep(2);
    }
    pthread_exit(NULL);
}

void* consumidor(void* args){
    int id = (long long int) args;
    while(1){
        if(DEBUG) printf("C[%d] na funcao consumidor\n", id);
        removendo(id);
        if(DEBUG) sleep(2);
    }
    pthread_exit(NULL);
}

int main(int argc, char* argv[]){
    
    pthread_t* tidConsumidor = (pthread_t *) malloc(sizeof(pthread_t) * NUM_CONSUMIDORES);
    pthread_t* tidProdutor = (pthread_t *) malloc(sizeof(pthread_t) * NUM_PRODUTORES);
    
    //inicio dos semaforos
    sem_init(&semCons, 0, 1);
    sem_init(&semProd, 0, 1);
    sem_init(&bufferCheio, 0, 0);
    sem_init(&bufferVazio, 0, TAMANHO_BUFFER);

    //criando as threads
    for(int i = 0; i < NUM_PRODUTORES; i++){
        if(pthread_create(&tidProdutor[i], NULL, produtor, (void*)i)){
            printf("ERROR... pthread_create\n");
            return 2;
        }
    }
    for(int i = 0; i < NUM_CONSUMIDORES; i++){
        if(pthread_create(&tidConsumidor[i], NULL, consumidor, (void*)i)){
            printf("ERROR... pthread_create\n");
            return 2;
        }
    }

    //esperando as threads
    for(int i = 0; i < NUM_PRODUTORES; i++){
        if(pthread_join(tidProdutor[i], NULL)){
            printf("ERROR... pthread_join\n");
            return 3;
        }
    }
    for(int i = 0; i < NUM_CONSUMIDORES; i++){
        if(pthread_join(tidConsumidor[i], NULL)){
            printf("ERROR... pthread_join\n");
            return 3;
        }
    }
    return 0;
}