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
#define A_rows 16
#define A_cols 4

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
		{87, -48, 1, 73}, // Matrix A row
		{56, -48, 5, 103}, // Matrix A row
		{67, 39, 87, -81}, // Matrix A row
		{93, -59, -35, 119}, // Matrix A row
		{-109, 22, 56, -56}, // Matrix A row
		{-122, -95, 103, -62}, // Matrix A row
		{101, 87, 24, 52}, // Matrix A row
		{103, 123, -68, -24}, // Matrix A row
		{126, 90, 59, 104}, // Matrix A row
		{21, 31, -31, 47}, // Matrix A row
		{-86, -121, 65, 66}, // Matrix A row
		{51, -21, -24, -17}, // Matrix A row
		{62, -99, 72, -53}, // Matrix A row
		{65, -62, 118, 69}, // Matrix A row
		{12, -9, 125, -96}, // Matrix A row
		{114, -59, 29, 13} // Matrix A row
	}; // Matrix A row

  uint32_t A_sp_addr = 0;

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