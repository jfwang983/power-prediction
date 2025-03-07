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
#define A_rows 4
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
		{9, 55, -45, -84, -68, -62, -124, 95, 123, 117, 34, -65, -52, -9, -93, -8}, // Matrix A row
		{81, 82, 43, -48, -73, -98, -56, -63, -78, 34, 41, -67, -44, 9, 64, -36}, // Matrix A row
		{34, -28, -72, 42, 42, 31, 116, 72, 106, -38, -83, -121, -49, -62, 127, -76}, // Matrix A row
		{39, -53, 123, -63, 36, -21, -2, -84, -96, -61, -60, 13, -68, 106, 60, -109} // Matrix A row
	}; // Matrix A row

  // Matrix B, elem_t, B_rows, B_cols Setup
	elem_t B[B_rows][B_cols] = { // Matrix B row
		{-43, 50, 56, 21, -47, 2, -59, 114}, // Matrix B row
		{53, 15, -78, -123, 87, -115, -125, 91}, // Matrix B row
		{-82, -126, 21, 91, -89, 116, 63, -92}, // Matrix B row
		{-110, 58, -107, -100, 98, 84, 124, 75}, // Matrix B row
		{-35, -62, -125, -32, -9, 67, -74, -17}, // Matrix B row
		{3, -6, -6, -50, -38, 98, 15, 69}, // Matrix B row
		{-87, -64, 94, 106, -68, 55, 30, -121}, // Matrix B row
		{-39, -109, 92, -97, -7, -6, -84, 30}, // Matrix B row
		{55, 55, 121, 50, -89, -28, -30, 98}, // Matrix B row
		{-96, -32, -15, -41, 127, 127, 2, -93}, // Matrix B row
		{-10, 109, -46, 91, -79, 83, -117, 64}, // Matrix B row
		{28, 108, -63, -89, 26, -42, -35, 21}, // Matrix B row
		{-85, -60, -93, -19, 23, 16, 48, 78}, // Matrix B row
		{-52, 42, 91, 121, 23, -61, -109, 100}, // Matrix B row
		{91, -103, -82, -38, -6, 58, 57, 103}, // Matrix B row
		{-115, 35, -45, -13, 52, -121, -98, -1} // Matrix B row
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