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
		{89, 26, -50, -84, 16, 92, -34, -89, 110, -99, 54, 50, 125, 120, -122, 109}, // Matrix A row
		{70, -57, 72, -18, -119, -27, 69, 32, 26, 80, 29, 58, 84, 13, 88, 4}, // Matrix A row
		{105, 28, -108, -68, -8, -127, 120, 69, -107, -14, 72, 126, -27, 3, -98, 83}, // Matrix A row
		{-123, 44, 55, -94, 28, -33, 6, 92, -15, 29, 30, 57, -108, 32, -54, 50}, // Matrix A row
		{51, -85, 88, 124, 59, -101, -8, 63, -6, -65, 39, -45, -81, -54, -3, -100}, // Matrix A row
		{44, -75, 103, -122, -25, 58, 68, -38, -100, 13, 92, -34, -12, -113, 57, 124}, // Matrix A row
		{4, -65, 45, -59, -108, 111, -55, 120, -43, -77, 126, -85, -23, -125, -43, 36}, // Matrix A row
		{71, 76, 77, -97, -106, -1, 113, -91, -123, -43, -83, 118, -19, 32, -99, -112}, // Matrix A row
		{-79, -109, 47, 86, 98, 95, -119, -88, 58, 51, -60, -48, 73, -77, -101, 70}, // Matrix A row
		{-92, -13, -18, 21, 93, 116, 70, 44, -52, 8, 52, 56, 101, 94, 9, -5}, // Matrix A row
		{-43, -81, -94, 83, 121, -117, 40, -80, 53, -89, -59, -30, -101, -53, 74, -103}, // Matrix A row
		{60, 49, 15, 51, -36, -79, 48, -110, 102, -92, -91, 82, -89, -104, 72, -77}, // Matrix A row
		{-52, 26, -21, 56, -112, -77, 47, -65, -33, -54, -112, 69, 47, -85, -21, 18}, // Matrix A row
		{65, 59, 36, -75, -37, -69, 78, -71, 65, 120, 83, -65, 83, 53, 66, 121}, // Matrix A row
		{57, 25, 113, -71, -36, 35, 103, -33, 17, -86, -34, 90, 3, -30, -98, -59}, // Matrix A row
		{-123, 18, -67, 22, 28, -127, -14, 4, -24, 14, -4, 48, 95, 101, 78, 103} // Matrix A row
	}; // Matrix A row

  // Matrix B, elem_t, B_rows, B_cols Setup
	elem_t B[B_rows][B_cols] = { // Matrix B row
		{-111, 26, -16, -42, 41, 23, 112, 44, -51, 5, 121, -3, 33, 109, -21, 9}, // Matrix B row
		{117, 0, 95, -105, 121, -42, -86, -40, -116, -29, -69, 112, 19, 105, -13, 68}, // Matrix B row
		{33, -30, 14, -69, 74, -27, 63, -56, -110, -94, -15, -28, 15, 118, -108, -27}, // Matrix B row
		{15, 84, -111, 54, -30, -105, -38, -65, -18, 33, -74, -1, 104, 30, 10, 41}, // Matrix B row
		{-23, 54, 75, 28, 118, -79, -80, 68, 66, 19, -78, -32, 42, 55, -55, 117}, // Matrix B row
		{25, -7, 88, -128, 104, -47, 27, -118, 13, -42, 91, -73, -109, 36, -14, 43}, // Matrix B row
		{10, 90, -58, 86, 19, 93, -109, -81, 5, 9, -20, -11, 105, -74, -75, -59}, // Matrix B row
		{113, 116, 112, -4, 90, 45, -32, -7, 59, -26, -81, 62, 5, 62, -82, -11}, // Matrix B row
		{-24, -79, 14, -10, 22, -118, 81, 38, -74, -79, 98, 121, -82, -73, 91, -110}, // Matrix B row
		{74, -84, -116, 66, -92, 21, -112, 40, -99, 100, 63, -34, -81, -50, 57, 19}, // Matrix B row
		{-75, 3, 87, -63, 25, 106, -98, 99, -43, -78, 3, -103, 28, 0, 25, 13}, // Matrix B row
		{106, -97, 5, -75, 32, -115, 73, 65, 30, -83, 107, 48, -26, -30, 19, -77}, // Matrix B row
		{100, -123, -83, -43, -18, 48, 4, -124, -13, -94, 49, 31, -121, -27, -10, -35}, // Matrix B row
		{-107, 60, -72, -118, -57, 52, 2, 24, 45, -115, 20, 83, 86, 1, -115, 114}, // Matrix B row
		{-42, 16, 17, -36, -52, -104, 21, 105, 111, 89, -44, -51, -119, 59, 52, -52}, // Matrix B row
		{69, -59, 2, 21, 121, 120, 117, 36, -20, -52, -107, 120, -55, 11, 44, 109} // Matrix B row
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