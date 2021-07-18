/*
Objetivo: Implementar um programa concorrente, com duas threads (alem da thread principal), para elevar ao quadrado cada elemento de um vetor de 10000 elementos. 
(Para cada elemento ai do vetor, calcular (ai)**2 e escrever o resultado na mesma posicao do elemento.)
*/

#include <stdio.h>
#include <pthread.h>

#define NTHREADS 2
#define NELEMENTOS 10
#define METADE 5
int vetor[NELEMENTOS];

//funcao que a thread vai executar
// void * lala(void * lele)
void *tarefa(void *arg)
{
    int ident = *(int *)arg; //typecast pra int
    if (ident == 1)
    {
        printf("Ola, sou a thread %d \n", ident);
        for (int i = 0; i < METADE; i++)
        {
            vetor[i] = i * i;
        }
    }else{
        printf("Ola, sou a thread %d \n", ident);
        for (int j = METADE; j < NELEMENTOS; j++)
        {
            vetor[j] = j * j;
        }
    }
    pthread_exit(NULL);
}

//funcao principal
int main(void)
{
    pthread_t tid[NTHREADS]; //identificador da thread no sistema
    int ident[NTHREADS];     //identificador local da thread
    int quantErros = 0;

    for (int i = 0; i < NTHREADS; i++)
    {
        ident[i] = i;
        //cria a thread nova e verifica seu retorno (sendo diferente de 0 é pq deu algum erro )
        if (pthread_create(&tid[i], NULL, tarefa, (void *)&ident[i]))
            printf("ERRO -- pthread_create\n");
    }

    for (int i = 0; i < NTHREADS; i++)
    {
        //espera as threads terminarem
        if (pthread_join(tid[i], NULL))
            printf("ERRO -- pthread_join\n");
    }
    
    for (int i = 0; i < NELEMENTOS; i++)
    {
        //imprimindo o valor do vetor nessa posicao
       // printf(" %d ",vetor[i]);

        //verifico se todos as posicoes dos vetores tem o valor esperado
        if(vetor[i]!=i*i){
            printf("Valor diferente aqui hein: %d - %d ", vetor[i], i*i);
            quantErros++;
            // incremento quantErros em caso de erro (dãa.. rs)
        }
    }

    //verifico, na minha variavel auxiliar, se teve algum erro ao popular os vetores
    if(quantErros == 0){
        printf("\nResultados OK"); // não teve :)
    }else{
        printf("\nTivemos erros.");// teve :(
    }

    printf("\nOla, sou a thread principal \n");

    //desvincula o termino da main
    pthread_exit(NULL);
    return 0;
}