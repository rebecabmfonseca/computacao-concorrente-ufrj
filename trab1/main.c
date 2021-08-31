/* Programa que faz a busca em um arquivo grande .txt e retorna a quantidade de ocorrências da plavra buscada.
*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "timer.h"
#include <time.h>
#include <pthread.h>

FILE **texto;
long long int quantidadeLinhas = 0;
long long limite;
int nThreads;
char *valorBusca;

typedef struct
{
    int id;
} tArgs;

void *tarefa(void *arg)
{
    tArgs *entrada = (tArgs *)arg;
    long long int comecoLinha = 0;
    long long linhas = 0;
    long long aux;
    long long index;
    long long totalLinhas = limite;
    int *totalEncontrados; //ponteiro pq vai retornar quando a thread terminar aqui
    char *pos, *str;

    str = (char *)malloc(sizeof(char) * 2000);
    if (str == NULL)
    {
        printf("Error no malloc -- str");
        exit(2);
    }

    totalEncontrados = (int *)malloc(sizeof(int));
    if (totalEncontrados == NULL)
    {
        printf("Error no malloc -- totalEncontrados");
        exit(2);
    }
    *totalEncontrados = 0;

    aux = limite * (entrada->id);
    if (entrada->id == (nThreads - 1))
        totalLinhas = quantidadeLinhas - aux;

    fseek(*(texto + entrada->id), 0, SEEK_SET); //move ponteiro para o inicio (SEEK_SET)

    while ((fgets(str, 2000, *(texto + entrada->id))) != NULL)
    { //faz a leitura do arquivo em si
        if (comecoLinha == aux)
        {
            index = 0;
            linhas++;
            if (linhas > totalLinhas)
                break;
            while ((pos = strstr(str + index, valorBusca)) != NULL)
            {
                index = (pos - str) + 1;
                *totalEncontrados += 1;
            }
        }
        else
        {
            comecoLinha++;
        }
    }

    fclose(*(texto + entrada->id));         //fecha o arquivo
    free(str);                              //libera
    pthread_exit((void *)totalEncontrados); //retorna totalEncontrados
}

int main(int argc, char *argv[])
{
    int valor;
    int *retornoThreads;
    int ocorrencias = 0;
    double inicio, fim;
    pthread_t *tid;
    FILE *file;
    tArgs *entrada;
    double total = 0.0;

    GET_TIME(inicio);
    if (argc < 3)
    {
        printf("Digite: %s  <nomeArquivo.txt> <valor que quero encontrar> <numero de threads>\n", argv[0]);
        return 2;
    }

    valorBusca = argv[2];
    nThreads = atoi(argv[3]);

    file = fopen(argv[1], "r"); //abrindo arquivo para leitura
    if (file == NULL)
    {
        printf("Erro ao abrir o arquivo.\n");
        return 2;
    }

    //vendo a quantidade de linhas totais no arquivo
    while (((valor = fgetc(file)) != EOF))
    {
        if (valor == '\n')
            quantidadeLinhas++;
    }
    fclose(file); //fechando o arquivo

    texto = malloc(sizeof(FILE *) * nThreads);
    if (texto == NULL)
    {
        printf("Erro no malloc -- texto.\n");
        return 2;
    }

    //
    for (int i = 0; i < nThreads; i++)
    {
        *(texto + i) = fopen(argv[1], "r"); 
        if (*(texto + i) == NULL)
        {
            printf("Erro ao abrir o arquivo.\n");
            return 2;
        }
    }
    GET_TIME(fim);
    total = fim - inicio;
    printf("Tempo sequencial:%.10f\n", fim - inicio);

    GET_TIME(inicio);

    //argumentos para as threads
    entrada = (tArgs *)malloc(sizeof(tArgs) * nThreads);
    if (entrada == NULL)
    {
        printf("Erro no malloc -- entrada");
        return 2;
    }

    //criando os ids das threads
    tid = (pthread_t *)malloc(sizeof(pthread_t) * nThreads);
    if (tid == NULL)
    {
        printf( "Erro no malloc -- tid");
        return 2;
    }

    //divisao igual entre as linhas do arquivo e as threads
    limite = quantidadeLinhas / nThreads;

    //criando as threads
    for (long int i = 0; i < nThreads; i++)
    {
        (entrada + i)->id = i;
        if (pthread_create(tid + i, NULL, tarefa, (void *)(entrada + i)))
        {
            printf("Erro no malloc -- pthread_create");
            return 2;
        }
    }
    
    // aguardando o retorno das threads
    for (long int i = 0; i < nThreads; i++)
    {
        if (pthread_join(*(tid + i), (void **)&retornoThreads))
        {
            printf("Erro no pthread join \n");
            return 2;
        }
        ocorrencias += *retornoThreads;
        printf("Thread: %ld encontrou %d ocorrencias\n", i + 1, *retornoThreads);
    }
    GET_TIME(fim);
    total += fim - inicio;

    printf("Encontrei '%s', %d vezes no arquivo, que tinha %lld linhas \n", valorBusca, ocorrencias, quantidadeLinhas);
    printf("Tempo de execução com %d threads: %.10f\n",nThreads,fim - inicio);
    printf("Tempo total:%.10f \n", total);

    free(tid);
    free(retornoThreads);
    free(texto);
    free(entrada);
    return 0;
}