#include "libs/improved_distributed_select.c"
#include <stdio.h>
#include <mpi.h>


// RUN for i in {0..200}; do mpirun -np 4 output; done in terminal

int main(int argc, char *argv[]) {
  int rank = 200;
  int res;
  MPI_Init(&argc, &argv);
  int k = 0;
  
  res = improved_distributed_select("./data/SameMedian", rank, &argc, &argv);

  if(res != -1){
    printf("Loops taken: %d\n", res+1);
    // append to file
    FILE *fp;
    fp = fopen("./data/base/loops.txt", "a");
    fprintf(fp, "%d\n", res);
    fclose(fp);
  }
  MPI_Finalize();
}