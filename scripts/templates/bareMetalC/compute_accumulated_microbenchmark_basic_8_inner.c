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
#define A_rows 16
#define A_cols 8
#define B_rows 8
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
		{105, -55, 7, 127, -34, -36, -28, -10}, // Matrix A row
		{85, -54, -118, -5, -100, -126, 26, -11}, // Matrix A row
		{70, 83, -100, 112, -71, -101, -128, 89}, // Matrix A row
		{121, -92, -84, 80, -26, -55, 95, 106}, // Matrix A row
		{36, 101, -84, -127, -69, -96, -95, 36}, // Matrix A row
		{-109, 40, -6, 75, 45, 42, 74, 91}, // Matrix A row
		{-66, -105, 30, 63, 62, 103, 102, -114}, // Matrix A row
		{-126, -65, -121, -59, -116, 47, 10, -83}, // Matrix A row
		{46, 26, 9, 13, 68, -79, -70, -64}, // Matrix A row
		{96, 7, -55, 66, 58, 49, -71, -12}, // Matrix A row
		{-23, 122, 127, -43, 14, 57, 26, -13}, // Matrix A row
		{2, 35, 78, -23, -4, -14, -49, 85}, // Matrix A row
		{27, 38, 35, -108, -37, -120, -8, 37}, // Matrix A row
		{73, -82, 72, -116, 64, 37, -107, 8}, // Matrix A row
		{36, 81, 85, 24, 1, 88, 80, 114}, // Matrix A row
		{-44, -29, -123, 24, -40, -71, -108, -108} // Matrix A row
	}; // Matrix A row

  // Matrix B, elem_t, B_rows, B_cols Setup
	elem_t B[B_rows][B_cols] = { // Matrix B row
		{102, 9, -18, 34, -69, 16, -1, 46, 35, -115, 12, 122, -65, -112, -26, -109}, // Matrix B row
		{79, 77, 35, 15, -52, 97, -55, -59, -91, -113, -29, 85, 38, 43, -47, 83}, // Matrix B row
		{61, 125, -17, -93, -35, 13, 35, -86, -91, -51, 125, -35, -41, -2, -56, 122}, // Matrix B row
		{-107, 46, 64, -76, 102, -53, 73, -72, -119, 26, 60, 127, 111, 117, -107, 51}, // Matrix B row
		{-3, -5, 12, -74, 102, 33, -111, 18, 117, 94, -21, -62, -98, -86, 48, -69}, // Matrix B row
		{10, -118, 1, -49, 23, -18, -15, -59, -28, 6, -100, 43, -103, 25, 77, 53}, // Matrix B row
		{124, -46, -35, 34, -106, -99, -79, -79, -8, 70, -86, -64, -83, -37, 8, 13}, // Matrix B row
		{119, 26, -21, 34, -21, -17, 121, -27, -104, -68, -1, -3, -122, -111, 22, 36} // Matrix B row
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