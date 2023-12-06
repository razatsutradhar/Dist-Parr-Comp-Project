#include <limits.h>
#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
int merge_and_select(char *data_path, int rank, int *argc, char ***argv) {
  setbuf(stdout, NULL);
  // MPI_Init(argc, argv);
  int i, j;
  int world_size,
      world_rank; // size is number of processes, rank is rank of each process
  MPI_Comm_size(MPI_COMM_WORLD, &world_size);
  MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);
  FILE *data_file;
  char *full_path;
  int this_count, min;
  int *data;
  int *all_counts;
  int all_count;
  int *all_data;
  int *displs;
  int *merged_data;
  int answer;

  asprintf(&full_path, "%s/%d.txt", data_path, world_rank);
  data_file = fopen(full_path, "r");
  free(full_path);
  fscanf(data_file, "%d\n", &this_count);
  data = calloc(this_count, sizeof(int));
  for (i = 0; i < this_count; i++) {
    fscanf(data_file, "%d", &data[i]);
  }
  fclose(data_file);

  if (world_rank == 0) {
    all_counts = calloc(world_size, sizeof(int));
    displs = calloc(world_size, sizeof(int));
  }
  MPI_Gather(&this_count, 1, MPI_INT, all_counts, 1, MPI_INT, 0,
             MPI_COMM_WORLD);

  if (world_rank == 0) {
    all_count = 0;
    for (i = 0; i < world_size; i++) {
      displs[i] = all_count;
      all_count += all_counts[i];
    }

    all_data = calloc(all_count, sizeof(int));
    merged_data = calloc(all_count, sizeof(int));
  }

  MPI_Gatherv(data, this_count, MPI_INT, all_data, all_counts, displs, MPI_INT,
              0, MPI_COMM_WORLD);

  if (world_rank == 0) {
    for (i = 0; i < world_size; i++) {
      all_counts[i] += displs[i];
    }

    i = 0;
    while (i < all_count) {
      min = -1;
      for (j = 0; j < world_size; j++) {
        printf("pointer %d max %d", displs[j], all_counts[j]);
        if (displs[j] < all_counts[j]) {
          if (min == -1) {
            min = j;
          } else if (all_data[displs[j]] < all_data[displs[min]]) {
            min = j;
          }
        }
        printf("\n");
      }
      printf("i %d equals %d from %d\n", i, displs[min], min);
      merged_data[i] = all_data[displs[min]];
      displs[min]++;
      i++;
    }
  }

  if (world_rank == 0) {
    for (i = 0; i < all_count; i++) {
      printf("%d %d\n", i, merged_data[i]);
    }
    printf("\n");

    if (rank >= 0 && rank < all_count) {
      answer = merged_data[rank];
    } else {
      answer = -1;
    }
  }
  MPI_Bcast(&answer, 1, MPI_INT, 0, MPI_COMM_WORLD);

  free(data);
  if (world_rank == 0) {
    free(all_data);
    free(all_counts);
    free(displs);
    free(merged_data);
  }

  // MPI_Finalize();
  return answer;
}
