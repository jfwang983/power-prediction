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
		{-94, -34, 17, -92, -35, -14, 51, 100, 28, 68, -111, 98, -12, -23, -123, -2}, // Matrix A row
		{121, 27, -71, 36, -122, -42, -78, 7, 24, -79, 89, 127, -53, 13, -118, -123}, // Matrix A row
		{33, -88, 39, -91, 86, 48, -6, -109, 79, -4, -124, -123, -101, 80, 110, -68}, // Matrix A row
		{-91, -105, 82, -58, 8, 8, -21, 44, 105, 80, 69, -120, 74, 79, -53, 46}, // Matrix A row
		{23, 80, -81, -53, -51, 30, 58, 5, 73, 77, -24, 107, 76, -78, -80, -13}, // Matrix A row
		{-85, -38, 79, 78, 62, 74, -28, 64, -50, -14, -74, 113, -123, -42, -14, 85}, // Matrix A row
		{-117, 3, -125, 116, 79, 27, -111, -66, -120, -108, 88, -42, -93, 102, -23, 43}, // Matrix A row
		{124, -30, -36, 47, 94, -107, 107, 29, -29, -23, -44, 5, -80, -106, 7, -33}, // Matrix A row
		{127, -114, -118, -30, 0, 48, 2, -7, -32, -99, 109, 108, 1, 112, 28, -127}, // Matrix A row
		{-66, 97, -68, 89, -122, -124, -67, -80, 24, 112, 57, 28, -82, -103, 19, 69}, // Matrix A row
		{108, 113, 114, -71, -37, 49, 82, -46, -73, 95, -108, -4, -127, 18, 14, -18}, // Matrix A row
		{-26, 68, -104, -12, 40, 109, 118, 66, 93, 2, 11, 66, -55, 34, 126, -15}, // Matrix A row
		{104, -84, 94, 43, 76, -57, 31, -104, -62, 59, 91, 1, -109, 101, 99, -88}, // Matrix A row
		{45, -34, 76, -13, 107, -87, 10, 34, -52, 10, 54, -8, -19, -108, -126, 121}, // Matrix A row
		{-64, -103, 22, 1, 126, -100, 37, 59, -118, 99, 59, 93, -5, 72, -125, 25}, // Matrix A row
		{-41, -80, 22, 45, -54, -23, 91, 32, -30, 78, 24, 70, 75, 55, 48, -85} // Matrix A row
	}; // Matrix A row

  // Matrix B, elem_t, B_rows, B_cols Setup
	elem_t B[B_rows][B_cols] = { // Matrix B row
		{55, -27, 35, 17, 67, 67, 79, 6}, // Matrix B row
		{29, -75, 119, -50, 117, -16, 39, 100}, // Matrix B row
		{-109, 110, 99, -46, -43, -73, -52, -32}, // Matrix B row
		{-5, 67, -101, -113, 72, 10, -55, -63}, // Matrix B row
		{52, -7, -81, 44, -118, -118, -40, -36}, // Matrix B row
		{-56, -105, 1, -14, -46, -39, -64, 93}, // Matrix B row
		{8, 82, 15, 116, 126, 68, 121, -119}, // Matrix B row
		{-98, -50, 23, 77, -40, -17, 68, 99}, // Matrix B row
		{0, -24, 91, 15, -105, 7, 24, -17}, // Matrix B row
		{-111, -10, 117, 62, -11, 58, -119, -100}, // Matrix B row
		{50, -71, 97, 42, -5, 5, 1, 4}, // Matrix B row
		{-122, 40, -29, -89, -11, -123, 105, 34}, // Matrix B row
		{13, 57, -108, -38, -16, -42, 91, 21}, // Matrix B row
		{-63, -128, 112, 48, 57, -53, 43, -78}, // Matrix B row
		{-36, 104, 99, -120, -2, 120, 97, 16}, // Matrix B row
		{-65, 114, 73, 112, -49, 38, 122, -119} // Matrix B row
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