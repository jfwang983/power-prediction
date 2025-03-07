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
		{-59, 84, 3, 25, -84, 58, 92, -105, -37, -97, 93, 42, -59, 45, 45, 44}, // Matrix A row
		{-111, -72, 101, -37, 10, 72, 64, 104, -86, -24, -81, 96, -86, 81, -107, -86}, // Matrix A row
		{2, 116, 79, 111, 3, -71, 91, -61, 76, -5, 11, -32, -10, -83, -18, -93}, // Matrix A row
		{-106, 113, 31, 124, -110, 67, -79, 108, -11, -94, -26, -48, -90, -11, 87, 32}, // Matrix A row
		{-39, -25, 41, -68, 23, 42, -47, -54, 67, 98, 32, -59, -32, 98, -6, 41}, // Matrix A row
		{126, 7, 99, -12, -39, 66, -114, 114, -105, 86, 20, 37, -56, -61, -49, 17}, // Matrix A row
		{-1, 26, 79, 37, -11, -2, 23, -93, 85, -123, -41, 29, -23, 10, -128, -116}, // Matrix A row
		{126, -119, 78, -109, 113, -101, -68, -50, 103, 87, -20, 42, -100, -70, -84, 96}, // Matrix A row
		{-44, 110, 125, -60, -15, 6, -105, -5, -107, 61, 1, 5, -82, -127, 18, 126}, // Matrix A row
		{-95, -2, 49, 63, 43, 107, -23, 42, 77, 119, 78, 25, -117, 32, 11, 65}, // Matrix A row
		{41, 56, -85, 18, -88, -115, -84, -115, 68, -110, -78, 36, 40, -61, 71, -117}, // Matrix A row
		{49, 19, -95, 18, -17, -30, 103, -27, 88, -25, -126, 34, -93, 72, 31, 30}, // Matrix A row
		{105, -63, -119, 116, 28, -73, 102, -87, 13, 51, -26, -45, 102, -116, -94, -16}, // Matrix A row
		{-94, -60, 2, 116, -70, 19, 125, -27, 58, -118, -101, 91, 32, -128, -106, 44}, // Matrix A row
		{-122, 87, -60, -59, 38, 103, 33, -106, 105, -24, 72, -127, -15, 30, -30, 89}, // Matrix A row
		{-60, 84, -27, -100, -84, -24, 58, -8, 101, 59, 76, 15, 51, 83, 8, 58} // Matrix A row
	}; // Matrix A row

  // Matrix B, elem_t, B_rows, B_cols Setup
	elem_t B[B_rows][B_cols] = { // Matrix B row
		{-48, 12, 18, -61, -84, -94, 124, -84, 52, -1, 114, 13, 94, 99, -15, -32}, // Matrix B row
		{60, 82, 21, -114, -122, -54, -52, 118, -2, 36, 77, 30, -14, -19, -114, -17}, // Matrix B row
		{-90, 17, 91, -122, 99, 79, 32, 76, 3, 1, 55, -49, -25, 127, 101, -63}, // Matrix B row
		{77, 119, -67, -124, -70, -110, -85, -18, 48, 12, -120, 92, 46, -115, 81, 46}, // Matrix B row
		{-99, -80, -27, 41, -27, 79, -93, -69, 47, -93, -116, -104, 44, -66, 57, -18}, // Matrix B row
		{126, -127, -18, 68, -49, 96, 59, 22, 33, -46, 116, -70, 42, 68, -35, -106}, // Matrix B row
		{-116, -42, 62, 102, -36, 23, 17, 25, 29, 31, -79, 57, -41, 6, 46, -93}, // Matrix B row
		{-108, 66, -107, 121, 54, 84, -63, 67, -47, 115, 80, -57, -89, -109, -28, 41}, // Matrix B row
		{-44, 77, -56, -108, -67, -59, 89, 106, -18, -24, 34, -106, -94, -81, 122, -114}, // Matrix B row
		{-53, -12, 88, -20, 60, -34, -43, -7, 109, 54, -42, 123, 10, 20, 58, 75}, // Matrix B row
		{-86, 93, -96, 5, 125, -90, -94, 76, -82, -31, 4, 2, 90, -28, -30, -126}, // Matrix B row
		{-72, 42, 43, -39, -73, 50, 46, -96, 102, 53, 3, 62, 9, -26, -95, 98}, // Matrix B row
		{-47, -125, -25, 120, -15, 101, -61, 84, 58, -66, 10, -13, 69, -45, 54, 120}, // Matrix B row
		{-10, 119, 108, -115, -41, 119, -19, -77, 82, 38, 59, 86, -69, -32, -96, 97}, // Matrix B row
		{-116, -58, -76, -48, -91, 114, 104, 69, -55, 27, 71, 21, 117, -29, -10, -56}, // Matrix B row
		{-115, 108, -84, -100, 69, -49, -64, -94, 9, 0, -107, -28, -98, 98, -66, -52} // Matrix B row
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