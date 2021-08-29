//sincronizacao condicional
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>

#define NTHREADS 5
#define PASSOS 2 //primeiro passo eh o somatorio, segundo eh colocar valor aleatorio no vetor
#define PRINT 1 //ajuda para printar

int bloqueadas = 0;
pthread_mutex_t x_mutex;
pthread_cond_t x_cond;
int *vetor;

void barreira(int nthreads){
    pthread_mutex_lock(&x_mutex); //inicio secao critica
    if(bloqueadas == (nthreads-1)){
        pthread_cond_broadcast(&x_cond);
        bloqueadas=0;
    }else{
        bloqueadas++;
        pthread_cond_wait(&x_cond,&x_mutex);
    }
    pthread_mutex_unlock(&x_mutex); //fim secao critica
}

void *tarefa (void *arg){
    int id = *(int*) arg;
    int *somatorio;
    somatorio = (int*) malloc(sizeof(int*));

    for(int passo=0;passo<PASSOS; passo++){
        if(PRINT) printf("Thread %d passo=%d\n", id,passo);
        if(passo == 0){
            for(int i=0; i<NTHREADS;i++){
                *somatorio += vetor[i];
            }
        }else{
            vetor[id] = rand() % 10;
        }
        
        barreira(NTHREADS);
    }

    printf("Thread %d somatorio=%d vetor=%d\n", id,*somatorio, vetor[id]);

    pthread_exit(somatorio);
}

int main(int argc, char *argv[]){
    pthread_t threads[NTHREADS];
    int id[NTHREADS];
    int *somatorioThread;
    int vetorSomatorioThreads[NTHREADS];

    somatorioThread = (int*) malloc(sizeof(int));

    //criando vetor de inteiros com tamanho nthreads
    vetor = (int *) malloc(sizeof(int)*NTHREADS); 
    if(vetor == NULL){
      fprintf(stderr, "Erro ao criar vetor -- malloc\n"); 
      return 2;
    }

    if(PRINT) printf("Valores do vetor:");
    srand(time(NULL)); //contribui para que os valores do vetor sejam diferentes a cada execução
    for(int i=0; i<NTHREADS; i++){
        vetor[i] = rand() % 10; //gerando valores aleatorios
        if(PRINT) printf("%d ", vetor[i]);
    }
    if(PRINT) printf("\n");

    pthread_mutex_init(&x_mutex, NULL);
    pthread_cond_init(&x_cond, NULL);

    //criando as threads
    for(int i=0; i<NTHREADS; i++){
        id[i]=i;
        pthread_create(&threads[i], NULL, tarefa, (void*)&id[i]);
    }
    
    //esperando as threads retornarem
    for(int i=0; i<NTHREADS; i++){
        pthread_join(threads[i],(void *)&somatorioThread);
        vetorSomatorioThreads[i] = *somatorioThread;
    }

    //printando o retornodo somatorio
    for(int i=0; i<NTHREADS; i++){
        printf(" %d ",vetorSomatorioThreads[i] );
    }

    //checando se tem diferenca
    for(int i=1;i<NTHREADS;i++){
        if(vetorSomatorioThreads[i-1] !=vetorSomatorioThreads[i]){
            printf("\nTEM ALGUM DIFERENTE AI HEIN :(\n");
            return 3;
        }
    }
    printf("\nSAO TODOS IGUAIS :)\n");

    printf("\nFIM\n");

    pthread_mutex_destroy(&x_mutex);
    pthread_cond_destroy(&x_cond);
    return 0;
}
 