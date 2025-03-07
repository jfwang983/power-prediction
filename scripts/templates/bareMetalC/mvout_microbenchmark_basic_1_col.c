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
#define B_rows 16
#define B_cols 1
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
		{-15, -82, 4, 74, -33, -70, -63, 32, 56, 88, -85, -18, -118, -104, 127, -108}, // Matrix A row
		{47, 36, -8, -66, -82, 68, -116, -69, 123, 109, -73, 96, -99, -121, -19, -94}, // Matrix A row
		{65, 115, -118, -127, 96, 119, -108, -22, -98, 34, -46, 43, -23, 56, -80, 38}, // Matrix A row
		{-8, 96, -45, 84, 37, 39, -28, -76, 22, 46, -103, -51, 112, -43, 52, 81}, // Matrix A row
		{-11, -126, 46, 41, -19, -78, -46, -102, -114, 91, -53, 123, -61, -85, 60, 37}, // Matrix A row
		{-57, -40, -106, 117, -122, 22, 116, 54, 87, -65, -127, -68, 115, 61, -86, -74}, // Matrix A row
		{-109, -82, -41, -25, -6, -111, -41, 28, 98, 94, -69, 38, -45, -49, -92, -5}, // Matrix A row
		{79, 106, 61, -69, -78, -122, -126, -46, -29, 122, 111, 79, -77, 32, 44, -47}, // Matrix A row
		{58, -116, -109, 80, 79, 93, -49, 120, -90, 23, 13, 37, -71, -29, -14, -49}, // Matrix A row
		{113, -68, -127, 4, 83, 2, 96, 100, 32, -33, -124, -100, 24, -99, -66, -99}, // Matrix A row
		{121, -114, 61, 17, -40, -1, -87, -22, 59, 41, -54, -3, -88, -6, 43, -108}, // Matrix A row
		{28, -76, -71, 105, -57, 102, 6, -3, -88, -84, 115, -56, -102, -86, -79, -75}, // Matrix A row
		{-38, 12, -23, -11, -101, -4, -118, 18, -99, 32, 18, -104, -55, 105, 78, 94}, // Matrix A row
		{-64, 105, -43, -6, -51, -27, 80, -20, 61, 105, 109, -2, -35, -60, -56, -66}, // Matrix A row
		{-41, -123, -100, -18, -9, -15, -44, -2, 69, -4, -104, 122, 7, 28, 53, 28}, // Matrix A row
		{-87, 58, 44, -125, 14, 99, 46, 126, -64, 126, -46, 76, -37, 72, -60, 64} // Matrix A row
	}; // Matrix A row

  // Matrix B, elem_t, B_rows, B_cols Setup
	elem_t B[B_rows][B_cols] = { // Matrix B row
		{11}, // Matrix B row
		{-59}, // Matrix B row
		{-24}, // Matrix B row
		{-91}, // Matrix B row
		{127}, // Matrix B row
		{-125}, // Matrix B row
		{86}, // Matrix B row
		{34}, // Matrix B row
		{-51}, // Matrix B row
		{-35}, // Matrix B row
		{-62}, // Matrix B row
		{55}, // Matrix B row
		{-13}, // Matrix B row
		{-121}, // Matrix B row
		{-15}, // Matrix B row
		{112} // Matrix B row
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
  gemmini_config_st(C_cols);
  // printf("gemmini_extended3_config_ld(%u, MVIN_SCALE_IDENTITY, false, 0);\n", A_cols);
  // printf("gemmini_extended3_config_ld(%u, MVIN_SCALE_IDENTITY, false, 1);\n", B_cols);
  // printf("gemmini_config_ex(WS, NO_ACTIVATION, 0);\n");
  // printf("gemmini_config_st(%u);\n", C_cols);

  // Move in matrices for initialization
  gemmini_extended_mvin(A, A_sp_addr, A_cols, A_rows);
  gemmini_extended_mvin2(B, B_sp_addr, B_cols, B_rows);
  // printf("gemmini_extended_mvin(A, %p, %u, %u);\n", A_sp_addr, A_cols, A_rows);
  // printf("gemmini_extended_mvin(B, %p, %u, %u);\n", B_sp_addr, B_cols, B_rows);

  gemmini_extended_preload(B_sp_addr, C_sp_addr, B_cols, B_rows, C_cols, C_rows);
  gemmini_extended_compute_preloaded(A_sp_addr, GARBAGE_ADDR, A_cols, A_rows, B_cols, B_rows);
	// printf("gemmini_extended_preload(%p, 0x%x, %u, %u, %u, %u);\n", B_sp_addr, C_sp_addr, B_cols, B_rows, C_cols, C_rows);
	// printf("gemmini_extended_compute_preloaded(%p, 0x%x, %u, %u, %u, %u);\n", A_sp_addr, GARBAGE_ADDR, A_cols, A_rows, B_cols, B_rows);

  // Main microbenchmark code
  for(int i = 0; i < iterations; i++) {
    gemmini_extended_mvout(C, C_sp_addr, C_cols, C_rows);
  	// printf("gemmini_extended_mvout(C, %p, %u, %u);\n", C_sp_addr, C_cols, C_rows);
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