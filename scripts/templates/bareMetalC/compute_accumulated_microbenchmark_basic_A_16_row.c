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
		{-54, 124, -39, 21, -70, -83, 93, -17, -28, -87, 33, 83, 49, 49, 15, -35}, // Matrix A row
		{-93, 107, -122, 44, -31, 105, 54, 36, 54, 4, 6, -43, -20, 50, -55, -51}, // Matrix A row
		{127, -42, 15, 116, 32, -28, 31, -94, -95, 50, 75, 99, -102, 73, 123, 84}, // Matrix A row
		{-62, 6, 57, -47, 81, -21, -10, -59, -57, 25, -31, -51, -72, 113, -104, -13}, // Matrix A row
		{-106, -124, -119, -71, 51, -87, 27, -94, -3, -17, 67, -6, 105, 90, 107, 7}, // Matrix A row
		{-115, -106, -9, 66, 78, 16, -115, -91, -45, -98, 83, 85, 70, -124, 18, 53}, // Matrix A row
		{-64, -13, -2, 24, -55, -119, -111, -92, 97, -73, 29, -58, -90, -28, 23, -18}, // Matrix A row
		{-24, -50, 39, -35, 82, 20, -45, -119, -19, -118, 102, 50, 94, 29, -93, 79}, // Matrix A row
		{-116, 120, -37, 125, -60, 40, 10, -125, 120, 78, 98, 46, -113, 108, -77, -119}, // Matrix A row
		{42, 62, -23, 67, 47, 19, -86, 68, 12, 61, 14, 93, -16, 111, -37, 33}, // Matrix A row
		{86, 119, 32, 43, -77, -84, -25, 51, -123, -110, -49, 71, 113, -73, -4, -9}, // Matrix A row
		{123, 117, -96, -15, -64, -42, 107, -84, -121, 124, -104, 125, 93, 62, 55, -41}, // Matrix A row
		{-36, 25, -105, 15, 34, -74, 107, 122, -12, -96, -9, -118, -87, -54, 85, -19}, // Matrix A row
		{-9, -9, 17, -6, 34, -49, 60, 58, 30, -14, -7, -73, -38, -30, -117, 77}, // Matrix A row
		{61, 70, 38, -7, 7, 4, -106, 92, -111, -86, 55, -119, 120, 83, -102, 103}, // Matrix A row
		{52, -61, -34, 49, 123, -44, -39, -59, 97, 91, -61, -74, -69, -104, -50, 20} // Matrix A row
	}; // Matrix A row

  // Matrix B, elem_t, B_rows, B_cols Setup
	elem_t B[B_rows][B_cols] = { // Matrix B row
		{-123, 13, -16, -25, 49, -61, 47, 112, 24, -51, 4, 120, -66, 70, -59, -15}, // Matrix B row
		{110, 15, -21, -28, -5, 117, 96, -56, -103, -79, -60, -97, 125, -104, -79, -27}, // Matrix B row
		{-93, 13, 46, 15, -9, 32, -50, -39, 35, -20, 38, -82, 93, -52, -55, -42}, // Matrix B row
		{-63, 4, 15, -95, 7, 126, -128, -78, 67, -42, -35, 86, -41, -84, -50, -96}, // Matrix B row
		{26, 85, -31, 90, -104, -32, -82, 15, -58, 57, -65, -61, -40, -25, -99, -34}, // Matrix B row
		{74, -41, -64, 74, 87, -52, 90, 107, 39, -72, 78, 46, -5, -12, -95, 103}, // Matrix B row
		{67, -24, -53, 39, 107, -52, 125, 66, 28, -75, 121, 28, -29, -8, -116, 84}, // Matrix B row
		{120, -43, -58, 61, 122, 54, 45, -82, 86, 62, 66, -92, -89, -123, -58, -25}, // Matrix B row
		{-28, -26, 13, -99, -22, -123, -17, 3, -24, 92, -98, -80, 74, 124, 3, -22}, // Matrix B row
		{79, -117, -96, 110, -89, 62, 31, -119, 15, -101, -74, -23, 41, 61, -84, 78}, // Matrix B row
		{1, -90, -125, -109, -57, -50, -60, -106, 118, 17, 89, 71, -4, 38, -62, -54}, // Matrix B row
		{35, -103, 45, -15, -25, -52, -44, -126, 66, 102, 93, 109, -11, -128, -108, -12}, // Matrix B row
		{54, 111, -27, 127, -49, 107, -54, -60, -118, -16, -112, -46, 102, -118, 59, 84}, // Matrix B row
		{-35, 5, -44, -13, -125, -98, 77, 21, 79, -110, 84, 70, 41, -103, 17, -6}, // Matrix B row
		{1, 90, -81, 92, 33, -32, 44, 28, 4, 94, -14, 24, -44, 7, -118, -75}, // Matrix B row
		{34, 85, -49, 78, -97, 78, 12, 47, 91, 47, 112, 118, 3, -115, -95, -84} // Matrix B row
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