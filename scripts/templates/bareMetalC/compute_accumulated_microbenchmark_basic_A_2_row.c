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
		{-43, -91, -56, -122, -45, 91, 100, -112, -45, -101, 116, 50, 45, 43, 34, -55}, // Matrix A row
		{91, -42, 28, 24, 17, 33, -87, -14, -34, 125, 56, 108, -80, 99, -10, -84} // Matrix A row
	}; // Matrix A row

  // Matrix B, elem_t, B_rows, B_cols Setup
	elem_t B[B_rows][B_cols] = { // Matrix B row
		{-30, -59, 86, -47, 48, 43, -29, 44, 18, -79, -51, 108, -98, -25, -68, 107}, // Matrix B row
		{-92, 33, -112, 65, -16, -114, -75, 78, 124, -109, -39, 2, 113, -60, -76, -55}, // Matrix B row
		{15, -116, -91, -86, -65, -98, 16, 81, 105, -109, -7, 72, 58, -71, 48, 21}, // Matrix B row
		{62, -86, -46, 115, 116, -100, 4, -1, 122, 33, -17, 15, 43, 21, 100, -33}, // Matrix B row
		{-42, -88, -112, 100, 79, -78, 36, -128, -103, -85, 63, 12, 64, 0, 81, -17}, // Matrix B row
		{-58, 27, 102, -85, 125, -27, -33, 7, 116, -15, 114, 82, 46, 125, -16, 40}, // Matrix B row
		{26, -39, -76, 85, -118, -78, -89, 37, 62, 55, -61, 93, -108, 44, 13, 11}, // Matrix B row
		{40, -85, 112, 127, -75, 65, -81, 18, 21, 30, 26, -57, -35, 63, 117, 114}, // Matrix B row
		{30, 24, 44, 108, 56, 67, -55, 92, 116, -43, 53, 59, 93, 125, -20, -116}, // Matrix B row
		{-4, -113, 52, 104, -29, 2, -51, -84, 104, -37, -2, 37, 119, 9, -26, -37}, // Matrix B row
		{-9, 46, -37, 74, 46, -23, -127, 102, 16, -46, -25, 26, 5, -50, 61, 102}, // Matrix B row
		{77, -104, -120, 59, -20, 102, -35, 20, 82, 5, -12, 113, 21, -103, 119, -61}, // Matrix B row
		{-109, 89, 82, 23, -61, -72, -123, 124, -49, 83, -37, -54, -15, -80, 65, -91}, // Matrix B row
		{-101, 84, 37, 113, 56, 53, -24, -65, -118, -45, 60, -34, -15, 124, 114, -9}, // Matrix B row
		{-102, 1, -73, 105, 9, 52, 24, 17, -90, -1, 117, -8, 30, -14, 3, 75}, // Matrix B row
		{22, 21, -39, 86, -123, 27, -26, -81, -5, -63, -92, -7, -16, 83, -110, -29} // Matrix B row
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