#include "libs/improved_distributed_select.c"
#include "libs/merge_and_select.c"
#include <mpi.h>
#include <stdio.h>
#include <time.h>

int main(int argc, char *argv[]) {
  MPI_Init(&argc, &argv);
  time_t start, stop;
  int result, i, world_rank, rank = 200, rounds = 100;
  MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);
  srand(world_rank + time(0));
  for (i = 0; i < rounds; i++) {
    if (world_rank == 0)
      start = clock();
    result =
        improved_distributed_select("./data/same_mean", rank, &argc, &argv);

    // result = merge_and_select("./data/base", rank, &argc, &argv);
    if (world_rank == 0)
      stop = clock();

    if (world_rank == 0)
      printf("%d %ld\n", result, stop - start);
  }
  MPI_Finalize();
  return 0;
}
