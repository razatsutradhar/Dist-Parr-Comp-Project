#include "libs/improved_distributed_select.c"
#include "libs/merge_and_select.c"
#include <stdio.h>

int main(int argc, char *argv[]) {
  int result, rank = 100;
  result = improved_distributed_select("./data/base", rank, &argc, &argv);
  // result = merge_and_select("./data/base", rank, &argc, &argv);

  printf("data[%d] is %d", rank, result);
  return 0;
}
