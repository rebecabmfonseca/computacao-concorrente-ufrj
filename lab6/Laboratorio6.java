// Contando numero de elementos pares em um vetor

class S {
    // recurso compartilhado
    private int[] vetor;
    private int contPar;

    // construtor
    public S(int[] vetor) {
        this.vetor = vetor;
        this.contPar = 0;
    }

    public synchronized void check(int id) {
        //System.out.println("check= "+id);
        if (vetor[id] % 2 == 0) {
            this.contPar++;
        }
    }

    public synchronized int get() {
        return this.contPar;
    }

}

// classe que estende Thread e implementa a tarefa de cada thread do programa
class T extends Thread {
    // identificador da thread
    private int id;
    // objeto compartilhado com outras threads
    S s;

    private int inicio;
    private int fim;

    // construtor
    public T(int tid, S s, int inicio, int fim) {
        this.id = tid;
        this.s = s;
        this.inicio = inicio;
        this.fim = fim;
    }

    // metodo main da thread
    public void run() {
        System.out.println("Thread " + this.id + " iniciou!");
        for (int i = this.inicio; i < this.fim; i++) {
            this.s.check(i);
        }
        System.out.println("Thread " + this.id + " terminou!");
    }
}

// classe da aplicacao
class Laboratorio6 {
    static final int NThreads = 15;
    static final int NVetor = 55000000;
    static int tamBloco = 0;
    static int numElementosPares = 0;
    static int vetor[] = new int[NVetor];

    public static void main(String[] args) {

        // inicializando vetor
        for (int i = 0; i < NVetor; i++) {
            vetor[i] = i;
        }

        // reserva espaço para um vetor de threads
        Thread[] threads = new Thread[NThreads];

        // cria uma instancia do recurso compartilhado entre as threads
        S s = new S(vetor);
        //separa o tamanho do bloco para cada thread ler
        tamBloco = NVetor / NThreads;
        // cria as threads 
        for (int i = 0; i < threads.length; i++) {
            int inicial = i * tamBloco;
            int fim = inicial + tamBloco;
            if (i == NThreads - 1) fim = NVetor;
            threads[i] = new T(i, s, inicial, fim);
        }

        // inicia as threads
        for (int i = 0; i < threads.length; i++) {
            threads[i].start();
        }

        // espera pelo termino de todas as threads
        for (int i = 0; i < threads.length; i++) {
            try {
                threads[i].join();
            } catch (InterruptedException e) {
                return;
            }
        }

        //verifica valores de forma sequencial
        for (int i = 0; i < NVetor; i++) {
            if(vetor[i]%2 == 0){
                numElementosPares++;
            }
        }
        //verifica se o resultado da versao sequencia eh igual a concorrente
        if(numElementosPares!=s.get()){
            System.out.println("Versão sequencial e concorrente diferentes :(");    
        }else{
            System.out.println("Tudo certo :)");    
        }

        System.out.println("Quantidade de elementos pares: " + s.get());
    }
}
