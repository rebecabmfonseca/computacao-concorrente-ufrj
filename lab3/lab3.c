//encontrar o maior e menor elemento de um vetor inteiro com uma versão sequencial e uma versão concorrente
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include "timer.h"

long long int dim;
int nthreads;
float *vetor;

typedef struct
{
    int id;
    float maiorLocal;
    float menorLocal;
} tArgs;

void *tarefa(void *arg)
{
    tArgs *args = (tArgs *)arg;

    long int id = args->id; //id da thread
    
    long int tamanhoBloco = dim / nthreads;
    long int ini = id * tamanhoBloco;  // ponto inicial da thread
    long int fim = ini + tamanhoBloco; //ponto final da thread
    float menorElemento = vetor[ini];  //atribuindo um valor qualquer para iniciar a comparacao
    float maiorElemento = vetor[ini];  //atribuindo um valor qualquer para iniciar a comparacao

    if (id == nthreads - 1)
        fim = dim;
    //printf("Sou a Thread %ld, vou executar de %ld ate %ld \n",id, ini, fim);
    for (long int i = ini; i < fim; i++)
    {
        if (vetor[i] < menorElemento)
            menorElemento = vetor[i];
        if (vetor[i] > maiorElemento)
            maiorElemento = vetor[i];
    }
    args->maiorLocal = maiorElemento;
    args->menorLocal = menorElemento;
    pthread_exit((void *)args);
}

int main(int argc, char *argv[])
{
    double ini, fim;
    pthread_t *tid;   //id das threads
    tArgs *retorno; //retorno das threads
    tArgs *args;

    //recebe param de entrada (dim vetor, num threads)
    if (argc < 3)
    {
        fprintf(stderr, "Digite: %s <dimensao do vetor> <numero threads>\n", argv[0]);
        return 1;
    }
    dim = atoll(argv[1]);
    nthreads = atoi(argv[2]);

    //aloca espaco para vetor
    vetor = (float *)malloc(sizeof(float) * dim);
    if (vetor == NULL)
    {
        fprintf(stderr, "ERRO-malloc-cria-vetor\n");
        return 2;
    }
    args = malloc(sizeof(tArgs) * nthreads);
    retorno = malloc(sizeof(tArgs));

    //preenche vetor de entrada
    for (long int i = 0; i < dim; i++)
    {
        vetor[i] = 1000.1 / (i + 1);
    }

    //achar o maior e menor de forma sequencial
    GET_TIME(ini);
    float menorSeq = vetor[0]; //inicializando
    float maiorSeq = vetor[0]; //inicializando

    for (long int i = 0; i < dim; i++)
    {
        if (vetor[i] < menorSeq)
            menorSeq = vetor[i];
        if (vetor[i] > maiorSeq)
            maiorSeq = vetor[i];

    }
    GET_TIME(fim);
    printf("Tempo sequencial: %lf\n", fim - ini);

    //achar o menor e maior de forma concorrente
    GET_TIME(ini);
    //alocando espaco para as threads
    tid = (pthread_t *)malloc(sizeof(pthread_t) * nthreads);
    if (tid == NULL)
    {
        fprintf(stderr, "ERRO--malloc-threads\n");
        return 2;
    }

    // criar thread
    for (long int i = 0; i < nthreads; i++)
    {
        args[i].id = i;
        if (pthread_create(tid + i, NULL, tarefa, &args[i]))
        {
            fprintf(stderr, "ERRO--pthread_create\n");
            return 2;
        }
    }

    //aguardar termino
    float menorConcorrente = vetor[0];
    float maiorConcorrente = vetor[0];
    for (long int i = 0; i < nthreads; i++)
    {
        pthread_join(*(tid + i), (void **)&retorno);
         if (retorno->menorLocal < menorConcorrente)
            menorConcorrente = retorno->menorLocal;
         if (retorno->maiorLocal > maiorConcorrente)
            maiorConcorrente = retorno->maiorLocal;
        
    }
    GET_TIME(fim);
    printf("Tempo concorrente: %lf\n", fim - ini);

    //exibir resultados
    printf("Menor concorrente: %f - Maior concorrente: %f\n", menorConcorrente, maiorConcorrente);
    printf("Menor sequencial: %f - Maior sequencial: %f\n", menorSeq, maiorSeq);

    //verificando valores
    if(menorSeq!=menorConcorrente || maiorSeq!=maiorConcorrente){
        printf("Valores errados :( \n");
    }else{
        printf("Valores corretos :)\n");
    }

    //liberar memoria
    free(vetor);
    free(tid);
    free(args);

    return 0;
}