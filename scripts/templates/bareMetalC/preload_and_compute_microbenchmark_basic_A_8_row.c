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
		{-35, 105, 88, 125, -67, 19, 55, -68, -55, 125, -86, 84, -1, -69, -25, 50}, // Matrix A row
		{-99, 83, 115, -100, -110, -106, -108, -119, -85, -17, 87, 67, -92, 7, 34, -107}, // Matrix A row
		{52, 103, 99, 59, 36, 47, -74, -30, -40, -12, 53, 76, -126, 80, -109, -83}, // Matrix A row
		{-118, -70, -32, -120, -49, 24, -60, 104, 73, -105, 74, 84, -76, 66, -53, 94}, // Matrix A row
		{28, 60, 16, -9, 115, -68, -71, 85, 114, 123, -92, -85, -7, 21, -84, 3}, // Matrix A row
		{-32, -23, -67, 20, 93, 121, -84, -79, -38, -34, -31, 13, -89, 71, -75, 120}, // Matrix A row
		{100, 13, -125, 22, -48, -41, 16, -97, -53, 2, 28, -67, 95, -25, 94, -53}, // Matrix A row
		{-2, 8, 69, 107, 38, -63, -74, 61, 93, -51, 84, 101, -32, 71, 54, -75} // Matrix A row
	}; // Matrix A row

  // Matrix B, elem_t, B_rows, B_cols Setup
	elem_t B[B_rows][B_cols] = { // Matrix B row
		{-8, -33, 117, 2, 96, -10, 106, 0, 57, -23, -113, -124, -1, 33, -95, 49}, // Matrix B row
		{-102, 12, 75, -6, 54, 19, -77, 101, -30, 15, -63, 7, 89, 109, -58, 4}, // Matrix B row
		{-95, -28, 68, 57, -1, -125, -128, 25, 64, -81, -26, -94, 87, 78, 83, 38}, // Matrix B row
		{116, -35, -20, -7, 89, 73, 111, 77, -110, 39, 96, 71, 32, 53, 48, 5}, // Matrix B row
		{94, 117, -39, -29, -51, -128, 101, 89, -70, 60, 48, -80, 55, -94, 70, 114}, // Matrix B row
		{-15, 125, 66, -123, 23, -50, 127, 112, 89, -68, 108, -93, 92, -89, 33, -26}, // Matrix B row
		{-35, -42, 40, -64, 40, -42, 59, 94, -17, -36, -88, 61, -75, -4, 45, -72}, // Matrix B row
		{-61, 115, 98, -104, -123, 80, -32, 17, 35, -115, -100, 97, -51, 6, 97, -110}, // Matrix B row
		{-24, 102, -95, 104, 113, -111, 3, 16, 15, 107, -66, -2, -3, -61, -98, -105}, // Matrix B row
		{19, -45, -82, -20, 47, 92, -52, 12, 34, -33, 108, -31, 102, 83, 100, 10}, // Matrix B row
		{28, -104, 35, 79, -26, -99, 68, -20, -128, 21, 22, -103, 1, -81, -87, -49}, // Matrix B row
		{67, -6, 98, 2, 78, -127, -92, 63, -5, -41, 125, -55, 85, 58, 57, -10}, // Matrix B row
		{111, -79, -7, -38, -19, -5, 84, -59, 98, -64, -56, 71, 13, 84, 96, 126}, // Matrix B row
		{-72, 19, 73, 6, 120, -107, 115, -57, -8, 43, 61, -28, 78, -96, 75, 102}, // Matrix B row
		{53, -40, 46, -18, -8, -117, -73, 14, 95, -90, -106, 31, 23, 111, -89, 22}, // Matrix B row
		{-115, 98, 50, -16, -43, -55, -81, -12, -106, -123, -38, 125, 85, -45, -48, 80} // Matrix B row
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