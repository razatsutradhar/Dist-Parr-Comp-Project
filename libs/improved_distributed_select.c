#include <limits.h>
#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

// #define DEBUG
// #define BS_DEBUG

// recieve ranges and create a list
// while true
// select in range request in range
// request all possible lt and gt filling in known based of range list
int improved_distributed_select(char *data_path, int rank, int *argc,
                                char ***argv) {
  setbuf(stdout, NULL);
  //MPI_Init(argc, argv);
  int i, cont = 1; // loop itterator
  int k = 0;
  int l=1;
  int this_min, this_max, this_count; // min max of every local device
  int *data;                          // local data
  float error = 0;                      // error of guess
  int world_size,
      world_rank; // size is number of processes, rank is rank of each process
  MPI_Comm_size(MPI_COMM_WORLD, &world_size);
  MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);
  srand(time(0));

  int *all_min;      // gathers of all mins and maxes, dynamic array
  int *all_max;      // gathers of all mins and maxes, dynamic array
  int *all_lt_count; // gathers of less than counts, dynamic array
  int *all_gt_count;

  FILE *data_file;
  char *full_path;

  int ub = INT_MAX, lb = 0, possible, worker_select;
  // ub and lb are the upper and lower bounds of the range
  // possible is the number of possible workers fit the criteria
  // worker_select broadcasts the selected worker

  int hi, low, lb_rank, ub_rank, proposed = 0, all_lt, all_gt;
  // step is the step size for the binary search
  // lb_rank is the rank of the lower bound
  // ub_rank is the rank of the upper bound
  // proposed is the proposed rank

  //asprintf(&full_path, "%s/%d.txt", data_path, world_rank);
  full_path = malloc(sizeof(char) * 100);
  sprintf(full_path, "%s/%d.txt", data_path, world_rank);
  data_file = fopen(full_path, "r");
  free(full_path);
  // get count of lines
  data = calloc(200, sizeof(int));
  this_count = 0;
  while (fscanf(data_file, "%d", &data[this_count]) != EOF) {
    this_count++;
  }

  // allocate data
  // reset file
  rewind(data_file);
  // read data and store in data


  fclose(data_file);
  // print data

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
    all_gt_count = calloc(world_size, sizeof(int));
  }
  // gather all min and max
  MPI_Gather(&this_min, 1, MPI_INT, all_min, 1, MPI_INT, 0, MPI_COMM_WORLD);
  MPI_Gather(&this_max, 1, MPI_INT, all_max, 1, MPI_INT, 0, MPI_COMM_WORLD);
  MPI_Barrier(MPI_COMM_WORLD);
#ifdef DEBUG
  // print all min and max
  for (i = 0; i < world_size; i++) {
    if (world_rank == 0) {
      printf("min: %d max: %d\n\r", all_min[i], all_max[i]);
    }
  }
#endif
  while (cont>0) {
    
    //printf("loop %d\n", k++);
    // select a random possible worker based on ub and lb criteria
    if (world_rank == 0) {
      k++;
#ifdef DEBUG
      printf("\nub: %d lb: %d\n", ub, lb);
#endif
      // count possible
      possible = 0;
      for (i = 0; i < world_size; i++) {
        if (all_min[i] < ub && all_max[i] > lb)
          possible++;
      }
      //printf("possible: %d\n", possible);
      if (possible == 0) {
        cont = 0;
        break;
      }
      i = rand() % possible;
      worker_select = 0;
#ifdef DEBUG
      printf("chose %d out of %d possible\n", i, possible);
#endif
      while (i) {
        worker_select++;
        if (all_min[i] < ub && all_max[i] > lb) {
          i--;
        }
      }
#ifdef DEBUG
      printf("worker selected: %d\n", worker_select);
#endif
    }

    // Bcast worker_select so every process knows who the primary worker is
    MPI_Bcast(&worker_select, 1, MPI_INT, 0, MPI_COMM_WORLD);

    // Bcast lb and ub so every process knows the range
    MPI_Bcast(&lb, 1, MPI_INT, 0, MPI_COMM_WORLD);
    MPI_Bcast(&ub, 1, MPI_INT, 0, MPI_COMM_WORLD);
    MPI_Barrier(MPI_COMM_WORLD);
    // only worker select proposes the possible median
    if (world_rank == worker_select) {

      // find LB rank
      hi = this_count - 1;
      low = 0;
      while (hi >= low) {
        lb_rank = low + (hi - low) / 2;
#ifdef BS_DEBUG
        printf("lb low: %d mid: %d high: %d\n", low, lb_rank, hi);
#endif
        if (data[lb_rank] == lb)
          break;
        else if (data[lb_rank] > lb) {
          hi = lb_rank - 1;
        } else {
          low = lb_rank + 1;
        }
      }

      // find UB rank
      hi = this_count - 1;
      low = 0;
      while (hi >= low) {
        ub_rank = low + (hi - low) / 2;
#ifdef BS_DEBUG
        printf("ub low: %d mid: %d high: %d\n", low, ub_rank, hi);
#endif
        if (data[ub_rank] == ub)
          break;
        else if (data[ub_rank] > ub) {
          hi = ub_rank - 1;
        } else {
          low = ub_rank + 1;
        }
      }
      // get random number in the range
      //printf("lb_rank: %d ub_rank: %d\n", lb_rank, ub_rank);
      if (lb_rank == ub_rank) {
        proposed = data[lb_rank];
      } else {
        //proposed = data[lb_rank + (rand() % (ub_rank - lb_rank))];
        //proposed = data[(int)(lb_rank + ub_rank)/2];
        if(error<0){
          proposed = data[(int)((lb_rank + ub_rank)/2 - rand() % (int)((ub_rank - lb_rank)/4))];
        }else{
          proposed = data[(int)((lb_rank + ub_rank)/2 + rand() % (int)((ub_rank - lb_rank)/4))];
        }
      }
      // printf("proposed: %d\n", proposed);
#ifdef DEBUG
      printf("proposed: %d\n", proposed);
#endif
    }
    // Bcast selected
    MPI_Bcast(&proposed, 1, MPI_INT, worker_select, MPI_COMM_WORLD);
    MPI_Barrier(MPI_COMM_WORLD);
    hi = this_count - 1;
    low = 0;

    //printf("min: %d max: %d\n", this_min, this_max);
    if (proposed < this_min) {
      all_lt = 0;
      all_gt = this_count;
      //printf("proposed: %d this_min: %d\n", proposed, this_min);
    } else if (proposed > this_max) {
      all_gt = 0;
      all_lt = this_count;
      //printf("proposed: %d this_max: %d\n", proposed, this_max);
    }else{
      // find index of proposed or closest to proposed in data

    while (hi >= low) {
      all_lt = low + (hi - low) / 2;
  
#ifdef BS_DEBUG
      printf("rank low: %d mid: %d high: %d\n", low, all_lt, hi);
#endif
      //printf("index: %d all_lt: %d proposed: %d all_lt-1: %d\n",all_lt, data[all_lt], proposed, data[all_lt-1]);
      if (data[all_lt] == proposed) {
        while (data[all_lt] == proposed) {
          all_lt--;
        }
        break;
      } 
      if (data[all_lt] >= proposed && data[all_lt - 1] <= proposed) {
        hi = low-1;
      } else if (data[all_lt] < proposed) {
        low = all_lt + 1;
      } else if (data[all_lt] > proposed){
        hi = all_lt - 1;
      }
      all_gt = this_count - all_lt;
    }
    }
  
    hi = this_count - 1;
    low = 0;
    while (hi >= low) {
      all_gt = low + (hi - low) / 2;
      
      if(data[all_gt] == proposed){
        while (data[all_gt] == proposed) {
          all_gt++;
        }
        break;
      }

      if (data[all_gt] >= proposed && data[all_gt - 1] <= proposed) {
        break;
      } else if (data[all_gt] < proposed) {
        low = all_gt + 1;
      } else if (data[all_gt] > proposed){
        hi = all_gt - 1;
      }
    }
    all_gt = this_count - all_gt;
  
    

    //printf("all_lt: %d all_gt: %d\n", all_lt, all_gt);
    MPI_Gather(&all_lt, 1, MPI_INT, all_lt_count, 1, MPI_INT, 0,
               MPI_COMM_WORLD);
    MPI_Gather(&all_gt, 1, MPI_INT, all_gt_count, 1, MPI_INT, 0,
               MPI_COMM_WORLD);
    // wait for all processes to finish gathering
    MPI_Barrier(MPI_COMM_WORLD);
    if (world_rank == 0) {
      all_lt = 0;
      for (i = 0; i < world_size; i++) {
#ifdef DEBUG
        printf("lt at %d is %d\n", i, all_lt_count[i]);
#endif
        //printf("lt at %d is %d\n", i, all_lt_count[i]);
        all_lt += all_lt_count[i];
      }
      all_gt = 0;
      for (i = 0; i < world_size; i++) {
#ifdef DEBUG
        printf("gt at %d is %d\n", i, all_gt_count[i]);
#endif
        all_gt += all_gt_count[i];
      }
#ifdef DEBUG
      printf("%d %d %d\n", all_lt, rank, all_gt);
#endif
      //printf("all_lt: %d all_gt: %d\n", all_lt, all_gt);
      if (all_lt-rank<2 && all_lt-rank>-2) {
        cont = 0;
        //printf("loop %d\n", k);
      }else if (all_lt > rank) {
        ub = proposed;
      } else {
        lb = proposed;
      }
    }
    error = (rank-all_lt);
    MPI_Bcast(&cont, 1, MPI_INT, 0, MPI_COMM_WORLD);
    MPI_Barrier(MPI_COMM_WORLD);
    // printf("\n");
  }

  free(data);

  if (world_rank == 0) {
    free(all_min);
    free(all_max);
    free(all_lt_count);
    free(all_gt_count);

  }

  //MPI_Finalize();

  if (world_rank == 0) {
    return k;
  }else{
    return -1;
  }
  
}
