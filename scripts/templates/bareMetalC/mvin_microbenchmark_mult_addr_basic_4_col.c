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
#define A_cols 4

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
		{-79, -122, -7, -18}, // Matrix A row
		{117, 58, -103, 77}, // Matrix A row
		{123, -55, -48, -74}, // Matrix A row
		{0, 100, -35, 47}, // Matrix A row
		{-39, -72, -55, -86}, // Matrix A row
		{68, 117, -51, 22}, // Matrix A row
		{-80, 29, 106, -88}, // Matrix A row
		{-77, 85, -118, -116}, // Matrix A row
		{-59, -95, -67, -2}, // Matrix A row
		{16, -93, -30, -12}, // Matrix A row
		{40, 47, 123, -82}, // Matrix A row
		{-23, -87, -14, 52}, // Matrix A row
		{106, -84, 97, 107}, // Matrix A row
		{50, -92, -54, -75}, // Matrix A row
		{-37, 85, -125, 13}, // Matrix A row
		{55, 16, -124, -57} // Matrix A row
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