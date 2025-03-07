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
		{-108, -105, 64, -98, 44, 63, -16, -45, -16, -5, -2, -9, 106, -80, 123, -72}, // Matrix A row
		{0, -125, -59, -30, 30, -68, -114, 34, -34, 93, -101, -5, -13, -67, 117, 102}, // Matrix A row
		{-31, 47, -87, 75, -69, 77, -71, 98, 71, -60, 2, -60, 1, 0, 40, 99}, // Matrix A row
		{-50, -8, -71, 90, -111, -103, 48, -1, -14, -83, -5, 24, 50, -99, 39, 108}, // Matrix A row
		{-85, 33, -25, -21, 55, 118, 86, -68, 55, -28, 5, -55, 97, -11, 25, 72}, // Matrix A row
		{-73, -99, 103, -86, 126, 101, -103, 68, 8, 84, 76, 22, -32, 115, -95, 5}, // Matrix A row
		{-71, -121, 111, 76, -24, -66, -52, 81, -55, 71, 72, 20, -122, 117, -92, -41}, // Matrix A row
		{51, -58, -7, -82, -115, 101, 105, -57, 12, 90, -121, 3, -108, 55, -84, -106}, // Matrix A row
		{-67, -127, 86, -37, -126, 92, -105, 108, 93, 77, 9, 46, 53, -123, -33, -91}, // Matrix A row
		{-18, 2, -66, -91, 118, 89, 110, -44, 87, 0, 100, 25, 114, 44, 103, -41}, // Matrix A row
		{104, -49, 100, 101, 118, -33, 115, -125, -125, -16, 105, -44, 3, 32, -64, 16}, // Matrix A row
		{-47, -69, -23, 105, -38, -14, 7, 82, -53, -7, -99, 95, -10, 52, 47, -6}, // Matrix A row
		{-5, 87, -12, 88, -54, -76, -104, -37, -49, -85, -103, 17, 44, -70, -26, -57}, // Matrix A row
		{89, 125, -62, 79, 48, 37, 107, -99, -49, 18, -18, -93, 92, -95, 42, -3}, // Matrix A row
		{-41, 9, 88, -87, -75, -54, -57, -57, -117, -33, 75, 3, 50, -44, -90, -123}, // Matrix A row
		{-76, -16, 118, -97, 93, -114, 10, 68, 1, 124, -107, -57, -16, 83, -51, 121} // Matrix A row
	}; // Matrix A row

  // Matrix B, elem_t, B_rows, B_cols Setup
	elem_t B[B_rows][B_cols] = { // Matrix B row
		{-110, 92, 57, 73, -28, 67, -15, 6, 23, -71, -101, 125, -6, 48, 124, -45}, // Matrix B row
		{102, 49, -96, -53, 81, 57, -71, -111, -54, -101, -96, -101, -40, -84, -31, 41}, // Matrix B row
		{60, -80, -6, -104, 56, 92, -12, 56, 79, 105, 110, -91, -29, -101, 127, -73}, // Matrix B row
		{57, 89, -92, -17, -33, 38, -41, 7, 34, -18, -57, 46, -4, -98, 76, 76}, // Matrix B row
		{101, -76, 24, -6, 25, -88, -99, -26, 88, -3, -87, 42, 112, 53, -90, -17}, // Matrix B row
		{13, -66, -106, -57, -84, 13, 23, -52, -72, 45, -116, -36, -19, -65, -48, 113}, // Matrix B row
		{-104, 69, 39, 126, 22, -104, -126, 5, 89, 114, -11, -20, 15, -118, -87, 15}, // Matrix B row
		{-76, -11, 84, -101, -98, -35, -71, 19, 127, -73, -78, -119, -64, -95, 50, 46}, // Matrix B row
		{-103, -83, -27, -1, 20, -32, 72, 40, -88, 17, -31, -35, -87, 100, -7, 34}, // Matrix B row
		{65, -103, -19, -94, -66, 51, -79, 92, 57, -76, -104, -89, 58, -115, -23, 105}, // Matrix B row
		{-12, 8, -40, -76, 24, 92, -72, -79, -6, -75, 100, -98, 36, 39, 53, -120}, // Matrix B row
		{-82, 123, 11, 56, -27, -33, -90, -28, 28, -28, -35, -92, -11, 39, 53, 28}, // Matrix B row
		{88, 109, 16, 16, 97, -18, -1, 121, -128, -69, 120, -9, 125, 43, 16, -84}, // Matrix B row
		{-38, 58, 76, 56, 90, -70, -12, 126, -98, 51, -109, 26, 11, -98, 2, 36}, // Matrix B row
		{102, -97, -5, -82, -9, 74, -77, 55, -90, -83, 41, -17, 69, 97, -84, 9}, // Matrix B row
		{-37, -96, -104, -57, 82, 14, -47, 87, 0, 103, 119, -21, 18, -107, -25, -73} // Matrix B row
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