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
		{57, 20, 110, -24, 21, 102, 106, 22, 118, -92, 34, 58, -79, -6, 71, -81}, // Matrix A row
		{93, 113, -76, -12, -9, -112, -114, -67, 92, 83, -56, -122, -24, -84, -74, 53}, // Matrix A row
		{67, -96, 91, 60, -120, 48, 37, 15, 71, 33, -63, -28, 72, -114, -110, -37}, // Matrix A row
		{46, -83, 69, 49, -82, -33, -85, -2, -32, 124, 62, 34, -120, -81, 119, -73}, // Matrix A row
		{-55, 15, -115, 96, 116, 30, 96, -47, 12, 57, 30, 82, -70, -118, 116, 69}, // Matrix A row
		{79, 105, -71, 114, 76, 23, 52, -5, 15, -13, -32, 100, 83, 71, -54, -96}, // Matrix A row
		{-110, -16, -76, 77, 5, 17, -26, 79, 68, -97, 25, -63, 118, -80, 26, 66}, // Matrix A row
		{62, -10, -52, -64, 27, -14, 12, -86, -76, -22, -118, -41, 115, -31, -51, 75} // Matrix A row
	}; // Matrix A row

  // Matrix B, elem_t, B_rows, B_cols Setup
	elem_t B[B_rows][B_cols] = { // Matrix B row
		{-5, -40, 24, 12, 9, -81, 48, 126}, // Matrix B row
		{-110, -1, 54, 14, 0, -66, -5, 92}, // Matrix B row
		{-105, 112, -4, 48, -49, 47, -115, 62}, // Matrix B row
		{120, -84, -33, -61, -104, 68, -106, -59}, // Matrix B row
		{-106, -63, 64, 93, -11, -42, -116, 28}, // Matrix B row
		{-103, -124, -55, 88, -86, 37, -19, 119}, // Matrix B row
		{-5, -86, -104, 10, 39, -16, 3, -70}, // Matrix B row
		{-109, -29, 101, 18, 27, 70, 13, -90}, // Matrix B row
		{9, -73, 93, -26, 73, 86, -112, 34}, // Matrix B row
		{-18, -128, -12, -11, -117, 81, -63, 93}, // Matrix B row
		{28, -51, -50, 106, -88, -41, -64, 12}, // Matrix B row
		{90, 50, -30, -20, 91, -110, -31, 88}, // Matrix B row
		{13, -37, 54, 103, -81, -65, -32, 34}, // Matrix B row
		{-6, 108, 92, -12, 100, -2, 71, -102}, // Matrix B row
		{-29, -29, 20, -63, 125, -82, -120, 72}, // Matrix B row
		{112, 0, -77, -54, -27, -10, -115, -16} // Matrix B row
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