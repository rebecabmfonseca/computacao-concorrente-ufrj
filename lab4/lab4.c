#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>

#define NTHREADS  4
#define PRINT 0 //ajuda pra printar

int x = 0;
pthread_mutex_t x_mutex;
pthread_cond_t cond_t1_t4;
pthread_cond_t cond_t3;


/* Thread 1 */
void *thread1 (void *t) {
  if(PRINT) printf("thread1: Comecei\n");

  pthread_mutex_lock(&x_mutex);
  while(x == 0) { 
     if(PRINT) printf("thread1: x= %d, vai se bloquear...\n", x);
     pthread_cond_wait(&cond_t1_t4, &x_mutex);
     if(PRINT) printf("thread1: sinal recebido e mutex realocado, x = %d\n", x);
  }
  x++;
  printf("Fique a vontade.\n");
  pthread_cond_signal(&cond_t3);
  pthread_mutex_unlock(&x_mutex); 
  pthread_exit(NULL);
}

/* Thread 2 */
void *thread2 (void *t) {
  if(PRINT) printf("thread2: Comecei\n");

  pthread_mutex_lock(&x_mutex);
  x++;
  printf("Seja bem vindo!\n");
  pthread_cond_broadcast(&cond_t1_t4);
  pthread_mutex_unlock(&x_mutex); 
  pthread_exit(NULL);
}

/* Thread 3 */
void *thread3 (void *t) {
  if(PRINT) printf("thread3: Comecei\n");

  pthread_mutex_lock(&x_mutex);
  while (x < 3) { 
     if(PRINT) printf("thread3: x= %d, vai se bloquear...\n", x);
     pthread_cond_wait(&cond_t3, &x_mutex);
     if(PRINT) printf("thread3: sinal recebido e mutex realocado, x = %d\n", x);
  }
  printf("Volte sempre!\n");
  pthread_mutex_unlock(&x_mutex); 
  pthread_exit(NULL);
}

/* Thread 4 */
void *thread4 (void *t) {
  if(PRINT) printf("thread4: Comecei\n");

  pthread_mutex_lock(&x_mutex);
  while(x == 0) { 
     if(PRINT) printf("thread4: x= %d, vai se bloquear...\n", x);
     pthread_cond_wait(&cond_t1_t4, &x_mutex);
     if(PRINT) printf("thread4: sinal recebido e mutex realocado, x = %d\n", x);
  }
  x++;
  printf("Sente-se por favor.\n");
  pthread_cond_signal(&cond_t3);
  pthread_mutex_unlock(&x_mutex); 
  pthread_exit(NULL);
}

/* Funcao principal */
int main(int argc, char *argv[]) {
  int i; 
  pthread_t threads[NTHREADS];

  pthread_mutex_init(&x_mutex, NULL);
  pthread_cond_init (&cond_t1_t4, NULL);
  pthread_cond_init (&cond_t3, NULL);

  pthread_create(&threads[3], NULL, thread4, NULL);
  pthread_create(&threads[2], NULL, thread3, NULL);
  pthread_create(&threads[1], NULL, thread2, NULL);
  pthread_create(&threads[0], NULL, thread1, NULL);

  for (i = 0; i < NTHREADS; i++) {
    pthread_join(threads[i], NULL);
  }
  if(PRINT) printf ("\nFIM\n");

  pthread_mutex_destroy(&x_mutex);
  pthread_cond_destroy(&cond_t1_t4);
  pthread_cond_destroy(&cond_t3);
}
