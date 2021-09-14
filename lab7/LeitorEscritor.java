
class LE {
  private int leit, escr, lescr;  
  int variavel;
  
  LE() { 
     this.leit = 0; 
     this.escr = 0;
     this.lescr = 0;
     this.variavel = 0; 
  } 
  
  public synchronized void EntraLeitor (int id) {
    try { 
      while (this.escr > 0 || this.lescr > 0) {
      //if (this.escr > 0 || this.lescr > 0) {
         //System.out.println ("le.leitorBloqueado("+id+")");
         wait();  
      }
      this.leit++; 
      //System.out.println ("le.leitorLendo("+id+")");
    } catch (InterruptedException e) {return; }
  }
  
  public synchronized void SaiLeitor (int id) {
     this.leit--; 
     if (this.leit == 0) 
           this.notifyAll(); 
     //System.out.println ("le.leitorSaindo("+id+") " + this.leit + " " + this.escr +" "+ this.lescr);
  }
  
  public synchronized void EntraEscritor (int id) {
    try { 
      //while((this.leit > 0) || (this.escr > 0) || (this.lescr > 0)) {
      if ((this.leit > 0) || (this.escr > 0) || (this.lescr > 0)) {
         //System.out.println ("le.escritorBloqueado("+id+")");
         wait();  
      }
      this.escr++; 
      //System.out.println ("le.escritorEscrevendo("+id+")");
    } catch (InterruptedException e) {return; }
  }
  
  public synchronized void SaiEscritor (int id) {
     this.escr--;
     this.notifyAll();
     //System.out.println ("le.escritorSaindo("+id+")");
  }
  
  public synchronized void EntraLescritor (int id) {
    try { 
      if ((this.leit > 0) || (this.escr > 0) || (this.lescr > 0)) {
         wait(); 
      }
      this.lescr++; 
      //System.out.println ("le.lescritorEscrevendo("+id+")");
    } catch (InterruptedException e) {return; }
  }
  
  public synchronized void SaiLescritor (int id) {
     this.lescr--; 
     this.notifyAll(); 
     //System.out.println ("le.lescritorSaindo("+id+")");
  }
}


class Leitor extends Thread {
  int id;
  LE monitor;
  boolean prim = true;

  Leitor (int id, LE m) {
    this.id = id;
    this.monitor = m;
  }

  public void run () {
    try {
        for(;;){
            this.monitor.EntraLeitor(this.id);
            for(int i = 2; i<this.monitor.variavel;i++){
                if( this.monitor.variavel % i == 0){
                    prim = false;
                    break;
                }
            }
            
            System.out.println("L("+this.id+")["+this.monitor.variavel + "] é primo ? "+prim);
            this.monitor.SaiLeitor(this.id);
            sleep(3000);
        }
      }catch (Exception e) { return; }
  }
}

class Escritor extends Thread {
  int id; 
  LE monitor; 

  Escritor (int id, LE m) {
    this.id = id;
    this.monitor = m;
  }

  public void run () {
    try {
        for(;;){
            this.monitor.EntraEscritor(this.id);
            this.monitor.variavel = this.id;
            this.monitor.SaiEscritor(this.id);
            sleep(3000);
        }
    } catch (Exception e) { return; }
  }
}

class Lescritor extends Thread {
  int id; 
  LE monitor;
  boolean par = true;

  
  Lescritor (int id, LE m) {
    this.id = id;
    this.monitor = m;
  }

  public void run () {
    try { 
        for(;;){
            this.monitor.EntraLeitor(this.id);
            if(this.monitor.variavel %2 == 0) par = true;
            else par = false; 
            System.out.println("LE("+this.id+")["+this.monitor.variavel+"] é par ? "+par);
            this.monitor.SaiLeitor(this.id);
            this.monitor.EntraLescritor(this.id);
            this.monitor.variavel = this.monitor.variavel*2;
            this.monitor.SaiLescritor(this.id);
            sleep(3000);
        }
    } catch (Exception e) { return; }
  }
}

class LeitorEscritor {
  static final int L = 7;
  static final int E = 7;
  static final int LEC = 8;

  public static void main (String[] args) {
    int i;
    LE monitor = new LE();            // Monitor (objeto compartilhado entre leitores e escritores)
    Leitor[] l = new Leitor[L];       // Threads leitores
    Escritor[] e = new Escritor[E];   // Threads escritores
    Lescritor[] les = new Lescritor[LEC]; //thread de escritores/leitores;
    
    for (i=0; i<E; i++) {
       e[i] = new Escritor(i+1, monitor);
       e[i].start(); 
   
    }
     for (i=0; i<LEC; i++) {
       les[i] = new Lescritor(i+1, monitor);
       les[i].start(); 
    }
    for (i=0; i<L; i++) {
       l[i] = new Leitor(i+1, monitor);
       l[i].start(); 
    } 
    
  }
}