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
		{-2, -127, 10, 38, 101, -86, 92, 23, 13, -40, -93, -115, 72, -109, 6, 88}, // Matrix A row
		{0, 45, -27, -79, 8, 32, -71, 35, 71, -64, -43, -30, -108, -89, 21, 85}, // Matrix A row
		{-118, 100, 112, -81, -102, -39, -36, -5, -44, -42, -50, 47, 74, -58, -1, -72}, // Matrix A row
		{105, 6, -37, -128, -60, 3, 14, -108, 96, -125, 8, 114, -16, 121, -10, -58}, // Matrix A row
		{-104, 114, -88, 85, 54, -65, 14, -68, 88, -94, -14, 73, -55, 62, 12, 104}, // Matrix A row
		{122, -109, -72, 88, 54, -24, 110, -15, 50, -6, 35, 24, 96, 82, -77, -61}, // Matrix A row
		{9, 13, 21, 43, -99, 38, -12, -56, 84, -2, -124, -73, -29, -43, 50, -6}, // Matrix A row
		{-34, 15, -65, 12, -39, -95, -110, 73, -121, 117, 30, -83, 119, 9, 3, -115}, // Matrix A row
		{22, -3, 57, -3, 127, -12, -99, 102, -14, 107, -122, 113, 99, -126, -19, -17}, // Matrix A row
		{-126, 115, 124, 26, -70, -75, -120, -11, -7, -35, 38, -58, 40, -62, 91, -93}, // Matrix A row
		{46, -39, -26, 59, 26, 15, -57, -51, 23, 12, 104, -48, -77, -68, -59, 50}, // Matrix A row
		{-17, 25, -100, -96, -24, 4, -128, -54, 10, 74, -84, 112, 86, -17, -34, 60}, // Matrix A row
		{-19, 122, -102, -91, 120, 116, 38, -8, 45, -58, 90, -22, -118, -110, -36, 103}, // Matrix A row
		{57, -115, -53, 103, -73, -42, -36, -117, -49, -59, 121, 81, 108, -107, -12, -25}, // Matrix A row
		{-86, 67, 61, 117, 65, 90, 43, -36, 2, 78, -68, -81, 115, -13, -4, 114}, // Matrix A row
		{77, 97, 108, 21, -49, 56, 54, -47, -115, -42, 70, -31, 78, -100, -40, 8} // Matrix A row
	}; // Matrix A row

  // Matrix B, elem_t, B_rows, B_cols Setup
	elem_t B[B_rows][B_cols] = { // Matrix B row
		{-12, 33, -70, 68, -59, -65, -20, 23, 78, 85, 78, 41, 2, 41, -55, 29}, // Matrix B row
		{-70, 79, 52, -51, -9, 44, 117, 66, 78, 67, 50, 126, 64, 23, 48, 11}, // Matrix B row
		{-109, 46, -31, -86, 62, 90, 5, -77, -116, -89, 82, 123, -67, 103, 78, 22}, // Matrix B row
		{46, -108, 23, 121, 0, -38, 94, 99, 49, 91, 9, 126, -122, 89, 72, -88}, // Matrix B row
		{-14, 10, 102, -47, 95, -72, 51, -71, -51, 111, 109, 91, -120, 17, 109, 19}, // Matrix B row
		{25, -82, 110, 74, -43, -87, 50, -15, -48, 60, -92, -42, 46, 32, -52, 91}, // Matrix B row
		{-66, -40, -73, -64, -119, 98, -24, -42, -45, -128, -97, 103, 51, -69, 46, -67}, // Matrix B row
		{-54, -96, -84, 38, 60, -56, 87, -51, 28, -37, -42, -93, 7, -126, 4, 105}, // Matrix B row
		{-72, 70, -81, 30, -64, -74, 91, -110, -62, -30, 88, -89, -25, 54, 62, -19}, // Matrix B row
		{-32, -106, -117, -60, 7, -33, -88, 40, 115, 99, 97, -95, -64, 75, 94, 60}, // Matrix B row
		{-10, 75, 115, 57, -114, -28, -43, 97, 37, -8, 107, -69, -87, -93, -43, 124}, // Matrix B row
		{17, -127, -72, 70, 97, 110, 47, 91, 8, 41, -16, -2, -26, -11, -60, -88}, // Matrix B row
		{-87, -122, 39, -44, -89, 55, -39, -16, 127, 94, -34, 17, 91, -23, 120, -73}, // Matrix B row
		{-30, -57, 18, -22, 62, -36, 73, 47, -35, -6, -36, 52, -9, 72, 64, -8}, // Matrix B row
		{-12, -87, 28, 112, -34, 30, 93, 98, 23, -68, -37, -35, -114, -85, 72, -75}, // Matrix B row
		{-80, -114, 102, 98, 88, 105, -115, -120, 93, -48, -102, 46, -23, 116, -64, -108} // Matrix B row
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

  gemmini_extended_preload(B_sp_addr, C_sp_addr, B_cols, B_rows, C_cols, C_rows);
  gemmini_extended_compute_preloaded(A_sp_addr, GARBAGE_ADDR, A_cols, A_rows, B_cols, B_rows);
	// printf("gemmini_extended_preload(%p, 0x%x, %u, %u, %u, %u);\n", B_sp_addr, C_sp_addr, B_cols, B_rows, C_cols, C_rows);
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