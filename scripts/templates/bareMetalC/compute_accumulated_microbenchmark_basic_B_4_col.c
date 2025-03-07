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
#define B_cols 4
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
		{-79, 81, -14, 117, 14, -91, -24, -36, 124, 15, -78, -47, 22, 45, 115, 50}, // Matrix A row
		{30, 77, 116, -55, 112, 123, -47, -107, -39, 16, -118, -69, -95, -104, 26, -42}, // Matrix A row
		{-126, 99, -102, 96, 9, 54, -67, 125, -101, 21, 124, 123, 100, -63, -92, 104}, // Matrix A row
		{-59, 62, -23, 26, -102, -48, -98, -71, 6, 59, -53, -104, 103, -20, -76, 54}, // Matrix A row
		{73, -69, -127, 27, -93, -97, -95, 68, -123, 46, 54, -51, -111, 49, -49, 41}, // Matrix A row
		{33, 55, -113, 40, -109, -119, -76, -91, 66, 34, -40, 88, -16, 22, -79, -29}, // Matrix A row
		{-122, -5, 99, 114, 10, 66, -22, 15, 39, 2, 42, -50, 44, -35, -95, -5}, // Matrix A row
		{104, 108, -48, 36, 27, -26, 127, -108, 124, 40, 30, 10, -23, -59, 27, -69}, // Matrix A row
		{125, 84, -95, -33, 80, 101, 89, 48, 91, -68, -51, 35, 94, 6, 64, -87}, // Matrix A row
		{-34, -110, 53, -77, 65, -100, -114, -116, 13, 51, -110, -112, 99, 76, -8, 1}, // Matrix A row
		{-104, 74, 105, -11, 59, -9, -103, -30, -23, 59, -45, -37, -77, 99, -81, 66}, // Matrix A row
		{48, 87, 124, 7, 42, 63, 113, -23, -22, -88, 2, -77, 55, -102, 83, -13}, // Matrix A row
		{100, 124, -7, -2, 23, 33, 110, -41, -68, -26, 78, -65, 84, 111, 61, 120}, // Matrix A row
		{-1, 31, 115, 16, 52, -53, -44, -46, -84, -123, 117, 118, 98, -63, 61, 103}, // Matrix A row
		{-84, 57, -79, -14, 117, -12, -40, 77, 124, -124, 90, 46, 22, 115, -1, 25}, // Matrix A row
		{-2, 89, 112, 18, 119, -98, -62, -121, 89, -55, 39, 88, 7, -74, 76, -46} // Matrix A row
	}; // Matrix A row

  // Matrix B, elem_t, B_rows, B_cols Setup
	elem_t B[B_rows][B_cols] = { // Matrix B row
		{16, 69, -82, -49}, // Matrix B row
		{-57, -67, -63, -6}, // Matrix B row
		{-36, -6, -94, 76}, // Matrix B row
		{-18, 19, 122, 97}, // Matrix B row
		{118, -28, 89, -63}, // Matrix B row
		{-55, 101, -123, 28}, // Matrix B row
		{2, -61, 116, -59}, // Matrix B row
		{-9, 50, 94, -61}, // Matrix B row
		{-106, -63, -83, 34}, // Matrix B row
		{113, -126, 104, 110}, // Matrix B row
		{-88, -78, 55, -56}, // Matrix B row
		{-8, 13, 88, 25}, // Matrix B row
		{-114, -102, -91, 101}, // Matrix B row
		{-80, 44, -1, 20}, // Matrix B row
		{78, -50, 11, -17}, // Matrix B row
		{-119, -40, -100, 121} // Matrix B row
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