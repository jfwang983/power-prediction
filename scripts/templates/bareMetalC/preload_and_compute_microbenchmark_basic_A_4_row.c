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
		{-117, -84, -98, 71, -65, -35, 50, -66, -27, -9, -64, -22, 72, -26, -124, -16}, // Matrix A row
		{-37, -37, 45, 77, 38, 32, -92, 42, 77, -29, -119, -102, 64, -34, 87, -19}, // Matrix A row
		{95, 117, -81, 78, 114, -87, 119, 2, -10, 121, 85, -29, -81, 74, -58, -63}, // Matrix A row
		{-39, -29, -91, 17, -125, -90, 97, 74, -62, 24, -114, -8, 7, -40, -37, -68} // Matrix A row
	}; // Matrix A row

  // Matrix B, elem_t, B_rows, B_cols Setup
	elem_t B[B_rows][B_cols] = { // Matrix B row
		{15, 58, -2, -110, 57, 5, 7, 46, 65, -74, 67, -51, 8, 119, 104, -63}, // Matrix B row
		{38, 116, 73, 5, -109, 8, -23, -61, 14, 105, -6, -88, 4, 87, 49, 107}, // Matrix B row
		{2, 53, -79, 121, 94, -67, 68, 34, 97, 100, -121, 5, -96, -119, 13, 123}, // Matrix B row
		{113, 68, 18, -57, -5, -99, 108, 7, -98, -85, 96, -31, 7, -112, 104, -99}, // Matrix B row
		{-76, -19, -41, 67, -124, 28, 16, 32, 11, -126, 53, -75, -22, -75, -42, 5}, // Matrix B row
		{103, 61, 10, -35, -34, -79, -22, -13, 107, 91, -60, 95, 52, 109, -22, 70}, // Matrix B row
		{109, -59, -30, -56, -107, 122, -90, 110, -36, -66, -7, 41, 46, -40, 28, 32}, // Matrix B row
		{64, 1, -49, -89, -95, 104, -120, -107, 116, 84, -99, -43, -13, -77, -24, 65}, // Matrix B row
		{69, 3, -46, 26, -33, -72, 51, 44, -123, 37, -34, -118, -27, 91, 107, -52}, // Matrix B row
		{85, 84, 67, 124, -109, 53, -30, -126, -28, 93, -115, 86, -115, -98, -125, -110}, // Matrix B row
		{93, 1, 106, 13, -86, 29, 102, -61, 107, 102, 85, 79, -34, 76, 70, -22}, // Matrix B row
		{-101, 21, -27, -46, 2, -79, 121, -11, 95, 116, 116, 82, 12, -54, 8, -97}, // Matrix B row
		{126, -18, 86, -29, 110, 14, -68, -126, -39, 58, -28, 27, 21, 118, 14, -7}, // Matrix B row
		{71, 61, 65, -21, -119, 104, 46, 112, 69, -37, 24, -48, -15, -39, -81, 119}, // Matrix B row
		{85, -65, 3, 30, 46, -115, 33, 108, 50, 41, -35, -64, 21, -96, 79, -54}, // Matrix B row
		{83, 116, -72, -36, 47, -68, 40, 68, 64, -40, -8, -114, 67, -32, -47, -80} // Matrix B row
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