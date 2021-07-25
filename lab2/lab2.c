/* Multiplicacao de matriz-matriz (quadradas)
*/
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include "timer.h"

float *mat;   //matriz 1
float *mat2;  //matriz 2
float *saida; //matriz de saida
int nthreads; //numero de threads

typedef struct
{
    int id;  //identificador do elemento que a thread ira processar
    int dim; //dimensao das estruturas de entrada
} tArgs;

//funcao em que as threads executarao
void *tarefa(void *arg)
{
    tArgs *args = (tArgs *)arg;
    int dim = args->dim;
    int id = args->id;

    for (int i = id; i < dim; i += nthreads)
    {
        //printf("Sou a thread %d \n", id);
        for (int l = 0; l < dim; l++)
        {
            for (int m = 0; m < dim; m++)
            {
                saida[i * dim + l] += mat[i * dim + l] * mat2[i * dim + l];
            }
        }
    }

    pthread_exit(NULL);
}

int main(int argc, char *argv[])
{
    pthread_t *tid;            //identificadores das threads do sistema
    tArgs *args;               //identificadores locais das threads e dimensao
    double inicio, fim, delta; //calcular tempo
    int dim;                   //dimensao da matriz de entrada

    GET_TIME(inicio);
    //leitura e avaliacao dos parametros de entrada
    if (argc < 3)
    {
        printf(" Digite: %s <dimensao das matrizes>  <numero de threads>\n", argv[0]);
        return 1;
    }
    dim = atoi(argv[1]);
    nthreads = atoi(argv[2]);

    if (nthreads > dim)
        nthreads = dim;

    //alocacao de memoria para as estruturas de dados
    mat = (float *)malloc(sizeof(float) * dim * dim);
    if (mat == NULL)
    {
        printf("ERRO-malloc-mat\n");
        return 2;
    }
    mat2 = (float *)malloc(sizeof(float) * dim * dim);
    if (mat2 == NULL)
    {
        printf("ERRO-malloc-mat2\n");
        return 2;
    }
    saida = (float *)malloc(sizeof(float) * dim * dim);
    if (saida == NULL)
    {
        printf("ERRO-malloc-saida\n");
        return 2;
    }

    //inicializacao das estruturas de dados
    for (int i = 0; i < dim; i++)
    {
        for (int j = 0; j < dim; j++)
        {
            mat[i * dim + j] = 1; //equivalente mat[i][j]
            mat2[i * dim + j] = 1;
            saida[i * dim + j] = 0;
        }
    }

    //multiplicacao da matriz pelo vetor
    //alocacao das estruturas
    tid = (pthread_t *)malloc(sizeof(pthread_t) * nthreads);
    if (tid == NULL)
    {
        puts("ERRO-pthread-malloc");
        return 2;
    }
    args = (tArgs *)malloc(sizeof(tArgs) * nthreads);
    if (args == NULL)
    {
        puts("ERRO-tArgs-malloc");
        return 2;
    }
    GET_TIME(fim);
    delta = fim - inicio;
    printf("Tempo de inicializacao: %lf\n", delta);

    GET_TIME(inicio);
    //criacao das threads
    for (int i = 0; i < nthreads; i++)
    {
        (args + i)->id = i;
        (args + i)->dim = dim;
        if (pthread_create(tid + i, NULL, tarefa, (void *)(args + i)))
        {
            puts("ERRO no pthread_create");
            return 3;
        }
    }

    //espera pelo termino das threads
    for (int i = 0; i < nthreads; i++)
    {
        pthread_join(*(tid + i), NULL);
    }
    GET_TIME(fim);
    delta = fim - inicio;
    printf("Tempo de multiplicacao: %lf\n", delta);

    //exibindo os valores
    // puts("matriz 1");
    // for (int i = 0; i < dim; i++)
    // {
    //     for (int j = 0; j < dim; j++)
    //     {
    //         printf("%.1f ", mat[i * dim + j]);
    //     }
    //     puts("");
    // }

    // puts("matriz 2");
    // for (int i = 0; i < dim; i++)
    // {
    //     for (int j = 0; j < dim; j++)
    //     {
    //         printf("%.1f ", mat2[i * dim + j]);
    //     }
    //     puts("");
    // }

    // puts("matriz de saida");
    // for (int i = 0; i < dim; i++)
    // {
    //     for (int j = 0; j < dim; j++)
    //     {
    //         printf("%.1f ", saida[i * dim + j]);
    //     }
    //     puts("");
    // }

    GET_TIME(inicio);
    //testando os valores
    int erros = 0;
    for (int i = 0; i < dim; i++)
    {
        for (int j = 0; j < dim; j++)
        {
            if (saida[i * dim + j] != mat[i * dim + j] * mat2[i * dim + j] * dim)
            {
                erros++;
                //printf("erro aqui: %lf", saida[i * dim + j]);
            }
        }
    }
    if(erros == 0){
        printf("Tudo certo nos testes :)\n");
    }else{
        printf("Deu erro nos testes :(\n");
    }
    GET_TIME(fim);
    delta = fim - inicio;
    printf("Tempo de teste dos valores: %lf\n", delta);
    

    //liberacao da memoria
    GET_TIME(inicio);

    free(mat);
    free(mat2);
    free(saida);
    free(args);
    free(tid);
    GET_TIME(fim);
    delta = fim - inicio;
    printf("Tempo de finalizacao: %lf\n", delta);
}