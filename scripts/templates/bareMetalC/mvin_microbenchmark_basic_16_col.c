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
#define A_cols 16

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
		{-37, 20, -95, 113, 126, -46, 13, -125, -50, 74, -53, 33, -97, 111, 25, -74}, // Matrix A row
		{92, -7, 96, -26, 76, -56, 62, 117, 4, -43, -39, 5, 126, -85, -55, 8}, // Matrix A row
		{51, -99, -80, 85, 64, -38, 55, 78, -94, 17, -65, -15, 6, 59, -90, 78}, // Matrix A row
		{4, -68, -102, -71, -100, -116, 15, 70, -85, -80, -25, -128, 39, -28, 25, 23}, // Matrix A row
		{107, 61, -105, 4, -45, 89, 67, 114, 36, -60, 2, 80, -43, 84, -14, 124}, // Matrix A row
		{100, 122, -99, -20, -72, 53, 79, 127, 109, -42, -32, -81, 115, 101, -92, -60}, // Matrix A row
		{-125, -81, 89, -70, 88, -122, 115, -70, 32, 119, -73, 15, -3, 121, -45, -16}, // Matrix A row
		{-126, 21, 126, -99, 124, -90, -31, 112, 99, 56, -9, -29, 79, 32, 120, -16}, // Matrix A row
		{-61, -127, -51, -56, 41, 118, 94, -61, -57, -35, 109, -63, -77, -87, -103, 24}, // Matrix A row
		{-14, -110, -102, -121, 66, -13, -83, 77, -76, 31, -79, 119, -63, 21, 82, 1}, // Matrix A row
		{-95, -28, -126, -36, -29, -2, 61, -70, -96, -116, 91, 5, 117, 45, -58, -82}, // Matrix A row
		{-107, -10, 54, -82, -48, -13, -67, -57, 49, -2, 53, -99, 9, 31, 10, 5}, // Matrix A row
		{122, 22, -69, 13, 5, 55, -105, 65, 106, -51, 72, 24, -106, -43, 19, 125}, // Matrix A row
		{-68, 15, -67, 70, -125, -41, -39, -11, -13, 45, 108, 76, 118, 23, 111, 58}, // Matrix A row
		{-99, -106, -5, -39, 4, 112, 43, 10, 0, 25, -59, 93, -126, -50, -92, 56}, // Matrix A row
		{-46, 27, 17, -37, -90, -18, -121, -70, -71, 90, -94, -115, -108, -69, 42, -126} // Matrix A row
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