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
#define C_cols 32

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
		{56, 101, -106, 17, -58, -7, 98, 111, 78, 31, -107, 12, -97, 28, 37, 21}, // Matrix A row
		{-93, 0, -51, -73, -15, -13, -77, 55, 125, -37, 121, 43, -107, 83, -14, 85}, // Matrix A row
		{71, -118, -75, -124, -63, -121, 18, -47, -106, 21, 81, -75, -106, 119, 74, -84}, // Matrix A row
		{-102, -11, 47, 38, -103, 107, 125, 19, -33, -47, -120, -29, 32, -106, 64, 43}, // Matrix A row
		{7, 125, 11, 51, -107, 101, -112, -89, 79, -40, -52, 115, 77, -42, 91, -60}, // Matrix A row
		{-21, 78, 112, -74, -82, -115, -64, -52, 85, -24, -91, 63, -59, -44, 79, 63}, // Matrix A row
		{-24, 74, 124, -17, -96, -50, 83, 13, 47, -128, -75, -9, 60, -84, 127, 67}, // Matrix A row
		{-34, -3, -2, 50, -55, -8, -66, 45, 108, 24, 25, -28, 11, -85, -29, 110}, // Matrix A row
		{90, 57, -98, -16, 58, 122, -48, -54, 14, 22, -19, -31, 58, 50, 99, -29}, // Matrix A row
		{123, -23, 62, 111, 105, 58, -88, -70, -102, -115, -91, 97, -8, -18, 93, 70}, // Matrix A row
		{-86, 55, -98, 80, 114, 124, -63, -54, -127, 96, -82, 111, 90, -101, -116, -20}, // Matrix A row
		{-92, -92, 17, 3, -94, -104, -48, -93, -70, -82, 12, 30, -101, 75, -18, 121}, // Matrix A row
		{-24, -8, -97, -85, -54, -74, 67, -3, 102, 23, -3, 24, -74, -123, 108, -32}, // Matrix A row
		{-12, -83, -80, 13, -90, -95, -41, -60, 3, 53, 16, 7, -49, 63, 26, 43}, // Matrix A row
		{27, 85, 46, 50, 111, -84, -69, 85, 116, -77, 109, 31, -61, 95, -37, -29}, // Matrix A row
		{36, 101, 28, -101, 22, -12, -23, -19, 103, 79, 93, -69, 6, 48, 77, -76} // Matrix A row
	}; // Matrix A row

  // Matrix B, elem_t, B_rows, B_cols Setup
	elem_t B[B_rows][B_cols] = { // Matrix B row
		{-115, -103, -127, 89, 123, -28, 60, 13, -56, 3, -49, -49, -40, 88, 40, 0}, // Matrix B row
		{19, 99, -46, 3, 59, 69, -83, -40, -123, 72, -107, -128, -121, -62, 52, 66}, // Matrix B row
		{-113, -23, -18, 0, 41, 104, 78, -64, 116, 29, -39, -36, -118, -93, 57, 92}, // Matrix B row
		{25, 24, -62, -68, 33, -125, 82, 49, 126, 10, -52, 104, -87, -13, -110, -46}, // Matrix B row
		{87, 11, -58, 79, 126, -9, -45, -10, -126, 107, -75, -40, 82, -7, 12, 36}, // Matrix B row
		{61, 68, 24, -86, -68, -3, 107, 20, -36, 109, 58, -20, 62, 99, 3, -32}, // Matrix B row
		{112, 52, 4, -37, 52, -5, -110, 21, 51, -100, 94, 61, -34, 53, 12, -65}, // Matrix B row
		{34, 125, 6, 20, 91, -20, -120, 48, -66, 69, -49, -33, -127, -41, 57, -44}, // Matrix B row
		{88, 31, 11, -81, 37, 20, -108, -69, -122, 113, -25, -10, -109, 42, -6, 30}, // Matrix B row
		{66, -77, 6, 9, 126, 73, -12, -6, 126, -47, 118, -108, -44, 113, -6, 73}, // Matrix B row
		{-29, -77, 119, -16, -120, 20, 51, -126, 4, 9, -90, -53, -84, -106, -128, -39}, // Matrix B row
		{-79, -110, -114, -92, -128, 11, 4, 91, 57, -32, 125, 112, -37, 83, -77, 65}, // Matrix B row
		{64, -30, 125, 10, 10, -52, 113, 87, -29, -4, -54, -19, 46, -89, 86, 37}, // Matrix B row
		{48, 25, -28, 74, 42, 8, -99, -100, 45, -90, 7, -64, 101, -24, 39, 77}, // Matrix B row
		{-42, -27, -51, 30, 95, 9, 1, -43, 103, -39, 91, 21, 10, -59, 71, -24}, // Matrix B row
		{4, -67, 3, -85, -118, -96, 3, -54, 29, 17, 42, 58, 50, 68, -120, -123} // Matrix B row
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