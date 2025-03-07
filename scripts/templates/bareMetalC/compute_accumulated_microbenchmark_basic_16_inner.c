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
		{-80, -34, 21, 39, 94, 90, -108, 55, 110, 113, 113, 113, 45, 39, 82, 9}, // Matrix A row
		{-99, 80, -66, 90, -13, -5, 79, 36, 43, -27, 9, -39, -79, -23, -36, -64}, // Matrix A row
		{21, 7, -91, -61, -103, -85, -92, -77, -20, 27, 39, 48, 115, 54, 119, 10}, // Matrix A row
		{38, -56, 99, 2, 1, -25, 35, 40, -54, -11, 46, 86, -11, 33, -50, 23}, // Matrix A row
		{6, -7, -61, 57, 77, 89, 5, 104, -57, 0, -44, 102, -97, 48, -64, -41}, // Matrix A row
		{-63, 102, -58, -34, 46, -33, 48, -116, -103, -97, 83, -128, 125, 44, 55, 64}, // Matrix A row
		{-35, -102, -37, -123, -27, -96, 48, 63, 107, -51, -16, -76, -69, 20, -126, -71}, // Matrix A row
		{-36, -123, 111, 61, 116, 40, 35, -124, 52, -10, -20, 26, 106, -126, -45, 52}, // Matrix A row
		{52, -25, -122, 15, 110, -100, 30, 74, 62, 109, -70, -19, -5, 9, 121, 84}, // Matrix A row
		{83, 73, -80, -55, -72, 79, -20, 20, -54, 22, -76, 65, -103, 52, -39, -54}, // Matrix A row
		{69, 121, 2, 25, 20, 46, 103, -47, -92, 64, -16, -79, -7, 2, 86, -97}, // Matrix A row
		{-127, 100, -58, -27, 2, -99, -10, 5, -81, 100, -72, -74, 28, -19, -43, 51}, // Matrix A row
		{-114, 37, -29, -43, 40, -76, 77, -42, -21, -33, 109, 47, 60, 114, 2, 45}, // Matrix A row
		{-69, -64, -7, 125, 54, 58, 106, -45, 46, 127, 118, 125, -119, 9, -39, -103}, // Matrix A row
		{40, 38, -44, 101, 83, -21, -80, -57, 6, 61, 32, 71, 21, 96, -27, -28}, // Matrix A row
		{54, 107, -109, 113, 84, -109, 100, -88, -6, 69, -43, -79, -51, -116, 39, 47} // Matrix A row
	}; // Matrix A row

  // Matrix B, elem_t, B_rows, B_cols Setup
	elem_t B[B_rows][B_cols] = { // Matrix B row
		{110, 108, 73, -99, -29, 87, -11, -126, 17, 3, -11, -48, -66, -85, 73, -78}, // Matrix B row
		{-110, -107, 80, 96, 64, -47, -2, 86, -45, 35, -102, -11, 15, -95, 16, 2}, // Matrix B row
		{28, -119, -107, 101, 123, 110, -103, 67, -42, -124, -49, 53, 69, 125, -13, 19}, // Matrix B row
		{-83, 50, -111, -3, 95, 28, -108, -84, 84, 41, 61, 114, -3, 105, 90, 64}, // Matrix B row
		{-64, -70, -39, -100, -39, -45, -11, -91, 42, 61, -70, -49, -89, -9, 57, -29}, // Matrix B row
		{14, -48, 35, -85, -96, 27, 27, -79, -64, 102, -71, 27, -48, 121, -38, 79}, // Matrix B row
		{123, -70, 46, -64, -8, 114, -73, 44, 2, -113, -110, -118, 2, -107, -32, -35}, // Matrix B row
		{2, -58, 122, 34, -30, -80, -114, -106, -55, 36, 105, -107, -112, 39, -34, 116}, // Matrix B row
		{6, 73, 12, -117, 63, -118, -90, 60, -79, -120, -11, 45, -20, 117, -78, 63}, // Matrix B row
		{-36, 18, 117, 71, 64, 102, -19, 7, -110, -83, 91, 1, -28, -61, 108, -94}, // Matrix B row
		{-61, -45, 112, 24, -99, 63, 50, -72, 35, -33, -40, 42, -121, -83, -100, 104}, // Matrix B row
		{-68, -68, 34, -126, 64, 72, -14, -89, 53, -32, 56, -10, 118, 39, 17, 19}, // Matrix B row
		{64, 72, 18, -70, 126, 114, 121, 117, -45, -105, -60, 10, 19, -109, -55, 66}, // Matrix B row
		{122, 39, 62, 47, 38, -83, -123, 54, 122, -46, 81, 124, 44, -95, 30, 108}, // Matrix B row
		{0, -92, -13, -46, -19, -16, 19, -89, 110, 9, -116, 58, -86, -24, -123, 76}, // Matrix B row
		{-123, -62, 3, -126, -93, -60, -124, 98, 76, -68, 60, -60, 17, 13, 21, -98} // Matrix B row
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