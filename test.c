#include "libs/improved_distributed_select.c"
#include "libs/merge_and_select.c"
#include <mpi.h>
#include <stdio.h>
#include <time.h>

int main(int argc, char *argv[]) {
  MPI_Init(&argc, &argv);
  time_t start, stop;
  int result, i, j, k, l, world_rank;
  MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);

  char *types[2];
  asprintf(&types[0], "0_offset_100_std");
  asprintf(&types[1], "200_offset_100_std");

  int sizes[5] = {100, 200, 500, 1000, 2000};
  int ranks[3] = {100, 4, 2};

  int rounds = 100;
  int num_types = 2;
  int num_sizes = 5;
  int num_ranks = 3;

  FILE *dest_file;
  char *data_path;
  char *dest_path;

  srand(world_rank + time(0));
  for (i = 0; i < num_types; i++) {
    for (j = 0; j < num_sizes; j++) {

      asprintf(&data_path, "./data/%s/test_%d_points", types[i], sizes[j]);

      for (k = 0; k < num_ranks; k++) {

        if (world_rank == 0)
          asprintf(&dest_path, "./results/%s/test_%d_points/%d.txt", types[i],
                   sizes[j], ranks[k]);
        if (world_rank == 0)
          dest_file = fopen(dest_path, "w");

        for (l = 0; l < rounds; l++) {
          if (world_rank == 0)
            printf("running sim %d %s > %s #%d\n", i * num_sizes + j, data_path,
                   dest_path, k);
          if (world_rank == 0)
            start = clock();
          result = improved_distributed_select(
              data_path, 4 * sizes[j] / ranks[k], &argc, &argv);
          // result = merge_and_select("./data/base", rank, &argc, &argv);
          if (world_rank == 0)
            stop = clock();
          if (world_rank == 0)
            fprintf(dest_file, "%d %ld\n", result, stop - start);
        }

        if (world_rank == 0) {
          fclose(dest_file);
          free(dest_path);
        }
      }
      free(data_path);
    }
  }
  free(types[0]);
  free(types[1]);
  MPI_Finalize();
  return 0;
}
