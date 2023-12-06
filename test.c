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

  int sims = 8;
  char *b, *m;
  char *offset0, *offset100;
  asprintf(&b, "./data/base");
  asprintf(&m, "./data/same_mean");
  asprintf(&offset0, "./data/0_offset_100_std/test_2000_points");
  asprintf(&offset100, "./data/200_offset_100_std/test_2000_points");
  char *data[12] = {b,       m,         b,       m,         b,       m,
                    offset0, offset100, offset0, offset100, offset0, offset100};
  int rank[12] = {1, 1, 100, 100, 200, 200, 1, 1, 2000, 2000, 4000, 4000};
  int rounds[12] = {100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100};
  FILE *files[12];
  if (world_rank == 0) {
    files[0] = fopen("./results/b1.txt", "w");
    files[1] = fopen("./results/m1.txt", "w");
    files[2] = fopen("./results/b100.txt", "w");
    files[3] = fopen("./results/m100.txt", "w");
    files[4] = fopen("./results/b200.txt", "w");
    files[5] = fopen("./results/m200.txt", "w");
    files[6] = fopen("./results/0o20001.txt", "w");
    files[7] = fopen("./results/100o20001.txt", "w");
    files[8] = fopen("./results/0o20002000.txt", "w");
    files[9] = fopen("./results/100o20002000.txt", "w");
    files[10] = fopen("./results/0o20004000.txt", "w");
    files[11] = fopen("./results/100o20004000.txt", "w");
  }

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
    for (i = 0; i < sims; i++)
      fclose(files[i]);
  }
  free(b);
  free(m);
  MPI_Finalize();
  return 0;
}
