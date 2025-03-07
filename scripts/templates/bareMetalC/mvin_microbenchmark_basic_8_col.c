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
#define A_cols 8

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
		{20, 102, -91, 5, -25, -45, 15, 111}, // Matrix A row
		{-35, 82, 50, -59, -25, -57, -117, -11}, // Matrix A row
		{115, -2, 57, 89, 6, 50, -83, 18}, // Matrix A row
		{127, -59, 48, -11, 85, 83, 97, -24}, // Matrix A row
		{-58, -81, -13, -103, -84, -102, -73, -112}, // Matrix A row
		{62, 4, 87, 25, 108, -22, 127, 82}, // Matrix A row
		{-102, -17, 3, 25, -85, -5, -35, -68}, // Matrix A row
		{41, -114, -122, 68, -20, 68, 91, -93}, // Matrix A row
		{-68, -91, -128, 2, 82, -20, -13, 32}, // Matrix A row
		{-33, -89, 126, -14, 75, 125, -99, 60}, // Matrix A row
		{-111, 49, 1, 28, -10, 77, 72, -79}, // Matrix A row
		{-55, -59, -43, 124, 38, 70, -113, 11}, // Matrix A row
		{89, -85, 3, -107, 125, -79, 11, 11}, // Matrix A row
		{14, -87, -101, -46, -113, 106, -110, -32}, // Matrix A row
		{-114, -71, -47, -113, -76, 48, 46, -103}, // Matrix A row
		{71, -3, 54, 56, -113, -9, -56, -46} // Matrix A row
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