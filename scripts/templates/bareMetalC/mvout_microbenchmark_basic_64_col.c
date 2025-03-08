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
#define C_cols 64

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
		{34, -3, -105, 121, 22, 47, -49, 121, -33, 111, -2, 30, -33, -116, 113, -88}, // Matrix A row
		{-67, -50, -31, 83, -11, -46, 19, -103, 75, -40, 54, -17, -3, -110, -121, 71}, // Matrix A row
		{19, -12, -96, 64, -24, -22, -54, -7, 26, 23, 74, 31, 48, 30, 36, -10}, // Matrix A row
		{66, -36, -57, 29, 86, 108, 91, 104, -16, 120, -26, -39, 26, -71, 119, -14}, // Matrix A row
		{-123, -45, 34, -75, 120, 117, 81, -60, -6, -71, -113, -105, 86, 70, -32, 70}, // Matrix A row
		{-56, -68, -14, -75, -114, 121, -83, 15, 18, 94, -112, 53, -30, -82, -77, 117}, // Matrix A row
		{42, 91, -117, -58, 115, 124, -27, -20, -112, 40, -116, 24, 13, 79, -6, 68}, // Matrix A row
		{8, 122, 98, 82, -83, 113, 8, 12, -37, -85, 102, 113, 108, 76, 53, -45}, // Matrix A row
		{86, -32, 21, 54, -119, 100, 102, -90, 72, 60, 126, 26, -52, 47, 54, 107}, // Matrix A row
		{-111, 20, 94, 83, -4, 52, 105, 5, 22, -11, -123, 0, 127, 16, -80, -32}, // Matrix A row
		{118, -93, 80, 104, -55, -24, -46, -70, 106, -94, -99, 100, -68, -69, -73, 100}, // Matrix A row
		{110, -27, 67, -94, 13, -75, 21, 93, -78, -49, -1, 71, -89, 114, 9, -55}, // Matrix A row
		{-30, -51, 36, 78, -65, -78, 93, 2, 116, 84, -21, 125, 69, -122, 121, 4}, // Matrix A row
		{39, 9, 100, 32, -120, -82, 36, 81, 22, -47, -6, -58, 101, 95, -72, 45}, // Matrix A row
		{85, 108, -22, 37, -19, 94, -12, -40, 25, -2, 30, -72, -120, 24, -115, -25}, // Matrix A row
		{24, 111, 66, 123, -75, -96, -45, -89, 27, 10, 28, 33, 35, -114, -74, -16} // Matrix A row
	}; // Matrix A row

  // Matrix B, elem_t, B_rows, B_cols Setup
	elem_t B[B_rows][B_cols] = { // Matrix B row
		{-123, -57, -80, 100, -48, -79, 75, -120, 2, -42, -39, -98, 2, -116, 42, -68}, // Matrix B row
		{7, 95, 37, -122, 120, 80, -49, -83, 5, 62, 115, 57, -37, 59, 65, 7}, // Matrix B row
		{-99, 109, -49, -63, 94, 49, 98, 91, 3, -89, -50, 124, -3, -28, -92, -3}, // Matrix B row
		{-97, -43, 43, 7, 19, -36, -64, -7, 87, 2, 90, -111, 23, 89, -126, -77}, // Matrix B row
		{-87, 41, 75, 30, 14, -92, -51, -93, 22, -118, -57, -73, 36, 67, 13, -44}, // Matrix B row
		{-126, -50, -49, 21, 7, -115, -83, 111, 31, 15, 102, -91, -78, 3, -56, 15}, // Matrix B row
		{66, -68, -5, 43, -110, -93, -91, -44, -107, -110, -30, 21, 127, 44, 90, 45}, // Matrix B row
		{-31, -115, -42, -71, -122, -69, 99, 11, -69, -52, 71, 34, -16, -111, -80, -82}, // Matrix B row
		{2, 92, -112, 121, 5, 49, 125, 55, 28, -85, -94, -23, -32, -99, 119, 41}, // Matrix B row
		{-110, -2, 124, 119, -49, 5, 86, 119, 124, -97, 81, 78, -80, -108, -15, -71}, // Matrix B row
		{30, 97, -73, 52, 125, 50, -33, 86, 35, 39, 80, 104, 122, 22, -24, -38}, // Matrix B row
		{-9, -38, -49, -83, 0, -64, 30, 27, -40, 12, 85, -67, 85, 71, -88, -77}, // Matrix B row
		{-40, 97, 98, -64, -88, 48, 111, -94, -64, 77, 112, 106, -26, -39, -111, 72}, // Matrix B row
		{-41, -51, 102, 41, 76, 5, -74, -58, -84, 33, 97, -23, -102, 112, 111, 45}, // Matrix B row
		{-125, 3, -60, 72, -39, 123, -120, 12, -32, 61, 96, -115, -3, 119, -31, 26}, // Matrix B row
		{-63, -127, -15, 107, -59, 92, 59, 112, 109, 18, -21, -100, 119, -120, -77, 25} // Matrix B row
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
  gemmini_config_st(C_cols);
  // printf("gemmini_extended3_config_ld(%u, MVIN_SCALE_IDENTITY, false, 0);\n", A_cols);
  // printf("gemmini_extended3_config_ld(%u, MVIN_SCALE_IDENTITY, false, 1);\n", B_cols);
  // printf("gemmini_config_ex(WS, NO_ACTIVATION, 0);\n");
  // printf("gemmini_config_st(%u);\n", C_cols);

  // Move in matrices for initialization
  gemmini_extended_mvin(A, A_sp_addr, A_cols, A_rows);
  gemmini_extended_mvin2(B, B_sp_addr, B_cols, B_rows);
  // printf("gemmini_extended_mvin(A, %p, %u, %u);\n", A_sp_addr, A_cols, A_rows);
  // printf("gemmini_extended_mvin(B, %p, %u, %u);\n", B_sp_addr, B_cols, B_rows);

  gemmini_extended_preload(B_sp_addr, C_sp_addr, B_cols, B_rows, B_cols, A_rows);
  gemmini_extended_compute_preloaded(A_sp_addr, GARBAGE_ADDR, A_cols, A_rows, B_cols, B_rows);
	// printf("gemmini_extended_preload(%p, 0x%x, %u, %u, %u, %u);\n", B_sp_addr, C_sp_addr, B_cols, B_rows, B_cols, A_rows);
	// printf("gemmini_extended_compute_preloaded(%p, 0x%x, %u, %u, %u, %u);\n", A_sp_addr, GARBAGE_ADDR, A_cols, A_rows, B_cols, B_rows);

  // Main microbenchmark code
  for(int i = 0; i < iterations; i++) {
    gemmini_extended_mvout(C, C_sp_addr, C_cols, C_rows);
  	// printf("gemmini_extended_mvout(C, %p, %u, %u);\n", C_sp_addr, C_cols, C_rows);
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