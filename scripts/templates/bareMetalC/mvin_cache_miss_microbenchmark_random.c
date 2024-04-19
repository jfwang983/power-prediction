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

uint32_t rand_addr(uint32_t A) {
  uint8_t byte_0 = rand();
  uint8_t byte_1 = rand();
  uint8_t byte_2 = rand();
  // nibble_7 cannot be 0x0
  // uint8_t nibble_6 = 0x0F & rand();
  // uint8_t nibble_7 = 0x10 & rand();
  // uint8_t byte_3 = (nibble_7 == 0x00) ? (0x10 | nibble_6) : (nibble_7 | nibble_6);
  uint32_t byte_3 = A & 0xFF000000;

  printf("A: %x\t", A);
  printf("byte_3: %x\n", byte_3);

  // Construct address from bytes
  uint32_t addr = byte_3 | (byte_2 << 16) | (byte_1 << 8) | byte_0;
  return 0;
}

int main() {
#ifndef BAREMETAL
    if (mlockall(MCL_CURRENT | MCL_FUTURE) != 0) {
      perror("mlockall failed");
      exit(1);
    }
#endif

  elem_t A[DIM][DIM];
  elem_t B[DIM][DIM];
  
  uint32_t A_sp_addr = 0;
  uint32_t B_sp_addr = DIM + DIM;
  uint32_t C_sp_addr = 1 << 31;

  int iterations = 1000;

  // Matrix Setup
  for(int i = 0; i < DIM; i++) {
    for(int j = 0; j < DIM; j++) {
      A[i][j] = 0;
      B[i][j] = i == j;
    } 
  }

  unsigned long start, end, benchmark_cycles;
  start = read_cycles();

  // Gemmini instructions start
  gemmini_flush(0);

  // Config Setup
  gemmini_config_ld(DIM);
  gemmini_config_ex(WS, NO_ACTIVATION, 0);

  // Move in matrices for initialization
  gemmini_extended_mvin(A, A_sp_addr, DIM, DIM);
  gemmini_extended_mvin(B, B_sp_addr, DIM, DIM);
  
  // Initialize mesh to be all 0
  gemmini_extended_preload(B_sp_addr, C_sp_addr, 16, 16, 16, 16);
  gemmini_extended_compute_preloaded(A_sp_addr, GARBAGE_ADDR, 16, 16, 16, 16);

  // Main microbenchmark code
  for(int i = 0; i < iterations; i++) {
    gemmini_extended_mvin(A + (i * 0x1000), A_sp_addr, DIM, DIM);
  }

  gemmini_fence();
  // Gemmini instructions end

  end = read_cycles();
  benchmark_cycles = end - start;
  printf("Cycles taken: %u\n", benchmark_cycles);

  exit(0);
}