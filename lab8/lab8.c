#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>

#define NTHREADS 4

// Variaveis globais
sem_t condicao1, condicao2, condicao3; //semaforos para sincronizar a ordem de execucao das threads

//funcao executada pela thread 1
void *t1(void *threadid)
{
    sem_wait(&condicao2);
    printf("Fique a vontade.\n");
    sem_post(&condicao1);
    sem_post(&condicao2);

    pthread_exit(NULL);
}

//funcao executada pela thread 2
void *t2(void *threadid)
{
    printf("Seja bem-vindo!\n");
    sem_post(&condicao2);

    pthread_exit(NULL);
}

//funcao executada pela thread 3
void *t3(void *threadid)
{
    sem_wait(&condicao1);
    sem_wait(&condicao3);
    printf("Volte sempre!\n");

    pthread_exit(NULL);
}

//funcao executada pela thread 4
void *t4(void *threadid)
{

    sem_wait(&condicao2);
    printf("Sente-se por favor.\n");
    sem_post(&condicao3);
    sem_post(&condicao2);

    pthread_exit(NULL);
}

//funcao principal
int main(int argc, char *argv[])
{
    pthread_t tid[NTHREADS];
    int *id[NTHREADS], t;

    for (t = 0; t < NTHREADS; t++)
    {
        if ((id[t] = malloc(sizeof(int))) == NULL)
        {
            pthread_exit(NULL);
            return 1;
        }
        *id[t] = t + 1;
    }

    //inicia os semaforos
    sem_init(&condicao1, 0, 0);
    sem_init(&condicao2, 0, 0);
    sem_init(&condicao3, 0, 0);

    //cria as tres threads
    if (pthread_create(&tid[1], NULL, t2, (void *)id[1]))
    {
        printf("--ERRO: pthread_create()\n");
        exit(-1);
    }
    if (pthread_create(&tid[2], NULL, t3, (void *)id[2]))
    {
        printf("--ERRO: pthread_create()\n");
        exit(-1);
    }
    if (pthread_create(&tid[0], NULL, t1, (void *)id[0]))
    {
        printf("--ERRO: pthread_create()\n");
        exit(-1);
    }
    if (pthread_create(&tid[3], NULL, t4, (void *)id[3]))
    {
        printf("--ERRO: pthread_create()\n");
        exit(-1);
    }

    //--espera todas as threads terminarem
    for (t = 0; t < NTHREADS; t++)
    {
        if (pthread_join(tid[t], NULL))
        {
            printf("--ERRO: pthread_join() \n");
            exit(-1);
        }
        free(id[t]);
    }
    pthread_exit(NULL);
}