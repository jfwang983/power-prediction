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
		{92, 40, 85, -50, 36, -103, 107, 71, 38, 64, -127, -21, 127, 39, -95, 55}, // Matrix A row
		{-12, 92, -22, -73, 7, 24, 5, 97, -27, -99, 35, 102, -51, 14, 123, -127}, // Matrix A row
		{-104, 52, -44, -67, 68, -106, 5, 65, 28, -99, -36, 101, -123, 104, -52, -4}, // Matrix A row
		{-124, -86, 51, -66, 67, 107, -34, -98, -98, -52, -128, 81, -31, 36, -18, -73}, // Matrix A row
		{-105, -49, -1, -54, 106, -9, 13, 124, 84, -47, -63, 19, -123, -64, -87, 105}, // Matrix A row
		{-63, -34, 5, -100, 24, -93, -21, -103, 0, -29, -21, -33, -35, 102, -44, -82}, // Matrix A row
		{41, -101, 105, 6, 8, 72, 81, 7, 19, 27, -117, -113, 58, 10, 57, -38}, // Matrix A row
		{23, -121, 99, -105, -49, -89, 114, 74, -118, 126, 68, 121, -82, 18, 108, 100}, // Matrix A row
		{22, -34, 98, -57, -7, 87, -107, -68, 127, -51, 8, 119, -55, -102, 82, 42}, // Matrix A row
		{4, 118, -122, 46, -30, 47, 125, -40, -19, 27, -26, 107, -118, -12, -3, -99}, // Matrix A row
		{93, 4, 79, 76, -84, -30, 6, 47, -4, 58, -25, -41, 122, 57, -46, -1}, // Matrix A row
		{58, 2, -96, 122, -60, -69, 32, -19, -90, -110, 81, -82, -100, -104, -47, 86}, // Matrix A row
		{-48, 102, -35, -98, -14, -125, -80, -13, -55, -84, -72, -27, 90, 41, -24, 6}, // Matrix A row
		{125, 50, 56, 36, 7, -67, 107, 77, 80, -97, -72, -26, -112, 86, 122, -66}, // Matrix A row
		{-124, 3, -104, -36, 40, -77, 13, -3, 75, 97, -101, -36, 19, -104, -109, -125}, // Matrix A row
		{83, 88, -18, -103, -36, -126, -9, 14, 32, 106, 80, -39, 86, 125, -54, 110} // Matrix A row
	}; // Matrix A row

  // Matrix B, elem_t, B_rows, B_cols Setup
	elem_t B[B_rows][B_cols] = { // Matrix B row
		{-102, 46, 72, -20, -7, -123, -13, -78, 83, 91, -11, 28, -119, 11, 113, 106}, // Matrix B row
		{107, -86, -101, 34, 28, 66, 76, -64, -116, -67, -20, 119, -49, 125, -105, -109}, // Matrix B row
		{73, 114, -69, -23, 74, -53, -94, -46, -45, 0, 96, 104, 87, -62, 12, -16}, // Matrix B row
		{-81, -111, 124, -103, -52, 67, -3, -116, -55, 90, 3, -43, -57, 72, -24, -29}, // Matrix B row
		{59, 4, -38, 34, -25, 91, 82, -11, -128, 124, 105, -85, -80, 38, 100, -85}, // Matrix B row
		{-112, -50, -109, -64, -87, 113, -64, -113, -79, 0, -34, -55, 56, 53, -97, -84}, // Matrix B row
		{-78, 117, -38, 81, -116, 34, 4, -106, -16, 102, -127, -39, 21, 13, 30, -9}, // Matrix B row
		{106, -10, 97, -58, -30, -64, 14, 11, 39, -77, -76, -24, 10, 79, 1, -68}, // Matrix B row
		{34, 65, -53, -76, 116, 93, -95, -83, 42, -10, 70, 45, -44, -28, 78, -105}, // Matrix B row
		{127, 57, 125, -16, -43, -46, -3, 29, -82, -21, -86, 7, -27, 13, -60, -120}, // Matrix B row
		{-59, 45, -34, -52, 108, 103, 13, 61, 64, -5, 19, -11, 115, -30, -97, -68}, // Matrix B row
		{83, -24, 74, 17, -108, 26, -28, 106, 95, 47, -2, -20, 94, -43, 25, -84}, // Matrix B row
		{116, 83, -24, -36, -61, 94, 59, 55, -29, 7, 39, 104, 121, 20, 36, -71}, // Matrix B row
		{18, -86, 33, 45, -86, 14, -22, 38, 85, -42, -95, -110, 120, 42, 14, 108}, // Matrix B row
		{-24, -28, 19, -9, -15, -33, 113, 34, 21, -128, 111, -66, -112, -39, 78, 82}, // Matrix B row
		{56, 121, -55, -95, 88, -96, 29, 108, 9, 118, -75, -3, 61, -49, -71, -108} // Matrix B row
	}; // Matrix B row

  elem_t C[C_rows][C_cols];

  uint32_t A_sp_addr = 0;
  uint32_t B_sp_addr = DIM;
  uint32_t C_sp_addr = 1 << 31 | 1 << 30;

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