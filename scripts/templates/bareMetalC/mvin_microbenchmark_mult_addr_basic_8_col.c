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
#define A_cols 8

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
		{-16, -42, 32, -78, 69, -10, -9, 51}, // Matrix A row
		{78, 72, -89, 12, 88, 101, 62, -124}, // Matrix A row
		{79, 122, -60, 110, 118, 97, 21, -59}, // Matrix A row
		{-7, 117, 116, -115, -97, 103, 31, -43}, // Matrix A row
		{-88, -11, 116, 64, 7, -57, 68, 120}, // Matrix A row
		{-29, 33, -4, -43, 5, -4, 65, -110}, // Matrix A row
		{1, 88, 13, 78, -61, 69, 62, -93}, // Matrix A row
		{81, 53, 109, 42, -68, -117, 23, -33}, // Matrix A row
		{-51, 21, -87, -9, 21, 79, -53, -110}, // Matrix A row
		{-17, 15, -74, 103, 39, 84, 82, -75}, // Matrix A row
		{-75, -46, -87, 36, 36, -100, 69, -90}, // Matrix A row
		{-104, 41, 11, 49, -103, 117, -61, -98}, // Matrix A row
		{78, 60, -64, 50, 76, -58, 95, 28}, // Matrix A row
		{75, -67, -55, -22, -5, 70, -27, 36}, // Matrix A row
		{6, -8, 58, -72, 110, -38, -52, -67}, // Matrix A row
		{-103, 66, -29, 4, -1, -38, 57, -67} // Matrix A row
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