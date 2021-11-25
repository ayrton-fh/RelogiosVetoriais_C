/*
 * Código base (incompleto) para implementação de relógios vetoriais.
 * Meta: implementar a interação entre três processos ilustrada na figura
 * da URL: 
 * 
 * https://people.cs.rutgers.edu/~pxk/417/notes/images/clocks-vector.png
 * 
 * Compilação: mpicc -o rvet rvet.c
 * Execução:   mpiexec -n 3 ./rvet
 */
 
#include <stdio.h>
#include <string.h>  
#include <mpi.h> 

const int CLOCK_TAM = 3;

typedef struct Clock { 
  int p[3];
} Clock;

void Event(int pid, Clock *clock) {
  clock->p[pid]++;   
  printf("Process: %d, Clock: (%d, %d, %d)\n", pid, clock->p[0], clock->p[1], clock->p[2]);
}

void Send(int pidE, int pidR, Clock *clock) {
  // Alterar relógio
  // Event(pidE, &clock);
  clock->p[pidE]++;

  // Vetor auxiliar
  int clockAux[CLOCK_TAM];
  for (int i = 0; i < CLOCK_TAM; i++) clockAux[i] = clock->p[i];

  // Utilizar MPI
  MPI_Send(clockAux, CLOCK_TAM, MPI_INT, pidR, pidE, MPI_COMM_WORLD);

  // Exibir alterações
  printf("Process: %d, Clock: (%d, %d, %d)\n", pidE, clock->p[0], clock->p[1], clock->p[2]);
}

void Receive(int pidE, int pidR, Clock *clockR) {
  // Alterar o relógio
  // Event(pidR, &clockR);
  clockR->p[pidR]++;

  // Utilizar MPI
  int clockE[3];
  MPI_Recv(clockE, CLOCK_TAM, MPI_INT, pidE, pidE, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

  // Analisar e atualizar de acordo com o recebido
  for (int i = 0; i < CLOCK_TAM; i++) {
    if (clockR->p[i] < clockE[i]) clockR->p[i] = clockE[i];
  }

  printf("Process: %d, Clock: (%d, %d, %d)\n", pidR, clockR->p[0], clockR->p[1], clockR->p[2]);
}

// Representa o processo de rank 0
void process0() {
  Clock clock = {{0,0,0}};
  
  // (1, 0, 0)
  Event(0, &clock);
  
  // (2, 0, 0) 
  Send(0, 1, &clock);

  // (3, 1, 0)
  Receive(1, 0, &clock);
  
  // (4, 1, 0)
  Send(0, 2, &clock);
  
  // (5, 1, 2)
  Receive(2, 0, &clock);
  
  // (6, 1, 2)
  Send(0, 1, &clock);
  
  // (7, 1, 2)
  Event(0, &clock);
}

// Representa o processo de rank 1
void process1() {
  Clock clock = {{0,0,0}};
  printf("Process: %d, Clock: (%d, %d, %d)\n", 1, clock.p[0], clock.p[1], clock.p[2]);
  
  // (0, 1, 0)
  Send(1, 0, &clock);

  // (2, 2, 0)
  Receive(0, 1, &clock);

  // (6, 3, 2)
  Receive(0, 1, &clock);
}

// Representa o processo de rank 2
void process2() {
  Clock clock = {{0,0,0}};
  printf("Process: %d, Clock: (%d, %d, %d)\n", 2, clock.p[0], clock.p[1], clock.p[2]);
  
  // (0, 0, 1)
  Event(2, &clock);

  // (0, 0, 2)
  Send(2, 0, &clock);

  // (4, 1, 3)
  Receive(0, 2, &clock);
}

int main(void) {
  int my_rank;               

  MPI_Init(NULL, NULL); 
  MPI_Comm_rank(MPI_COMM_WORLD, &my_rank); 

  if (my_rank == 0) { 
    process0();
  } 
  
  else if (my_rank == 1) {  
    process1();
  } 
  
  else if (my_rank == 2) {  
    process2();
  }

  // Finaliza MPI
  MPI_Finalize(); 

  return 0;
}  /* main */
