#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>

#define NTHREADS 4

long long int contador = 0;
//variavel controladora
int controlador = 0;

// Variaveis de condições
pthread_mutex_t mutex;
pthread_cond_t x_cond;

//Função FazAlgo que faz coisa boba
int FazAlgo(long long int x)
{
    int boba1, boba2;
    boba1 = 10000;
    boba2 = -10000;
    while (boba2 < boba1)
        boba2++;

    return 0;
}

//Funcao T1
void *T1(void *arg)
{
    while (1)
    {

        FazAlgo(contador);

        //exclusao mutua
        pthread_mutex_lock(&mutex);
        contador++;
        //vendo se eh divisivel por 100
        if (contador % 100 == 0)
        {
            //libera todo mundo
            pthread_cond_broadcast(&x_cond);
            controlador = 2;
        }
        //exclusao mutua
        pthread_mutex_unlock(&mutex);
    }
}

//Funcao T2 pedido no exercicio
void *T2(void *arg)
{

    while (1)
    {
        //exclusao mutua
        pthread_mutex_lock(&mutex);
        while (controlador < 1)
        {
            pthread_cond_wait(&x_cond, &mutex);
        }
        printf("%lld\n", contador);

        controlador = 0;
        //exclusao mutua
        pthread_mutex_unlock(&mutex);
    }
}

//Funcao principal
int main()
{
    pthread_t threads[NTHREADS];

    //inicializando mutex e condicional
    pthread_mutex_init(&mutex, NULL);
    pthread_cond_init(&x_cond, NULL);

    //criando as threads
    pthread_create(&threads[0], NULL, T1, NULL);
    pthread_create(&threads[1], NULL, T2, NULL);

    for (int i = 0; i < NTHREADS; i++)
    {
        pthread_join(threads[i], NULL);
    }

    //terminando mutex e condicional
    pthread_mutex_destroy(&mutex);
    pthread_cond_destroy(&x_cond);
    return 0;
}