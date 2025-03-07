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
		{47, 111, -16, 96, -67, 80, -102, 127, 118, -55, -115, -48, 37, 29, 62, -122}, // Matrix A row
		{-14, 21, -120, 75, -86, -94, -16, 56, -47, -78, -64, 64, -118, 76, 3, -110} // Matrix A row
	}; // Matrix A row

  // Matrix B, elem_t, B_rows, B_cols Setup
	elem_t B[B_rows][B_cols] = { // Matrix B row
		{11, -28, -62, 112, 120, -124, 34, -55, -11, -78, 44, 40, -23, 51, 58, -6}, // Matrix B row
		{25, -11, 37, 61, 14, 121, -80, -70, 18, -120, -99, -114, 17, -74, 74, 12}, // Matrix B row
		{-21, -78, 43, 32, -87, 37, 50, -127, -120, -128, 56, -7, -40, 61, 60, 28}, // Matrix B row
		{90, -36, -110, -66, 93, 48, 97, -58, 7, -121, -28, 88, -68, -80, 70, -22}, // Matrix B row
		{-30, 108, -4, 95, 114, -36, -113, -41, 59, 9, -36, 71, 83, 18, 12, -79}, // Matrix B row
		{-55, -46, -100, -96, -98, -67, -41, -127, -50, -52, 92, 101, 70, -89, 109, 60}, // Matrix B row
		{9, 91, 57, -25, -99, -124, -103, -8, 117, 90, -117, -40, -77, 104, 120, -115}, // Matrix B row
		{47, -57, -79, -54, 8, -121, -52, 0, -9, 16, 32, 9, 45, -41, 60, -57}, // Matrix B row
		{107, -108, 34, -85, 64, -15, -28, 43, -20, 10, -24, -98, 90, -54, -46, -114}, // Matrix B row
		{44, 85, 28, -89, -57, -35, -24, -3, 31, 19, 124, 0, 31, 42, -65, 111}, // Matrix B row
		{96, 13, -112, 120, 81, -99, 33, -72, 81, 10, 63, 58, -21, 102, 77, 24}, // Matrix B row
		{100, -115, -119, -33, 47, -56, 33, -15, 119, -69, 87, -26, -90, -2, -123, -96}, // Matrix B row
		{97, 93, 117, -27, 119, -28, -26, -79, -59, -114, 41, 95, 77, 93, 17, 11}, // Matrix B row
		{62, 112, -20, -125, -54, 110, 37, 53, -68, -79, 125, 26, 15, 75, -98, -59}, // Matrix B row
		{77, 115, -69, -74, -97, 60, -112, 5, 23, -95, -20, 69, -49, -16, -117, -122}, // Matrix B row
		{-52, -37, -42, -98, -82, 101, -64, -18, -26, -126, -23, 18, -79, 97, 20, 120} // Matrix B row
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