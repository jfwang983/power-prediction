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
		{-118, 109, 100, 102, 0, 34, -119, -29, 9, 29, 11, 112, 30, 59, -34, -48}, // Matrix A row
		{-77, 40, 101, -74, 36, 67, -115, -26, 116, -60, 123, -113, -74, -66, 123, -91}, // Matrix A row
		{33, -113, 110, -75, 125, 100, -21, 64, -67, 42, -68, 122, 51, -4, 90, 103}, // Matrix A row
		{-103, -7, -60, 122, -95, 40, 29, 15, -117, 88, 101, 117, -9, -120, -91, 3}, // Matrix A row
		{-20, -105, -14, -120, -27, -7, 33, 73, -36, -42, -99, 8, -18, 116, -72, -61}, // Matrix A row
		{48, 33, 117, 64, -52, 83, -3, -123, -75, -2, -114, 74, -8, -98, -110, 116}, // Matrix A row
		{55, 7, -79, 101, -12, 63, 124, -6, 117, -8, 42, 89, -13, 121, -121, 6}, // Matrix A row
		{-117, 45, -15, 13, -41, 104, -54, -87, -13, -5, 42, -112, 109, 66, 111, 78}, // Matrix A row
		{-108, -126, -8, 3, -94, -40, 95, 18, -40, -124, -72, -85, 89, -61, 40, -118}, // Matrix A row
		{63, 44, -56, 91, 109, 120, -60, 73, 50, -128, 123, -82, -74, -100, -72, 51}, // Matrix A row
		{26, 108, -112, -86, 28, -73, 46, 27, 113, -44, 124, -29, 12, 1, 75, 41}, // Matrix A row
		{-56, -101, 19, 105, -73, -14, -77, 3, -123, 5, 93, 54, -60, -10, 103, 124}, // Matrix A row
		{112, -65, -59, -43, -69, 113, -26, 66, 111, -8, -95, 25, -88, -105, 111, -86}, // Matrix A row
		{-103, -3, -18, 38, -1, 34, -45, 18, -56, 113, -100, 16, -28, 17, 36, 35}, // Matrix A row
		{-76, -86, -69, -72, -52, -101, 92, 82, -9, 95, -35, 17, -84, -76, 76, -36}, // Matrix A row
		{82, -73, -60, 54, -8, -77, 38, 115, -1, 35, -65, 111, -100, -121, -13, -75} // Matrix A row
	}; // Matrix A row

  // Matrix B, elem_t, B_rows, B_cols Setup
	elem_t B[B_rows][B_cols] = { // Matrix B row
		{-79, 2, 109, 20, -45, 22, 28, 51}, // Matrix B row
		{123, -67, -94, -91, -80, -59, -12, 54}, // Matrix B row
		{15, 21, 94, 45, -126, 71, -59, -99}, // Matrix B row
		{1, -29, -45, -36, -38, 109, -53, 124}, // Matrix B row
		{16, -116, 81, -118, 39, 28, 91, -113}, // Matrix B row
		{-83, -11, -36, 39, 13, 72, -21, 114}, // Matrix B row
		{21, 36, 109, 85, -109, 46, 65, 103}, // Matrix B row
		{116, 19, 21, -13, 79, 98, 74, -19}, // Matrix B row
		{1, 96, 51, -87, 101, 16, -23, -125}, // Matrix B row
		{78, -81, -95, 95, 8, 28, -105, 60}, // Matrix B row
		{-47, -118, -104, 122, -7, 0, -40, 76}, // Matrix B row
		{-43, -7, 124, -122, -69, -10, 124, 2}, // Matrix B row
		{76, -31, -84, -87, 64, -112, -73, 28}, // Matrix B row
		{69, 69, 25, 114, 55, 87, 93, -103}, // Matrix B row
		{97, -42, -43, 58, -115, 87, -60, 99}, // Matrix B row
		{-119, -55, 35, 103, -8, 91, -58, 39} // Matrix B row
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