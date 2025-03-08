// See LICENSE for license details.

#include <stdint.h>
#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>
#ifndef BAREMETAL
#include <sys/mman.h>
#endif
#include "matmul_funcs.h"

#define BATCH_SIZE  16
#define INPUT_SIZE  128
#define HIDDEN_SIZE 32
#define OUTPUT_SIZE 64

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

  // Input Layer
  // Matrix input_mat, elem_t, BATCH_SIZE, INPUT_SIZE Setup

  // Matrix weights0, elem_t, INPUT_SIZE, HIDDEN_SIZE Setup
  
  elem_t inter_results0[BATCH_SIZE][HIDDEN_SIZE];

  // Hidden Layer
  // Matrix weights1, elem_t, HIDDEN_SIZE, HIDDEN_SIZE Setup
  
  elem_t inter_results1[BATCH_SIZE][HIDDEN_SIZE];

  // Output Layer
  // Matrix weights2, elem_t, HIDDEN_SIZE, OUTPUT_SIZE Setup
  
  elem_t output_mat[BATCH_SIZE][OUTPUT_SIZE];

  // Setup end
  setup_end = read_cycles();

  // Gemmini instructions start
  gemmini_fence();

  // Tiled matmul start
  start = read_cycles();

  // Clear TLB
  gemmini_flush(0);
  
  // MLP Layer Matmuls
  gemmini_extended_config_ex(WS, 0, 0, 1, 0, 0);
  gemmini_extended_config_st(32, 0, ACC_SCALE_IDENTITY);
  gemmini_extended3_config_ld(128, MVIN_SCALE_IDENTITY, 0, 0);
  gemmini_extended3_config_ld(32, MVIN_SCALE_IDENTITY, 0, 1);
  gemmini_extended3_config_ld(128, MVIN_SCALE_IDENTITY, 0, 2);
  gemmini_extended_mvin(input_mat + 0x0, 0x0, 64, 16);
  gemmini_extended_mvin2(weights0 + 0x0, 0x3f00, 32, 16);
  gemmini_extended_preload(0x3f00, 0x80000000, 16, 16, 16, 16);
  gemmini_extended_compute_preloaded(0x0, 0xffffffff, 16, 16, 16, 16);
  gemmini_extended_preload(0x3f10, 0x80000010, 16, 16, 16, 16);
  gemmini_extended_compute_preloaded(0x0, 0xffffffff, 16, 16, 16, 16);
  gemmini_extended_mvin2(weights0 + 0x200, 0x3f20, 32, 16);
  gemmini_extended_preload(0x3f20, 0xc0000000, 16, 16, 16, 16);
  gemmini_extended_compute_preloaded(0x10, 0xffffffff, 16, 16, 16, 16);
  gemmini_extended_preload(0x3f30, 0xc0000010, 16, 16, 16, 16);
  gemmini_extended_compute_preloaded(0x10, 0xffffffff, 16, 16, 16, 16);
  gemmini_extended_mvin2(weights0 + 0x400, 0x3f40, 32, 16);
  gemmini_extended_preload(0x3f40, 0xc0000000, 16, 16, 16, 16);
  gemmini_extended_compute_preloaded(0x20, 0xffffffff, 16, 16, 16, 16);
  gemmini_extended_preload(0x3f50, 0xc0000010, 16, 16, 16, 16);
  gemmini_extended_compute_preloaded(0x20, 0xffffffff, 16, 16, 16, 16);
  gemmini_extended_mvin2(weights0 + 0x600, 0x3f60, 32, 16);
  gemmini_extended_preload(0x3f60, 0xc0000000, 16, 16, 16, 16);
  gemmini_extended_compute_preloaded(0x30, 0xffffffff, 16, 16, 16, 16);
  gemmini_extended_preload(0x3f70, 0xc0000010, 16, 16, 16, 16);
  gemmini_extended_compute_preloaded(0x30, 0xffffffff, 16, 16, 16, 16);
  gemmini_extended_mvin(input_mat + 0x40, 0x40, 64, 16);
  gemmini_extended_mvin2(weights0 + 0x800, 0x3f80, 32, 16);
  gemmini_extended_preload(0x3f80, 0xc0000000, 16, 16, 16, 16);
  gemmini_extended_compute_preloaded(0x40, 0xffffffff, 16, 16, 16, 16);
  gemmini_extended_preload(0x3f90, 0xc0000010, 16, 16, 16, 16);
  gemmini_extended_compute_preloaded(0x40, 0xffffffff, 16, 16, 16, 16);
  gemmini_extended_mvin2(weights0 + 0xa00, 0x3fa0, 32, 16);
  gemmini_extended_preload(0x3fa0, 0xc0000000, 16, 16, 16, 16);
  gemmini_extended_compute_preloaded(0x50, 0xffffffff, 16, 16, 16, 16);
  gemmini_extended_preload(0x3fb0, 0xc0000010, 16, 16, 16, 16);
  gemmini_extended_compute_preloaded(0x50, 0xffffffff, 16, 16, 16, 16);
  gemmini_extended_mvin2(weights0 + 0xc00, 0x3fc0, 32, 16);
  gemmini_extended_preload(0x3fc0, 0xc0000000, 16, 16, 16, 16);
  gemmini_extended_compute_preloaded(0x60, 0xffffffff, 16, 16, 16, 16);
  gemmini_extended_preload(0x3fd0, 0xc0000010, 16, 16, 16, 16);
  gemmini_extended_compute_preloaded(0x60, 0xffffffff, 16, 16, 16, 16);
  gemmini_extended_mvin2(weights0 + 0xe00, 0x3fe0, 32, 16);
  gemmini_extended_preload(0x3fe0, 0xc0000000, 16, 16, 16, 16);
  gemmini_extended_compute_preloaded(0x70, 0xffffffff, 16, 16, 16, 16);
  gemmini_extended_preload(0x3ff0, 0xc0000010, 16, 16, 16, 16);
  gemmini_extended_compute_preloaded(0x70, 0xffffffff, 16, 16, 16, 16);
  gemmini_extended_mvout(inter_results0 + 0x0, 0xc0000000, 32, 16);

  gemmini_extended_config_ex(WS, 0, 0, 1, 0, 0);
  gemmini_extended_config_st(32, 0, ACC_SCALE_IDENTITY);
  gemmini_extended3_config_ld(32, MVIN_SCALE_IDENTITY, 0, 0);
  gemmini_extended3_config_ld(32, MVIN_SCALE_IDENTITY, 0, 1);
  gemmini_extended3_config_ld(128, MVIN_SCALE_IDENTITY, 0, 2);
  gemmini_extended_mvin(inter_results0 + 0x0, 0x0, 32, 16);
  gemmini_extended_mvin2(weights1 + 0x0, 0x3fc0, 32, 16);
  gemmini_extended_preload(0x3fc0, 0x80000000, 16, 16, 16, 16);
  gemmini_extended_compute_preloaded(0x0, 0xffffffff, 16, 16, 16, 16);
  gemmini_extended_preload(0x3fd0, 0x80000010, 16, 16, 16, 16);
  gemmini_extended_compute_preloaded(0x0, 0xffffffff, 16, 16, 16, 16);
  gemmini_extended_mvin2(weights1 + 0x200, 0x3fe0, 32, 16);
  gemmini_extended_preload(0x3fe0, 0xc0000000, 16, 16, 16, 16);
  gemmini_extended_compute_preloaded(0x10, 0xffffffff, 16, 16, 16, 16);
  gemmini_extended_preload(0x3ff0, 0xc0000010, 16, 16, 16, 16);
  gemmini_extended_compute_preloaded(0x10, 0xffffffff, 16, 16, 16, 16);
  gemmini_extended_mvout(inter_results1 + 0x0, 0xc0000000, 32, 16);

  gemmini_extended_config_ex(WS, 0, 0, 1, 0, 0);
  gemmini_extended_config_st(64, 0, ACC_SCALE_IDENTITY);
  gemmini_extended3_config_ld(32, MVIN_SCALE_IDENTITY, 0, 0);
  gemmini_extended3_config_ld(64, MVIN_SCALE_IDENTITY, 0, 1);
  gemmini_extended3_config_ld(256, MVIN_SCALE_IDENTITY, 0, 2);
  gemmini_extended_mvin(inter_results1 + 0x0, 0x0, 32, 16);
  gemmini_extended_mvin2(weights2 + 0x0, 0x3f80, 64, 16);
  gemmini_extended_preload(0x3f80, 0x80000000, 16, 16, 16, 16);
  gemmini_extended_compute_preloaded(0x0, 0xffffffff, 16, 16, 16, 16);
  gemmini_extended_preload(0x3f90, 0x80000010, 16, 16, 16, 16);
  gemmini_extended_compute_preloaded(0x0, 0xffffffff, 16, 16, 16, 16);
  gemmini_extended_preload(0x3fa0, 0x80000020, 16, 16, 16, 16);
  gemmini_extended_compute_preloaded(0x0, 0xffffffff, 16, 16, 16, 16);
  gemmini_extended_preload(0x3fb0, 0x80000030, 16, 16, 16, 16);
  gemmini_extended_compute_preloaded(0x0, 0xffffffff, 16, 16, 16, 16);
  gemmini_extended_mvin2(weights2 + 0x400, 0x3fc0, 64, 16);
  gemmini_extended_preload(0x3fc0, 0xc0000000, 16, 16, 16, 16);
  gemmini_extended_compute_preloaded(0x10, 0xffffffff, 16, 16, 16, 16);
  gemmini_extended_preload(0x3fd0, 0xc0000010, 16, 16, 16, 16);
  gemmini_extended_compute_preloaded(0x10, 0xffffffff, 16, 16, 16, 16);
  gemmini_extended_preload(0x3fe0, 0xc0000020, 16, 16, 16, 16);
  gemmini_extended_compute_preloaded(0x10, 0xffffffff, 16, 16, 16, 16);
  gemmini_extended_preload(0x3ff0, 0xc0000030, 16, 16, 16, 16);
  gemmini_extended_compute_preloaded(0x10, 0xffffffff, 16, 16, 16, 16);
  gemmini_extended_mvout(output_mat + 0x0, 0xc0000000, 64, 16);

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