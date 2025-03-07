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
		{-7, 4, 13, 95, 106, 37, 77, 38, 55, 82, 2, -66, 17, 46, 15, 58}, // Matrix A row
		{24, 113, 69, 117, 125, 97, 53, 68, -17, 114, -121, -89, 111, 63, 36, 70}, // Matrix A row
		{-98, 108, 0, -86, -66, 108, 119, 121, 47, 16, -19, -38, -101, 34, -99, 83}, // Matrix A row
		{20, 2, 113, -101, 82, -93, 67, 91, 104, -41, 66, 92, 36, 103, -40, 22}, // Matrix A row
		{94, -32, 44, 67, -62, -103, -83, 17, -87, -16, 62, -88, 16, 64, 6, 92}, // Matrix A row
		{-32, 25, -55, 43, -23, -12, 76, -38, -66, 57, 110, 72, 106, 41, -19, -20}, // Matrix A row
		{-8, -113, -20, -81, 26, -69, 65, -116, -101, 69, 94, 18, -74, -6, -35, 67}, // Matrix A row
		{-110, -73, -45, 25, 123, -96, 64, 63, -128, 110, 67, -54, 94, -122, 20, -127} // Matrix A row
	}; // Matrix A row

  // Matrix B, elem_t, B_rows, B_cols Setup
	elem_t B[B_rows][B_cols] = { // Matrix B row
		{-14}, // Matrix B row
		{121}, // Matrix B row
		{-84}, // Matrix B row
		{0}, // Matrix B row
		{75}, // Matrix B row
		{11}, // Matrix B row
		{28}, // Matrix B row
		{101}, // Matrix B row
		{-32}, // Matrix B row
		{5}, // Matrix B row
		{-34}, // Matrix B row
		{-4}, // Matrix B row
		{-35}, // Matrix B row
		{-98}, // Matrix B row
		{-74}, // Matrix B row
		{5} // Matrix B row
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