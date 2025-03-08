// See LICENSE for license details.

#include <stdint.h>
#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>
#ifndef BAREMETAL
#include <sys/mman.h>
#endif
#include "matmul_funcs.h"

#define BATCH_SIZE  32
#define INPUT_SIZE  32
#define HIDDEN_SIZE 32

int main() {
#ifndef BAREMETAL
    if (mlockall(MCL_CURRENT | MCL_FUTURE) != 0) {
      perror("mlockall failed");
      exit(1);
    }
#endif

  unsigned long setup_start, setup_end, start, end, setup_cycles, benchmark_cycles;
  // Setup start
  setup_start = read_cycles();

  // Matrix A, elem_t, BATCH_SIZE, INPUT_SIZE Setup

  // Matrix B, elem_t, INPUT_SIZE, HIDDEN_SIZE Setup

  // Matrix C
  elem_t C[BATCH_SIZE][HIDDEN_SIZE];

  // Setup end
  setup_end = read_cycles();

  // Gemmini instructions start
  gemmini_fence();

  // Tiled matmul start
  start = read_cycles();

  // Clear TLB
  gemmini_flush(0);

  // Main benchmark code
  gemmini_extended_config_ex(WS, 0, 0, 1, 0, 0);
  gemmini_extended_config_st(32, 0, ACC_SCALE_IDENTITY);
  gemmini_extended3_config_ld(32, MVIN_SCALE_IDENTITY, 0, 0);
  gemmini_extended3_config_ld(32, MVIN_SCALE_IDENTITY, 0, 1);
  gemmini_extended3_config_ld(128, MVIN_SCALE_IDENTITY, 0, 2);
  gemmini_extended_mvin(A + 0x0, 0x0, 32, 16);
  gemmini_extended_mvin2(B + 0x0, 0x3fc0, 32, 16);
  gemmini_extended_preload(0x3fc0, 0x80000000, 16, 16, 16, 16);
  gemmini_extended_compute_preloaded(0x0, 0xffffffff, 16, 16, 16, 16);
  gemmini_extended_mvin(A + 0x200, 0x20, 32, 16);
  gemmini_extended_preload(0xffffffffffffffff, 0x80000020, 16, 16, 16, 16);
  gemmini_extended_compute_accumulated(0x20, 0xffffffff, 16, 16, 16, 16);
  gemmini_extended_preload(0x3fd0, 0x80000010, 16, 16, 16, 16);
  gemmini_extended_compute_preloaded(0x0, 0xffffffff, 16, 16, 16, 16);
  gemmini_extended_preload(0xffffffffffffffff, 0x80000030, 16, 16, 16, 16);
  gemmini_extended_compute_accumulated(0x20, 0xffffffff, 16, 16, 16, 16);
  gemmini_extended_mvin2(B + 0x200, 0x3fe0, 32, 16);
  gemmini_extended_preload(0x3fe0, 0xc0000000, 16, 16, 16, 16);
  gemmini_extended_compute_preloaded(0x10, 0xffffffff, 16, 16, 16, 16);
  gemmini_extended_preload(0xffffffffffffffff, 0xc0000020, 16, 16, 16, 16);
  gemmini_extended_compute_accumulated(0x30, 0xffffffff, 16, 16, 16, 16);
  gemmini_extended_preload(0x3ff0, 0xc0000010, 16, 16, 16, 16);
  gemmini_extended_compute_preloaded(0x10, 0xffffffff, 16, 16, 16, 16);
  gemmini_extended_mvout(C + 0x0, 0xc0000000, 32, 16);
  gemmini_extended_preload(0xffffffffffffffff, 0xc0000030, 16, 16, 16, 16);
  gemmini_extended_compute_accumulated(0x30, 0xffffffff, 16, 16, 16, 16);
  gemmini_extended_mvout(C + 0x200, 0xc0000020, 32, 16);

  gemmini_fence();
  // Gemmini instructions end

  // Tiled matmul end
  end = read_cycles();
  
  setup_cycles = setup_end - setup_start;
  benchmark_cycles = end - start;
  printf("Setup cycles taken: %u\n", setup_cycles);
  printf("Cycles taken: %u\n", benchmark_cycles);

  exit(0);
}