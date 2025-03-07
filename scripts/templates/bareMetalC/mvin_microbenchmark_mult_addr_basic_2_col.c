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
#define SPAD_ROWS 16384
#define A_rows 16
#define A_cols 2

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

  // Matrix A, elem_t, A_rows, A_cols Setup
	elem_t A[A_rows][A_cols] = { // Matrix A row
		{7, -60}, // Matrix A row
		{3, -28}, // Matrix A row
		{-72, -44}, // Matrix A row
		{84, -17}, // Matrix A row
		{25, 30}, // Matrix A row
		{99, 46}, // Matrix A row
		{-55, -68}, // Matrix A row
		{75, 82}, // Matrix A row
		{95, -24}, // Matrix A row
		{52, 79}, // Matrix A row
		{5, -64}, // Matrix A row
		{43, -77}, // Matrix A row
		{104, -3}, // Matrix A row
		{-118, -26}, // Matrix A row
		{-111, 20}, // Matrix A row
		{46, 41} // Matrix A row
	}; // Matrix A row

  uint32_t A_sp_addr = 0;
  uint32_t next_A_sp_addr;

  // Setup end
  setup_end = read_cycles();

  // Gemmini instructions start
  gemmini_fence(); 

  // Mvout start
  start = read_cycles();

  // Clear TLB
  gemmini_flush(0);

  // Config setup
  gemmini_extended3_config_ld(A_cols, MVIN_SCALE_IDENTITY, false, 0); // A Matrix
  // printf("gemmini_extended3_config_ld(%u, MVIN_SCALE_IDENTITY, false, 0);\n", A_cols);

  // Main microbenchmark code
  for(int i = 0; i < iterations; i++) {
    gemmini_extended_mvin(A, A_sp_addr, A_cols, A_rows);
    // printf("gemmini_extended_mvin(A, %p, %u, %u);\n", A_sp_addr, A_cols, A_rows);
    next_A_sp_addr = A_sp_addr + A_cols;
    A_sp_addr = (next_A_sp_addr >= SPAD_ROWS) ? 0 : next_A_sp_addr;
  }

  gemmini_fence();
  // Gemmini instructions end

  // Mvout end
  end = read_cycles();

  setup_cycles = setup_end - setup_start;
  benchmark_cycles = end - start;

  printf("Setup cycles taken: %u\n", setup_cycles);
  printf("Cycles taken: %u\n", benchmark_cycles);

  exit(0);
}