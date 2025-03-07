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
		{75, 50, 48, 95, -61, 62, 65, 8, -22, 7, 10, 37, 32, -115, -28, 15}, // Matrix A row
		{87, -64, -12, -92, -84, 62, -3, 53, 77, 112, 27, 107, 46, -36, 93, 87}, // Matrix A row
		{-63, 65, -50, -51, 30, 17, -124, -116, -59, -47, -44, -100, 15, 81, 89, -115}, // Matrix A row
		{-77, 51, -41, -81, 102, -105, -9, -117, 113, -13, -125, 119, -34, -7, 43, 3}, // Matrix A row
		{-63, 97, -9, -108, -84, -65, 125, 89, 37, -78, -61, 95, -53, -18, 69, 113}, // Matrix A row
		{81, 62, -97, -28, 25, 66, -16, -89, 59, -63, -58, 44, -55, -81, 70, 4}, // Matrix A row
		{-33, 18, 114, -72, 102, -49, -88, -29, -77, 112, -82, 121, -91, 89, -58, 33}, // Matrix A row
		{112, 10, -2, -35, 80, -77, 107, -49, 88, -106, 7, -110, 42, 23, 45, -67}, // Matrix A row
		{-106, 63, -61, 3, -33, -109, -21, 112, 28, 66, -92, -56, 95, -109, 32, 80}, // Matrix A row
		{21, 57, -67, -123, -67, 86, -62, -66, -87, -120, -61, 1, 126, -101, 121, -70}, // Matrix A row
		{-49, 96, 50, 10, 41, 98, 26, 21, -86, 81, 77, -97, -26, -37, -69, -1}, // Matrix A row
		{-87, -103, 4, 86, -127, 110, -13, 23, -18, 9, 52, 69, -123, -99, -126, 58}, // Matrix A row
		{-115, 97, -37, -91, -52, -85, 112, -28, 45, -40, -57, -45, -18, -60, -30, 96}, // Matrix A row
		{-49, 1, 52, 68, -1, -113, 10, -99, -84, -19, -20, -56, -22, 115, 38, 74}, // Matrix A row
		{85, 43, 62, -85, -126, -5, -38, 22, 78, -49, 10, -26, -98, 81, 124, -60}, // Matrix A row
		{-13, 75, 124, 113, -52, -13, 44, -94, 79, -74, -84, 81, -30, -57, 91, -5} // Matrix A row
	}; // Matrix A row

  // Matrix B, elem_t, B_rows, B_cols Setup
	elem_t B[B_rows][B_cols] = { // Matrix B row
		{-12, 115, 99, -30, -48, 32, -15, 105}, // Matrix B row
		{-122, 10, -119, -118, -39, -71, 45, -53}, // Matrix B row
		{-82, -57, 66, 110, 0, -8, 25, 63}, // Matrix B row
		{-11, -79, -117, 24, 89, 46, -127, 6}, // Matrix B row
		{-107, 72, 8, 61, -78, -54, -75, 73}, // Matrix B row
		{32, -76, -64, 13, 74, 87, 70, -10}, // Matrix B row
		{11, -17, -126, -21, 61, -82, 114, 30}, // Matrix B row
		{-25, 126, 36, 67, 93, 1, -99, 12}, // Matrix B row
		{55, -97, 67, -102, 9, 5, 83, -85}, // Matrix B row
		{92, 40, 45, -101, -54, -64, -49, 42}, // Matrix B row
		{112, 98, 4, 119, -73, 12, -109, -6}, // Matrix B row
		{12, 8, 32, 80, -42, -70, 22, -69}, // Matrix B row
		{22, -28, -13, 113, 111, 63, -126, -36}, // Matrix B row
		{25, 30, -34, 36, -51, -32, -117, -29}, // Matrix B row
		{-3, 114, 90, -89, -125, 104, 13, -44}, // Matrix B row
		{93, 125, -42, -96, 20, -34, -127, -4} // Matrix B row
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