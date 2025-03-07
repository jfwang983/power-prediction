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
#define B_rows 8
#define B_cols 16
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
		{-44, 40, -128, 67, -76, 47, 104, -102}, // Matrix A row
		{-98, -117, -33, -50, 15, -86, -85, 7}, // Matrix A row
		{-9, 112, 54, -107, -100, 95, 117, 80}, // Matrix A row
		{-107, 58, -32, 97, -25, 5, -12, 40}, // Matrix A row
		{-125, 102, 77, 16, 80, 126, -88, 14}, // Matrix A row
		{92, 7, -11, 68, -16, -29, 24, -119}, // Matrix A row
		{-14, -16, -99, -128, -108, -61, -44, -66}, // Matrix A row
		{-78, 77, -71, -127, -50, 9, 8, -50}, // Matrix A row
		{125, 44, -59, -56, 21, 59, 74, 108}, // Matrix A row
		{125, 46, 59, 30, -52, -58, -54, 67}, // Matrix A row
		{116, -6, -101, -82, -33, -37, -11, -77}, // Matrix A row
		{-18, 20, -108, -125, -28, 56, 74, 77}, // Matrix A row
		{-107, -12, -103, 88, 82, -12, -97, -73}, // Matrix A row
		{-24, 23, -127, -46, 10, 29, 87, -100}, // Matrix A row
		{-13, -57, -95, -48, 40, -26, 79, -55}, // Matrix A row
		{115, -76, 9, 42, 111, 12, -83, 21} // Matrix A row
	}; // Matrix A row

  // Matrix B, elem_t, B_rows, B_cols Setup
	elem_t B[B_rows][B_cols] = { // Matrix B row
		{84, -94, 82, -5, 26, -60, -102, 88, 84, 42, -75, 76, 27, 83, 51, -52}, // Matrix B row
		{-1, -83, -18, -72, -12, -83, 51, 2, 59, -14, 9, -90, 22, 82, 5, -60}, // Matrix B row
		{59, 36, -43, -93, -83, -17, 85, 97, -30, -36, -106, 69, -117, 84, 70, -90}, // Matrix B row
		{-90, 25, -66, 68, 35, -45, 78, -4, 123, 62, 101, -88, -8, -58, 78, -72}, // Matrix B row
		{26, -67, -87, -26, 29, -27, -111, -101, 4, -22, 93, 46, 85, -104, 104, -128}, // Matrix B row
		{-88, -106, -84, 24, -45, 67, -117, 4, -121, -86, -87, -124, 4, 16, -111, -68}, // Matrix B row
		{77, -72, -123, 94, 38, -101, 66, -33, 29, 109, 99, -3, 35, 45, 49, -86}, // Matrix B row
		{-18, -17, -28, 3, 46, 26, -68, 0, 102, -51, -90, -102, 52, -39, 12, 55} // Matrix B row
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

  // Main microbenchmark code
  for(int i = 0; i < iterations; i++) {
    gemmini_extended_preload(B_sp_addr, C_sp_addr, B_cols, B_rows, C_cols, C_rows);
    gemmini_extended_compute_preloaded(A_sp_addr, GARBAGE_ADDR, A_cols, A_rows, 0, 0);
	  // printf("gemmini_extended_preload(%p, 0x%x, %u, %u, %u, %u);\n", B_sp_addr, C_sp_addr, B_cols, B_rows, C_cols, C_rows);
  	// printf("gemmini_extended_compute_preloaded(%p, 0x%x, %u, %u, 0, 0);\n", A_sp_addr, GARBAGE_ADDR, A_cols, A_rows);
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