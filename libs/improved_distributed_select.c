#include <limits.h>
#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>

// recieve ranges and create a list
// while true
// select in range request in range
// request all possible lt and gt filling in known based of range list
int improved_distributed_select(char *data_path, int rank, int *argc,
                                char ***argv) {
  setbuf(stdout, NULL);
  MPI_Init(argc, argv);
  int i; // loop itterator

  int this_min, this_max, this_count; // min max of every local device
  int *data;                          // local data

  int world_size,
      world_rank; // size is number of processes, rank is rank of each process
  MPI_Comm_size(MPI_COMM_WORLD, &world_size);
  MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);

  int *all_min;      // gathers of all mins and maxes, dynamic array
  int *all_max;      // gathers of all mins and maxes, dynamic array
  int *all_lt_count; // gathers of less than counts, dynamic array

  FILE *data_file;
  char *full_path;

  int ub = INT_MAX, lb = 0, possible, worker_select;
  // ub and lb are the upper and lower bounds of the range
  // possible is the number of possible workers fit the criteria
  // worker_select broadcasts the selected worker

  int step, lb_rank, ub_rank, proposed = 0;
  // step is the step size for the binary search
  // lb_rank is the rank of the lower bound
  // ub_rank is the rank of the upper bound
  // proposed is the proposed rank

  asprintf(&full_path, "%s/%d", data_path, world_rank);
  data_file = fopen(full_path, "r");
  free(full_path);
  fscanf(data_file, "%d\n", &this_count);
  data = calloc(this_count, sizeof(int));
  printf("pid: %d length: %d\n\r", world_rank, this_count);
  for (i = 0; i < this_count; i++) {
    fscanf(data_file, "%d", &data[i]);
  }
  fclose(data_file);

  this_min = data[0];
  this_max = data[0];
  for (i = 1; i < this_count; i++) {
    if (data[i] < this_min)
      this_min = data[i];
    if (data[i] > this_max)
      this_max = data[i];
  }

  // request all min and max
  if (world_rank == 0) {
    all_min = calloc(world_size, sizeof(int));
    all_max = calloc(world_size, sizeof(int));
    all_lt_count = calloc(world_size, sizeof(int));
  }

  MPI_Gather(&this_min, 1, MPI_INT, all_min, world_size, MPI_INT, 0,
             MPI_COMM_WORLD);
  MPI_Gather(&this_max, 1, MPI_INT, all_max, world_size, MPI_INT, 0,
             MPI_COMM_WORLD);

  // while (cont) {
  if (world_rank == 0) {
    // count possible
    possible = 0;
    for (i = 0; i < world_size; i++) {
      if (all_min[i] < ub || all_max[i] > lb)
        possible++;
    }
    i = rand() % possible;
    worker_select = 0;

    while (i) {
      worker_select++;
      if (all_min[i] < ub || all_max[i] > lb) {
        i--;
      }
    }
  }

  MPI_Bcast(&worker_select, 1, MPI_INT, 0, MPI_COMM_WORLD);
  MPI_Bcast(&lb, 1, MPI_INT, 0, MPI_COMM_WORLD);
  MPI_Bcast(&ub, 1, MPI_INT, 0, MPI_COMM_WORLD);

  if (world_rank == worker_select) {
    // find LB rank
    lb_rank = this_count / 2;
    step = this_count / 4;
    while (step) {
      printf("%d %d\n", lb_rank, step);
      if (data[lb_rank] == lb)
        break;
      else if (data[lb_rank] > lb) {
        lb_rank -= step;
      } else {
        lb_rank += step;
      }
      step /= 2;
    }

    // find UB rank
    ub_rank = this_count / 2;
    step = this_count / 4;
    while (step) {
      if (data[ub_rank] == ub)
        break;
      else if (data[ub_rank] < ub) {
        ub_rank += step;
      } else {
        ub_rank -= step;
      }
      step /= 2;
    }
    // rand between
    proposed = lb_rank + (rand() % (ub_rank - lb_rank));
  }
  // Bcast selected
  // MPI_Bcast(&proposed, 1, MPI_INT, worker_select, MPI_COMM_WORLD);

  /*
   proposed_rank = this_data_stats.count / 2;
   step = this_data_stats.count / 4;
   while (step) {
     if (data[proposed_rank] == proposed) {
       break;
     } else if (data[proposed_rank] < proposed) {
       proposed_rank += step;
     } else {
       proposed_rank -= step;
     }
     step /= 2;
   }
   MPI_Gather(&proposed_rank, 1, MPI_INT, proposed_ranks, world_size, MPI_INT,
 0, MPI_COMM_WORLD);

   if (world_rank == 0) {
     proposed_rank = 0;
     for (i = 0; i < world_size; i++) {
       proposed_rank += proposed_ranks[i];
     }
     if (proposed_rank == rank) {
       cont = 0;
     } else if (proposed_rank > rank) {
       UB = proposed;
     } else {
       LB = proposed;
     }
   }
   MPI_Bcast(&cont, 1, MPI_INT, 0, MPI_COMM_WORLD);
 }
 */

  free(data);

  if (world_rank == 0) {
    free(all_min);
    free(all_max);
    free(all_lt_count);
  }

  if (world_rank == 0) {
    printf("rank: %d value: %d\n\r", world_rank, rank);
  }
  MPI_Barrier(MPI_COMM_WORLD);
  MPI_Finalize();
  return rank;
}
