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
		{21, 105, 34, 52, -14, 111, 29, -83, 120, -19, 82, -6, -119, -53, 69, -41}, // Matrix A row
		{90, 43, -3, -17, 122, -114, -57, -84, -15, -22, 40, 35, -26, 25, -58, 8}, // Matrix A row
		{-85, 95, -34, 44, -97, 63, -87, -93, 122, 39, 68, -65, 45, 82, -56, 115}, // Matrix A row
		{93, -68, -28, -3, -88, -75, -12, -97, -110, -92, 92, -85, 77, 43, -42, 92}, // Matrix A row
		{23, 124, 33, -12, -57, -67, 71, 46, -36, -99, -72, -63, 1, -50, -41, 77}, // Matrix A row
		{-42, -74, 103, -55, 53, -26, -122, -18, 38, -109, 120, 72, 122, -45, -23, -2}, // Matrix A row
		{87, 125, -64, 19, 58, 10, -118, 25, 124, 68, 73, -64, -35, 78, 10, -74}, // Matrix A row
		{-68, -25, 8, 80, 20, 27, -94, 9, 26, -96, -96, -117, 85, 104, 55, -87}, // Matrix A row
		{-115, 98, -99, 2, -91, 40, -30, -40, -5, 44, 49, -120, -68, -26, 86, 112}, // Matrix A row
		{-22, -43, 113, -93, -6, -84, -36, -36, 98, 57, -34, 69, 58, -81, -127, -70}, // Matrix A row
		{-112, 27, 26, -122, -50, 94, -6, 87, 30, 11, 84, -61, 52, 32, -72, -103}, // Matrix A row
		{90, 104, -66, -127, -67, -80, 48, -100, -51, 26, 102, 73, -16, -20, 90, 80}, // Matrix A row
		{46, -23, -54, 3, -55, 3, -70, -68, 97, -85, -3, 12, 107, 90, -64, -127}, // Matrix A row
		{113, 111, 29, -80, -71, -117, -127, 127, -62, -23, -68, -60, 104, -94, -19, -91}, // Matrix A row
		{-38, 98, 64, -69, 86, 53, -84, -92, -9, -82, 50, -65, 72, -94, 6, -79}, // Matrix A row
		{20, 72, -118, -84, 26, 55, 29, 21, 78, -76, -48, -15, 113, 80, 124, -17} // Matrix A row
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