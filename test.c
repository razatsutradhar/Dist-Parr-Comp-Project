#include "libs/improved_distributed_select.c"
#include "libs/merge_and_select.c"
#include <mpi.h>
#include <stdio.h>
#include <time.h>

int main(int argc, char *argv[]) {
  MPI_Init(&argc, &argv);
  time_t start, stop;
  int result, i, j, world_rank;
  MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);

  int sims = 6;
  FILE *b1, *m1, *b100, *m100, *b200, *m200;
  if (world_rank == 0) {
    b1 = fopen("./results/b1.txt", "w");
    m1 = fopen("./results/m1.txt", "w");
    b100 = fopen("./results/b100.txt", "w");
    m100 = fopen("./results/m100.txt", "w");
    b200 = fopen("./results/b200.txt", "w");
    m200 = fopen("./results/m200.txt", "w");
  }
  FILE *files[6] = {b1, m1, b100, m100, b200, m200};
  int rounds[6] = {100, 100, 100, 100, 100, 100};
  char *b, *m;
  asprintf(&b, "./data/base");
  asprintf(&m, "./data/same_mean");
  char *data[6] = {b, m, b, m, b, m};
  int rank[6] = {1, 1, 100, 100, 200, 200};
  srand(world_rank + time(0));
  for (j = 0; j < sims; j++)
    for (i = 0; i < rounds[j]; i++) {
      if (world_rank == 0)
        printf("running sim %d test %d looking for %dwith data '%s'\n", j, i,
               rank[j], data[j]);
      if (world_rank == 0)
        start = clock();
      result = improved_distributed_select(data[j], rank[j], &argc, &argv);

      // result = merge_and_select("./data/base", rank, &argc, &argv);
      if (world_rank == 0)
        stop = clock();

      if (world_rank == 0)
        fprintf(files[j], "%d %ld\n", result, stop - start);
    }

  if (world_rank == 0) {
    fclose(b1);
    fclose(m1);
    fclose(b100);
    fclose(m100);
    fclose(b200);
    fclose(m200);
  }
  free(b);
  free(m);
  MPI_Finalize();
  return 0;
}
