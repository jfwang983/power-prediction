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
#define B_cols 2
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
		{-75, -70, -116, 32, -6, -10, 64, 32, 84, 60, -91, -89, 17, -109, 59, 43}, // Matrix A row
		{-47, 98, 111, 2, 65, -28, 109, -103, 54, 57, -60, 57, -4, 2, -60, -97}, // Matrix A row
		{-21, 103, 15, 109, 0, -125, 27, 124, 116, 99, -67, 93, -32, -54, -97, 19}, // Matrix A row
		{-45, 19, -123, 102, -117, 41, -83, 28, -35, -67, 127, 121, -91, 106, -12, -97}, // Matrix A row
		{-79, 69, -70, 65, 50, 97, 88, 122, 8, 60, 34, 124, -9, -2, 22, 36}, // Matrix A row
		{-8, 59, 8, -22, -115, -54, -124, -84, 34, 33, 116, 119, 107, -123, 108, 57}, // Matrix A row
		{112, -9, -82, 123, -27, 120, 123, 113, 73, -24, 51, 44, -36, -113, 12, -11}, // Matrix A row
		{11, 103, 8, -67, 58, -70, -77, 109, 70, 67, 51, 39, 99, -55, -34, 21} // Matrix A row
	}; // Matrix A row

  // Matrix B, elem_t, B_rows, B_cols Setup
	elem_t B[B_rows][B_cols] = { // Matrix B row
		{2, 84}, // Matrix B row
		{-52, 59}, // Matrix B row
		{34, 32}, // Matrix B row
		{-35, -118}, // Matrix B row
		{123, -55}, // Matrix B row
		{-32, -20}, // Matrix B row
		{-87, -124}, // Matrix B row
		{-125, -10}, // Matrix B row
		{-104, 36}, // Matrix B row
		{50, -53}, // Matrix B row
		{101, 98}, // Matrix B row
		{-100, 76}, // Matrix B row
		{-120, -106}, // Matrix B row
		{76, -86}, // Matrix B row
		{89, -45}, // Matrix B row
		{-43, 116} // Matrix B row
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