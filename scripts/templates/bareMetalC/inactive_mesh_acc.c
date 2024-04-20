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

  unsigned long pre_start, start, end, pre_prebenchmark_cycles, benchmark_cycles;
  pre_start = read_cycles();

  elem_t A[DIM][DIM];
  
  uint32_t A_sp_addr = 0;

  int iterations = 1000;

  // Matrix Setup
  for(int i = 0; i < DIM; i++) {
    for(int j = 0; j < DIM; j++) {
      A[i][j] = 0;
    } 
  }

  start = read_cycles();
  pre_prebenchmark_cycles = start - pre_start;

  // Gemmini instructions start
  gemmini_flush(0);

  // Config Setup
  gemmini_config_ld(DIM);

  // Main microbenchmark code
  for(int i = 0; i < iterations; i++) {
    gemmini_extended_mvin(A, A_sp_addr, DIM, DIM);
  }

  gemmini_fence();
  // Gemmini instructions end

  end = read_cycles();
  benchmark_cycles = end - start;
  printf("Pre cycles taken: %u\n", pre_prebenchmark_cycles);
  printf("Cycles taken: %u\n", benchmark_cycles);

  exit(0);
}