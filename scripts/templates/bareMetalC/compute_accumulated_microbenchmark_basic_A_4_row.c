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
#define A_rows 4
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
		{-30, -29, -50, 94, 119, -3, 75, -45, 17, 82, 14, 58, -30, -107, -99, 43}, // Matrix A row
		{103, 105, 33, -63, 3, -92, -22, -101, 19, -122, 126, 19, -84, -11, 75, -97}, // Matrix A row
		{96, 102, -56, -109, -101, 9, 4, 18, -72, 24, -66, 10, 79, -49, -28, 87}, // Matrix A row
		{-114, 124, -50, 26, -21, 123, 29, 28, -92, -125, -109, -3, 8, -8, 42, -48} // Matrix A row
	}; // Matrix A row

  // Matrix B, elem_t, B_rows, B_cols Setup
	elem_t B[B_rows][B_cols] = { // Matrix B row
		{-92, 28, 8, -74, 43, -59, 25, 127, -38, 117, 111, -1, -80, -33, -79, 50}, // Matrix B row
		{-18, -89, -117, -23, 40, -24, -80, 90, 54, 40, 61, 107, -111, 69, -42, 119}, // Matrix B row
		{80, 10, -8, -10, -120, 112, 53, -8, 79, 55, 92, -64, 72, 123, -101, -33}, // Matrix B row
		{111, -12, -36, 79, 103, 58, 29, -43, 73, -29, -89, 65, -97, -52, -48, -35}, // Matrix B row
		{109, 44, 26, 115, -65, -112, -18, -93, 56, -68, -10, 51, 2, -14, -105, -117}, // Matrix B row
		{97, 81, 66, 126, 118, 81, 94, 52, 42, 115, -116, -110, -97, -94, 17, -13}, // Matrix B row
		{-115, -47, 4, -123, 98, 75, -70, -99, -100, -10, 81, 111, 57, -61, 53, -71}, // Matrix B row
		{18, 127, -28, -22, -98, 34, -118, -104, -19, 56, 89, -110, 115, -85, 54, -104}, // Matrix B row
		{66, 2, 0, -40, 18, -95, -26, -98, -13, -67, -44, 101, 45, -8, -81, -127}, // Matrix B row
		{-25, -116, -42, -3, 41, -106, 11, 51, 9, 53, 58, -110, -7, -78, -59, 59}, // Matrix B row
		{-108, 44, -73, 99, 105, -28, 125, 2, -88, -67, -113, 49, 19, 71, -14, 1}, // Matrix B row
		{110, -24, -54, -92, 84, 36, -11, -114, 62, -90, -113, 111, 111, -100, 37, 3}, // Matrix B row
		{-12, -30, 126, 82, -16, -21, -5, -48, 69, -111, 18, 2, 73, 75, 117, -102}, // Matrix B row
		{-33, 24, -83, 22, -124, 1, 56, 46, 43, -42, -26, 100, -111, 125, -63, -62}, // Matrix B row
		{30, 8, -42, -69, -78, -100, -61, 94, -72, -25, -40, 108, 16, 89, -124, -16}, // Matrix B row
		{-114, -127, -31, -93, -62, 115, -121, -37, -105, -1, 66, -4, -59, 84, -69, 43} // Matrix B row
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