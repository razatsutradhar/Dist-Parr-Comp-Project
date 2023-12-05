#include "libs/improved_distributed_select.c"
#include <stdio.h>

int main(int argc, char *argv[]) {
  int result;
  result = improved_distributed_select("./data/base", 15, &argc, &argv);
  return 0;
}
