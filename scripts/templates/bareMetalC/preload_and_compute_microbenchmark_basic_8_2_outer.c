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
		{110, -106, -116, 29, -15, -108, 105, 16, -41, 127, 15, 110, -70, 118, 121, -5}, // Matrix A row
		{-56, -119, 108, -92, 36, -119, 71, 62, 77, 12, 8, -13, -1, 62, 108, 90}, // Matrix A row
		{64, -112, 96, -82, 26, 123, 112, -35, 61, 9, 32, 115, -12, -41, -15, -12}, // Matrix A row
		{-48, 75, 100, 14, -53, -123, -88, -126, -72, -64, -91, 98, -72, 80, -41, -117}, // Matrix A row
		{92, 82, 115, -9, 6, -105, -4, 54, 10, 38, -19, -65, -22, -10, -42, 30}, // Matrix A row
		{39, -95, 24, 56, 113, 127, 65, 67, -60, 31, -88, -33, -14, 122, 53, 126}, // Matrix A row
		{-14, -109, 72, -59, -123, 108, -103, 122, -94, 1, 26, 125, -96, -125, -43, 57}, // Matrix A row
		{21, 37, -95, 26, -16, 125, 101, -47, 100, 43, -109, -84, -107, -32, -120, 6} // Matrix A row
	}; // Matrix A row

  // Matrix B, elem_t, B_rows, B_cols Setup
	elem_t B[B_rows][B_cols] = { // Matrix B row
		{0, 110}, // Matrix B row
		{107, -58}, // Matrix B row
		{-125, 68}, // Matrix B row
		{-122, -76}, // Matrix B row
		{-41, -26}, // Matrix B row
		{44, 49}, // Matrix B row
		{104, -54}, // Matrix B row
		{10, 88}, // Matrix B row
		{23, 114}, // Matrix B row
		{88, 48}, // Matrix B row
		{2, -76}, // Matrix B row
		{-67, 51}, // Matrix B row
		{-90, -71}, // Matrix B row
		{118, -99}, // Matrix B row
		{26, -4}, // Matrix B row
		{-119, 16} // Matrix B row
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