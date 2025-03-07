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
		{48, -47, 101, 5, -120, 88, 102, -2, -64, 96, -36, 72, -113, 20, 105, 37}, // Matrix A row
		{106, 68, 74, -4, 3, 102, -125, -32, 82, 33, -55, -18, 60, 73, -84, 54}, // Matrix A row
		{-103, 35, 11, -53, 114, -32, 1, 94, -13, -121, -14, 38, -18, 2, 89, -14}, // Matrix A row
		{61, 69, 26, 88, -104, 74, -111, -85, -30, 47, 103, 46, 34, 66, 30, 82} // Matrix A row
	}; // Matrix A row

  // Matrix B, elem_t, B_rows, B_cols Setup
	elem_t B[B_rows][B_cols] = { // Matrix B row
		{-6, -107, -74, -78, 29, 75, -13, -46}, // Matrix B row
		{56, 72, -85, -52, 97, 106, 92, 22}, // Matrix B row
		{-84, 5, 69, -11, 29, 113, -8, -111}, // Matrix B row
		{81, 32, 118, -21, -128, -74, -82, 28}, // Matrix B row
		{112, 29, -93, -46, -76, 108, 35, -77}, // Matrix B row
		{94, -3, 74, -1, -33, 114, 124, -46}, // Matrix B row
		{-98, 124, 101, 102, -93, 44, 14, -42}, // Matrix B row
		{-97, -46, -53, 47, 124, 23, 88, 116}, // Matrix B row
		{38, -124, 69, -15, -103, 80, 50, 38}, // Matrix B row
		{-94, 82, 121, 7, 47, -95, -111, -18}, // Matrix B row
		{50, -37, -57, -9, -118, -105, -16, 106}, // Matrix B row
		{-27, -81, 15, -40, -34, -3, -125, -70}, // Matrix B row
		{56, -1, 31, -35, -31, 35, -110, 30}, // Matrix B row
		{42, 49, 91, 27, 1, -70, 63, -67}, // Matrix B row
		{14, 16, -112, -108, -27, -74, 75, -100}, // Matrix B row
		{79, 31, 49, -63, -31, -116, -89, -12} // Matrix B row
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