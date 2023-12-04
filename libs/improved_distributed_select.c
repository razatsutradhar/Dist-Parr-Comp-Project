#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_LINE_LENGTH 1000
#define VALUES_PER_LINE 100
typedef struct request {
  int type;
  int UB;
  int LB;
  int NUM;
} request;

/*
void get_data(char *data_path, int world_rank, int *data) {
  FILE *data_file;
  sprintf(data_path, "%s/%d", data_path, world_rank);
  data_file = fopen(data_path, "r");
  int N = getw(data_file);
  data = calloc(N, sizeof(int));
  for (int i = 0; i < N; i++) {
    data[i] = getw(data_file);
  }
  fclose(data_file);
}
*/

void get_data(const char *file_path, int line_number, int *values) {
    FILE *file = fopen(file_path, "r");
    if (file == NULL) {
        perror("Error opening file");
        return;
    }

    char line[MAX_LINE_LENGTH];
    int current_line = 0;

    while (fgets(line, sizeof(line), file) != NULL) {
        if (current_line == line_number - 1) {
            char *token = strtok(line, ",");
            int i = 0;
            while (token != NULL && i < VALUES_PER_LINE) {
                values[i++] = atoi(token);
                token = strtok(NULL, ",");
            }
            break;
        }
        current_line++;
    }

    fclose(file);
}

// send range
// on request send a random in range number
// on request send the count lt and gt
void worker(char *data_path) {
  int i, N, choice, response, size;
  int *data;
  int world_rank;
  MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);
  
  // get_data(&N, data, data_path, world_rank);
  get_data(data_path, world_rank, data);

  MPI_Comm_size(MPI_COMM_WORLD, &size);
  struct request req;
  while (1) {
    MPI_Recv(&req, 4, MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    if (req.type == 0)
      break;
    if (req.type == 1) {
      response = req.LB + rand() % (req.UB - req.LB);
      MPI_Bcast(&response, 1, MPI_INT, world_rank, MPI_COMM_WORLD);
    }
    if (req.type == 2) {
      int rank = N / 2;
      int step = N / 4;
      while (step) {
        if (data[rank] == req.NUM)
          break;
        else if (data[rank] < req.NUM)
          rank += step;
        else
          rank -= step;
        step /= 2;
      }
      MPI_Gather(&rank, size - 1, MPI_INT, NULL, 1, MPI_INT, 0, MPI_COMM_WORLD);
    }
  }
  free(data);
}

typedef struct {
  int max;
  int min;
  int count;
} data_stats_t;

// recieve ranges and create a list
// while true
// select in range request in range
// request all possible lt and gt filling in known based of range list
void central() {
  int UB, LB, world_size, world_rank;
  data_stats_t this_data_stats;
  data_stats_t *data_stats;
  MPI_Datatype *data_stats_mpi_t;
  MPI_Type_contiguous(world_size, MPI_INT, data_stats_mpi_t);
  MPI_Comm_size(MPI_COMM_WORLD, &world_size);
  MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);
  recieve s
      // request all min and max
      if (world_rank == 0) {
    data_stats = calloc(size - 1, sizeof(data_stats_t));
  }
  MPI_Gatherv(&this_data_stats, 1, data_stats_mpi_t, data_stats, world_size, );
}

void improved_distributed_select(char *data_path) {
  MPI_Init(NULL, NULL);
  int rank, selected;
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);
  sprintf(data_path, "%s/%d", data_path, rank);
  if (rank == 0) {
    central();
  } else {
    worker(data_path);
  }
}
