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
#define A_rows 8
#define A_cols 16
#define B_rows 16
#define B_cols 8
#define C_rows A_rows
#define C_cols B_cols

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
		{-25, -103, 18, -47, 60, -78, 14, -122, 17, 11, 17, 53, 86, -128, -52, 65}, // Matrix A row
		{99, -55, 127, 51, 47, -39, -68, -72, -77, 90, -125, 47, 17, -122, 42, 75}, // Matrix A row
		{9, -110, 114, 118, -128, -71, -79, 13, 41, -59, -21, -1, -14, 42, 57, 10}, // Matrix A row
		{0, 19, 23, 35, -52, 121, -115, -57, 0, 51, 46, -78, -87, 30, -73, 16}, // Matrix A row
		{-90, -88, -24, 86, 79, -58, -11, -49, -52, -68, 38, 95, -119, -92, -95, -9}, // Matrix A row
		{-124, -11, -81, 46, -48, 121, 105, -74, -89, 43, 47, -74, -73, 68, -10, 31}, // Matrix A row
		{-36, 20, -91, -75, -27, 11, 68, -92, -96, -86, -12, 78, -63, 126, -33, 64}, // Matrix A row
		{-121, -108, 30, 74, -71, 125, -30, 93, -57, 105, 127, -24, 110, 123, 50, 1} // Matrix A row
	}; // Matrix A row

  // Matrix B, elem_t, B_rows, B_cols Setup
	elem_t B[B_rows][B_cols] = { // Matrix B row
		{-70, -57, -64, -1, 14, 52, 127, -27}, // Matrix B row
		{-72, 12, 54, -29, 41, -96, 125, 100}, // Matrix B row
		{84, -120, -10, 22, 96, 11, 49, -110}, // Matrix B row
		{43, 25, 95, -98, 72, 27, -24, 12}, // Matrix B row
		{-21, -42, 8, 27, -75, 94, -21, 33}, // Matrix B row
		{40, 118, -64, 125, 3, 92, -56, 121}, // Matrix B row
		{5, -117, 28, -47, 81, 37, -53, 76}, // Matrix B row
		{53, -111, 11, 63, 25, -7, 63, 81}, // Matrix B row
		{46, 11, -29, -21, -2, 83, -88, 47}, // Matrix B row
		{66, 75, 54, 37, 62, 126, 76, 38}, // Matrix B row
		{14, -66, 90, -75, -93, -112, -74, -110}, // Matrix B row
		{-9, 28, -73, 95, 33, 62, -35, -87}, // Matrix B row
		{71, -12, 84, -20, -52, 19, -86, -81}, // Matrix B row
		{52, -45, 52, 89, -52, 56, 23, 91}, // Matrix B row
		{49, 93, 20, 113, -25, 115, -72, -72}, // Matrix B row
		{57, -83, 117, 38, -4, -18, 67, 89} // Matrix B row
	}; // Matrix B row

  elem_t C[C_rows][C_cols];

  uint32_t A_sp_addr = 0;
  uint32_t B_sp_addr = DIM;
  uint32_t C_sp_addr = 1 << 31 | 0 << 30;

  // Setup end
  setup_end = read_cycles();

  // Gemmini instructions start
  gemmini_fence();

  // preload+compute start
  start = read_cycles();

  // Clear TLB
  gemmini_flush(0);

  // Config Setup
  gemmini_extended3_config_ld(A_cols, MVIN_SCALE_IDENTITY, false, 0); // A Matrix
  gemmini_extended3_config_ld(B_cols, MVIN_SCALE_IDENTITY, false, 1); // B Matrix
  gemmini_config_ex(WS, NO_ACTIVATION, 0);
  // printf("gemmini_extended3_config_ld(%u, MVIN_SCALE_IDENTITY, false, 0);\n", A_cols);
  // printf("gemmini_extended3_config_ld(%u, MVIN_SCALE_IDENTITY, false, 1);\n", B_cols);
  // printf("gemmini_config_ex(WS, NO_ACTIVATION, 0);\n");

  // Move in matrices for initialization
  gemmini_extended_mvin(A, A_sp_addr, A_cols, A_rows);
  gemmini_extended_mvin2(B, B_sp_addr, B_cols, B_rows);
  // printf("gemmini_extended_mvin(A, %p, %u, %u);\n", A_sp_addr, A_cols, A_rows);
  // printf("gemmini_extended_mvin(B, %p, %u, %u);\n", B_sp_addr, B_cols, B_rows);

  gemmini_extended_preload(B_sp_addr, C_sp_addr, B_cols, B_rows, C_cols, C_rows);
  gemmini_extended_compute_preloaded(A_sp_addr, GARBAGE_ADDR, A_cols, A_rows, 0, 0);
	// printf("gemmini_extended_preload(%p, 0x%x, %u, %u, %u, %u);\n", B_sp_addr, C_sp_addr, B_cols, B_rows, C_cols, C_rows);
  // printf("gemmini_extended_compute_preloaded(%p, 0x%x, %u, %u, 0, 0);\n", A_sp_addr, GARBAGE_ADDR, A_cols, A_rows);

  // Main microbenchmark code
  for(int i = 0; i < iterations; i++) {
    gemmini_extended_preload(B_sp_addr, C_sp_addr, B_cols, B_rows, C_cols, C_rows);
    gemmini_extended_compute_accumulated(A_sp_addr, GARBAGE_ADDR, A_cols, A_rows, 0, 0);
	  // printf("gemmini_extended_preload(%p, 0x%x, %u, %u, %u, %u);\n", B_sp_addr, C_sp_addr, B_cols, B_rows, C_cols, C_rows);
  	// printf("gemmini_extended_compute_accumulated(%p, 0x%x, %u, %u, 0, 0);\n", A_sp_addr, GARBAGE_ADDR, A_cols, A_rows);
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