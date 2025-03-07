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
		{122, -115, -16, 61, 87, 12, -96, 66, -97, 121, 117, -26, 80, 2, -45, 19}, // Matrix A row
		{-120, 104, -7, 30, -24, 113, 37, -65, -93, -72, -32, 111, 17, -20, 11, -43}, // Matrix A row
		{-80, 16, -30, 92, 97, 115, -6, -95, 65, 42, -105, -80, 56, -105, -93, 96}, // Matrix A row
		{-104, -99, -5, -96, -85, -54, -28, -31, 7, 75, 118, 2, -100, -77, -79, -71}, // Matrix A row
		{-57, 13, 92, -107, -5, 121, -84, -116, -75, 91, -96, 60, 79, 57, -97, 53}, // Matrix A row
		{120, -102, -125, 122, 55, -59, -85, -97, 16, -105, 28, 20, 18, -24, 35, 116}, // Matrix A row
		{25, 68, -121, -32, 127, -116, 83, -13, -112, 68, 103, 64, -41, 63, 36, 114}, // Matrix A row
		{-127, -37, 27, 4, -73, 50, -24, 91, -44, 55, -74, 39, -63, -110, 110, 9}, // Matrix A row
		{36, 0, -21, -97, -9, 25, -23, 35, 35, -15, -75, 22, -53, 101, -95, 106}, // Matrix A row
		{-128, 87, 110, -11, -39, 69, 16, 27, 3, 45, 34, -83, -66, -2, 102, 19}, // Matrix A row
		{-63, -123, -53, 53, -96, -37, 67, 7, -110, -85, -103, -88, -74, -26, 14, 127}, // Matrix A row
		{-119, 57, -98, -126, 40, -34, 56, 126, 32, -95, 111, -18, 7, 81, -85, -24}, // Matrix A row
		{81, -66, 12, 124, 86, -67, 78, -93, 91, -123, -3, -91, -105, -42, -63, 118}, // Matrix A row
		{89, -92, 124, -23, 87, -1, -109, 118, 127, -6, -94, -57, 89, -48, 47, 107}, // Matrix A row
		{-83, 116, -61, 10, 24, -15, -28, 118, -112, 100, -101, 28, 112, 89, -19, -123}, // Matrix A row
		{-126, 125, 59, 104, 64, -46, -62, -31, 127, 102, 7, -12, -21, 82, -103, 8} // Matrix A row
	}; // Matrix A row

  // Matrix B, elem_t, B_rows, B_cols Setup
	elem_t B[B_rows][B_cols] = { // Matrix B row
		{-122, 97, 66, -99, 6, -16, -119, 59, 119, 63, 123, 60, 29, 65, 86, -127}, // Matrix B row
		{-30, 116, 93, -41, 38, -116, -89, 16, 31, 42, -71, 46, 58, -104, 64, 38}, // Matrix B row
		{-77, -80, -118, 74, -35, 87, -56, -25, 111, -21, -31, -121, 59, 20, -127, 22}, // Matrix B row
		{13, -50, -120, 59, -77, 70, 125, -23, 123, -63, -13, -42, -98, -33, 82, -72}, // Matrix B row
		{109, -76, -41, 30, 26, -36, 75, 107, 115, 118, -115, -102, -96, 16, -76, -112}, // Matrix B row
		{73, -62, -25, 41, 82, -60, -119, 99, 1, -117, -60, 99, 54, -40, 78, -80}, // Matrix B row
		{-23, -6, -55, -39, -92, -118, 37, -49, 84, 22, -7, 21, -68, -14, 105, 76}, // Matrix B row
		{113, -79, -39, 51, -109, -36, -86, -98, -87, -49, 76, -70, -23, 114, -42, 85}, // Matrix B row
		{91, -98, 94, 97, 63, -99, -63, 33, 31, -77, -66, -57, 8, -81, 70, 105}, // Matrix B row
		{-34, -119, -52, 102, 55, 20, -121, -120, 86, 53, 100, 63, -123, -15, 57, 114}, // Matrix B row
		{121, 126, -117, -28, 20, -126, 62, 30, -34, 32, 82, -36, 126, -123, -59, 122}, // Matrix B row
		{-73, 116, -120, -4, -102, 123, -93, 32, -79, 91, -84, -76, -6, -3, -78, -110}, // Matrix B row
		{40, -126, 123, -109, 126, 88, -106, -63, -90, 126, -7, 33, 37, 51, -15, -91}, // Matrix B row
		{23, 120, -66, -50, 22, 11, -18, 97, -121, 45, -99, 51, 43, 65, 0, 57}, // Matrix B row
		{-109, 37, -17, -107, -18, 121, 123, -44, 106, -53, -128, -1, -30, 43, 32, 19}, // Matrix B row
		{-12, -47, -85, 21, -52, -11, 52, -55, 19, 37, -78, -128, -63, -128, 10, -7} // Matrix B row
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