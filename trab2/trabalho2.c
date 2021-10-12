/*
* Trabalho 2 de Implementação
* Alunas
* Ana Claudia Ribeiro dos Santos
* Rebeca Batista Medeiros da Fonseca
*/
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include "timer.h"

pthread_mutex_t mutex_prod_cons, mutex_escritor;
pthread_cond_t cond_cons, cond_prod;

#define NUM_PRODUTORES 1
#define TAM_BUFFER 10

FILE *arquivo_entrada;
FILE *arquivo_saida;

long int N;
int NUM_CONSUMIDORES;
int *buffer_compartilhado;
int total_numeros = 0;
int count = 0;
int in = 0;
int out = 0;
int DEBUG = 1;

void insere(long int id)
{

    pthread_mutex_lock(&mutex_prod_cons);

    while (count == 10)
    {
        // if(DEBUG) printf("P[%ld] em espera ..\n", &id);
        pthread_cond_wait(&cond_prod, &mutex_prod_cons);
    }
    //if(DEBUG) printf("P[%ld] lendo o bloco\n");
    for (int j = 0; j < N; j++)
    {
        fscanf(arquivo_entrada, "%d", &buffer_compartilhado[in * N + j]); //faz a leitura do arquivo e coloca no buffer_compartilhado
    }
    //decrementa em blocos
    total_numeros -= N;
    in = (in + 1) % TAM_BUFFER;
    count++;

    pthread_mutex_unlock(&mutex_prod_cons);
    //if(DEBUG) printf("P[%ld] Liberando para consumidor acessar ..\n", id);
    pthread_cond_signal(&cond_cons);
}

int *retira(long int id)
{
    pthread_mutex_lock(&mutex_prod_cons);

    while (count == 0)
    {
        //aguardando algo ser produzido

        if (total_numeros == 0)
        {
            //se entrou aqui é pq já consumiu tudo
            //if(DEBUG) printf("C[%ld] saindo ..\n");
            pthread_mutex_unlock(&mutex_prod_cons);
            pthread_exit(NULL);
        };
        //if(DEBUG) printf("C[%ld] em espera ..\n", id);

        pthread_cond_wait(&cond_cons, &mutex_prod_cons);
    }

    //pega só o N bloco do buffer compatilhado
    int *array;
    array = (int *)malloc(N * sizeof(int));
    for (int j = 0; j < N; j++)
    {
        array[j] = buffer_compartilhado[out * N + j];
    }
    count--;
    out = (out + 1) % TAM_BUFFER;

    pthread_mutex_unlock(&mutex_prod_cons);
    //if(DEBUG) printf("C[%ld] Liberando para um produtor \n",id);

    //libera sinal para uma thread produtora
    pthread_cond_signal(&cond_prod);
    return array;
}

void processaElemento(int *array)
{

    int aux;
    //ordenação
    for (int k = 1; k < N; k++)
    {
        for (int i = 0; i < N - 1; i++)
        {
            if (array[i] > array[i + 1])
            {
                aux = array[i];
                array[i] = array[i + 1];
                array[i + 1] = aux;
            }
        }
    }

    //padrão escritor, variavel mutex separada
    pthread_mutex_lock(&mutex_escritor);

    //guardando no arquivo
    for (int i = 0; i < N; i++)
    {
        fprintf(arquivo_saida, "%d", array[i]);
        fputc(' ', arquivo_saida); //separador entre numeros
    }
    fputc('\n', arquivo_saida); //add quebra de linha

    pthread_mutex_unlock(&mutex_escritor);

    pthread_mutex_lock(&mutex_prod_cons);

    //libera threads
    if (total_numeros == 0)
    {
        pthread_cond_broadcast(&cond_cons);
    }
    //libera mutex
    pthread_mutex_unlock(&mutex_prod_cons);
}

void *consumidor(void *args)
{
    long int id = (long long int)args;
    int *elemento;
    while (1)
    {
        //consome um elemento
        elemento = retira(id);
        //ordena e escreve no arquivo
        processaElemento(elemento);

        //forma de finalizar e não ficar em loop
        if (total_numeros == 0 && count == 0)
        {
            break;
        }
    }
    pthread_exit(NULL);
}

void *produtor(void *arg)
{
    long int id = (long long int)arg;
    while (1)
    {
        insere(id); //coloca um slot no buffer_compartilhado

        if (total_numeros <= 0)
        { //forma de finalizar e não ficar em loop
            break;
        }
    }
    pthread_exit(NULL);
}

int main(int argc, char *argv[])
{
    //medir tempo
    double ini, fim;

    //tempo inicio
    GET_TIME(ini);

    //inicializa as variaveis de sincronizacao
    pthread_mutex_init(&mutex_prod_cons, NULL);
    pthread_mutex_init(&mutex_escritor, NULL);
    pthread_cond_init(&cond_cons, NULL);
    pthread_cond_init(&cond_prod, NULL);

    //valida entrada
    if (argc < 5)
    {
        printf("Digite %s < Threads consumidoras > < Tamanho por bloco > < arquivo_entrada > < arquivo_saida > \n", argv[0]);
        printf("Ex: %s 1 5 arquivo-entrada.txt arquivo-saida.txt 1 \n", argv[0]);
        exit(1);
    }

    //recebendo parametros da linha de comandp
    NUM_CONSUMIDORES = atoi(argv[1]);
    N = atoi(argv[2]);                      // tamanho do bloco
    arquivo_entrada = fopen(argv[3], "rb"); //abre arquivo de entrada para leitura
    arquivo_saida = fopen(argv[4], "wb");   //abre arquivo de saida para escrita

    //ve se conseguiu abrir arquivo
    if (arquivo_entrada == NULL)
    {
        printf("Problema ao abrir arquivo de entrada\n");
        exit(1);
    }
    //ve se deu erro ao criar arquivo
    if (arquivo_saida == NULL)
    {
        printf("Problema ao criar arquivo de saída\n");
        exit(1);
    }

    fscanf(arquivo_entrada, "%d", &total_numeros); // pega a primeira linha

    //buffer_compartilhado compartilhado
    buffer_compartilhado = (int *)malloc(10 * N * sizeof(int));
    if (buffer_compartilhado == NULL)
    {
        printf("ERROR.. malloc - buffer\n");
        exit(1);
    }

    //identificadores de threads
    int prodMaisCons = NUM_PRODUTORES + NUM_CONSUMIDORES;
    pthread_t tid[prodMaisCons];
    int ident[prodMaisCons];

    //tempo fim
    GET_TIME(fim);
    printf("Tempo da execução sequencial: %lf\n", fim - ini);

    //tempo inicio
    GET_TIME(ini);

    //cria as threads produtoras
    for (int i = 0; i < NUM_PRODUTORES; i++)
    {
        ident[i] = i;
        if (pthread_create(&tid[i], NULL, produtor, (void *)&ident[i]))
        {
            printf("ERROR.. pthread_create\n");
            return 2;
        }
    }

    //cria as threads consumidoras
    for (int i = 0; i < NUM_CONSUMIDORES; i++)
    {
        ident[i] = i + NUM_CONSUMIDORES;
        if (pthread_create(&tid[i + NUM_PRODUTORES], NULL, consumidor, (void *)(void *)&ident[i]))
        {
            printf("ERROR.. pthread_create\n");
            return 2;
        }
    }

    //esperando as threads
    for (int i = 0; i < prodMaisCons; i++)
    {
        if (pthread_join(tid[i], NULL))
        {
            fprintf(stderr, "ERRO.. pthread_create\n");
            return 3;
        }
    }

    //tempo fim
    GET_TIME(fim);
    printf("Tempo da execução usando threads: %lf\n", fim - ini);

    //corretude quantidade de valores escritos, caso não seja igual a primeira linha do arquivo, fecha o programa antes da mensagem de fim
    // if(total_numerosEscritos!=total_nums){
    //     printf("---------------------ERRO CORRETUDE QUANTIDADE VALORES ESCRITOS-------------------\n");
    //     exit(1);
    // }

    printf("Verifique o arquivo corretude.py para ver se o processamento foi feito com sucesso.\n");

    //liberando as estruturas alocadas
    free(buffer_compartilhado);
    fclose(arquivo_saida);
    fclose(arquivo_entrada);
    return 0;
}