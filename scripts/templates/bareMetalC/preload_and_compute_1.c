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

  elem_t A[DIM][DIM];
  elem_t B[DIM][DIM];
  elem_t C[DIM][DIM];

  uint32_t A_sp_addr = 0;
  uint32_t B_sp_addr = DIM * 2;
  uint32_t C_sp_addr = 1 << 31;

  int iterations = 1000;

  unsigned long start, end, benchmark_cycles;
  start = read_cycles();
  
  // Matrix Setup
  for(int i = 0; i < DIM; i++) {
    for(int j = 0; j < DIM; j++) {
      A[i][j] = 1;
      B[i][j] = i == j;
    } 
  }

  // Gemmini instructions start
  gemmini_flush(0);

  // Config Setup
  gemmini_config_ld(DIM * sizeof(elem_t));
  gemmini_config_ex(WS, NO_ACTIVATION, 0);

  // Move in matrices for initialization
  gemmini_extended_mvin(A, A_sp_addr, DIM, DIM);
  gemmini_extended_mvin(B, B_sp_addr, DIM, DIM);

  // Main microbenchmark code
  for(int i = 0; i < iterations; i++) {
    gemmini_extended_preload(B_sp_addr, C_sp_addr, 16, 16, 16, 16);
    gemmini_extended_compute_preloaded(A_sp_addr, GARBAGE_ADDR, 16, 16, 16, 16);
  }

  gemmini_fence();
  // Gemmini instructions end

  end = read_cycles();
  benchmark_cycles = end - start;
  printf("Cycles taken: %u\n", benchmark_cycles);

  exit(0);
}