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
		{-37, -8, -113, -105, -33, -25, -122, -43, -108, -66, 32, -104, 99, -13, 18, -104}, // Matrix A row
		{-21, -66, -102, -28, 54, 60, -92, 87, 16, 76, 107, -69, -43, 99, -103, -81}, // Matrix A row
		{57, 110, -124, -59, -23, -61, -69, 97, -19, 25, -51, 77, -114, 53, -51, -66}, // Matrix A row
		{76, -39, 127, -56, -36, 127, -21, -2, 20, 69, 114, -16, 9, -12, 118, 73}, // Matrix A row
		{93, -56, -29, -56, -35, -101, -47, -117, -76, -126, 60, 80, -67, -93, 11, 82}, // Matrix A row
		{13, 43, 68, -123, 64, 27, 25, 68, 50, 4, -117, 61, -48, -1, 77, 125}, // Matrix A row
		{22, 120, -58, 39, 2, -30, 47, -46, -44, 26, -35, 71, -42, -86, -68, 7}, // Matrix A row
		{57, 63, -110, 52, 69, 98, 89, 2, 84, 56, -77, -27, 110, 37, 48, -47} // Matrix A row
	}; // Matrix A row

  // Matrix B, elem_t, B_rows, B_cols Setup
	elem_t B[B_rows][B_cols] = { // Matrix B row
		{19, 44, 73, 99, -112, 109, 30, -65}, // Matrix B row
		{-116, -31, 43, -44, 118, 26, -52, -15}, // Matrix B row
		{36, 120, 13, 30, -122, -104, 59, -118}, // Matrix B row
		{-107, -72, 119, 14, 63, 7, -80, 114}, // Matrix B row
		{-121, -112, -10, -55, -28, 102, -58, 81}, // Matrix B row
		{17, -88, 110, -118, -97, -82, 97, 53}, // Matrix B row
		{-126, -105, 39, -94, 70, 27, -95, 49}, // Matrix B row
		{-73, 48, -3, 111, -122, -89, 74, 47}, // Matrix B row
		{120, 28, -59, 74, 92, 29, -121, -1}, // Matrix B row
		{50, 102, -111, -57, -19, 34, 40, 67}, // Matrix B row
		{34, 24, 126, 42, -74, 5, -22, -17}, // Matrix B row
		{-107, 71, -70, -5, -59, 79, -26, 113}, // Matrix B row
		{120, -80, 125, -8, -61, -90, 34, -1}, // Matrix B row
		{-57, -62, -81, 61, 47, 100, -49, -69}, // Matrix B row
		{-25, -15, 41, 39, -68, -2, 55, 39}, // Matrix B row
		{10, 65, -19, -115, -50, 16, -33, -59} // Matrix B row
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