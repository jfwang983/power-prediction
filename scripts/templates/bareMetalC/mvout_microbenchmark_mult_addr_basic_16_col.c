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

#define DIM 16
#define ACC_ROWS 1024
#define AccData_rows 128
#define AccData_cols 16
#define C_rows 16
#define C_cols 16

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

  int iterations = 1000;

  // Matrix AccData, acc_t, AccData_rows, AccData_cols Setup

  elem_t C[C_rows][C_cols];

  uint32_t C_acc_base_addr = 1 << 31 | 0 << 30 | 1 << 29;
  uint32_t MAX_ACC_ADDR = C_acc_base_addr + ACC_ROWS;

  uint32_t C_acc_addr = C_acc_base_addr;
  uint32_t next_C_acc_addr;
  uint32_t C_acc_addr_offset = 0;

  // Setup end
  setup_end = read_cycles();

  // Gemmini instructions start
  gemmini_fence();

  // preload+compute start
  start = read_cycles();

  // Clear TLB
  gemmini_flush(0);

  // Config Setup
  gemmini_extended3_config_ld(AccData_cols * sizeof(acc_t), ACC_SCALE_IDENTITY, false, 0); // A Matrix
  gemmini_config_st(C_cols);
  // printf("gemmini_extended3_config_ld(%u, ACC_SCALE_IDENTITY, false, 0);\n", AccData_rows * sizeof(acc_t));
  // printf("gemmini_config_st(%u);\n", C_cols);

  // Move in matrices for accumulator initialization
  for(int i = 0; i < ACC_ROWS; i += AccData_rows) {
    for(int j = 0; j < AccData_rows; j += DIM) {
      gemmini_extended_mvin(AccData[j], C_acc_addr, DIM, DIM);
      C_acc_addr += DIM;
    }
  }

  C_acc_base_addr = 1 << 31;
  C_acc_addr = C_acc_base_addr;

  // Main microbenchmark code
  for(int i = 0; i < iterations; i++) {
    gemmini_extended_mvout(C, C_acc_addr, C_cols, C_rows);
    // printf("gemmini_extended_mvout(C, %p, %u, %u);\n", C_acc_addr, C_cols, C_rows);
    next_C_acc_addr = (C_cols >= DIM) ? C_acc_addr + C_cols : C_acc_addr + DIM;
    C_acc_addr = (next_C_acc_addr >= MAX_ACC_ADDR) ? C_acc_base_addr : next_C_acc_addr;
  }

  gemmini_fence();
  // Gemmini instructions end

  // preload + compute end
  end = read_cycles();

  setup_cycles = setup_end - setup_start;
  benchmark_cycles = end - start;

  printf("Setup cycles taken: %u\n", setup_cycles);
  printf("Cycles taken: %u\n", benchmark_cycles);

  exit(0);
}