// See LICENSE for license details.

#include <stdint.h>
#include <stddef.h>
#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#ifndef BAREMETAL
#include <sys/mman.h>
#endif
#include "include/gemmini_testutils.h"

int main() {
#ifndef BAREMETAL
    if (mlockall(MCL_CURRENT | MCL_FUTURE) != 0) {
      perror("mlockall failed");
      exit(1);
    }
#endif

  elem_t C[DIM][DIM];
  
  uint32_t C_sp_addr = 1 << 31;

  int iterations = 1000;

  unsigned long start, end, benchmark_cycles;
  start = read_cycles();

  // Gemmini instructions start
  gemmini_flush(0);

  // Config Setup
  gemmini_config_st(DIM);

  // Main microbenchmark code
  for(int i = 0; i < iterations; i++) {
    gemmini_extended_mvout(C, C_sp_addr, DIM, DIM);
  }

  gemmini_fence();
  // Gemmini instructions end

  end = read_cycles();
  benchmark_cycles = end - start;
  printf("Cycles taken: %u\n", benchmark_cycles);

  exit(0);
}