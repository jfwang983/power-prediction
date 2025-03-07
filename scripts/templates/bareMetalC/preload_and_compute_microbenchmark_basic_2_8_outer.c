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
#define A_rows 2
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
		{-56, 76, -15, -32, -5, -111, 24, 15, -96, 87, -34, -4, 68, 62, 15, 21}, // Matrix A row
		{-127, -33, -114, -44, 51, -120, -48, 95, 43, 119, -38, -15, -37, 62, -14, -70} // Matrix A row
	}; // Matrix A row

  // Matrix B, elem_t, B_rows, B_cols Setup
	elem_t B[B_rows][B_cols] = { // Matrix B row
		{112, -93, 117, -62, -89, -2, 37, 87}, // Matrix B row
		{69, -4, 63, -68, 76, -38, -96, -28}, // Matrix B row
		{72, -61, -75, -80, 103, 39, 84, 4}, // Matrix B row
		{30, -64, -83, -28, 2, -31, 65, 124}, // Matrix B row
		{87, -64, 120, -31, 70, 14, 43, -127}, // Matrix B row
		{-78, 101, 57, -51, -63, -43, 10, 79}, // Matrix B row
		{-22, -21, 49, 12, -3, -35, 109, 49}, // Matrix B row
		{26, -126, -50, -42, 26, -64, -83, 42}, // Matrix B row
		{86, 65, 108, -75, -56, 75, 12, 116}, // Matrix B row
		{-58, 25, 65, 119, 114, -10, -60, -36}, // Matrix B row
		{-7, 108, 17, 41, 73, -102, -43, 45}, // Matrix B row
		{102, 41, -15, 113, -39, -116, 56, 102}, // Matrix B row
		{81, -45, -125, -128, 121, -29, -107, 103}, // Matrix B row
		{15, -63, -49, -76, 9, 11, 105, 127}, // Matrix B row
		{-127, -113, 89, 120, 19, 104, -20, -42}, // Matrix B row
		{50, 102, 100, -5, 66, -124, -71, 59} // Matrix B row
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