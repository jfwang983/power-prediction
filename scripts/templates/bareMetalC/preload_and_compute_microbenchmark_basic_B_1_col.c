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
#define B_cols 1
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
		{119, 70, -96, 85, -26, -11, 37, -62, 76, -24, -20, -67, -95, 21, -52, 81}, // Matrix A row
		{53, 15, 65, 111, -91, 86, 3, -116, 17, -89, 57, -30, -24, -96, -96, -16}, // Matrix A row
		{-82, -43, 72, 49, 118, 65, -103, -5, 124, -64, -87, -117, 76, -108, -121, -123}, // Matrix A row
		{101, 120, 66, 81, -81, -100, -73, -37, -10, -53, -39, 19, 97, -106, -60, 49}, // Matrix A row
		{-55, -43, 9, 38, 89, -4, -105, -78, -110, -3, 18, 46, 54, -121, -6, -19}, // Matrix A row
		{-53, -88, 17, 62, 49, -105, -19, 95, 104, -20, 49, 100, 118, 25, 33, -13}, // Matrix A row
		{-118, -125, -53, -18, 19, 86, -14, -116, 96, 4, -17, 63, -68, 63, -21, 52}, // Matrix A row
		{126, 13, 62, 103, -81, 34, -24, 42, 100, -94, 115, -106, -67, -15, 55, 27}, // Matrix A row
		{29, 24, 31, 44, 19, 74, -58, -105, -111, 70, -89, -11, -64, 67, 125, -122}, // Matrix A row
		{-116, 85, -83, 15, 83, -75, -7, -60, 11, -53, -48, 107, -33, 121, 41, -30}, // Matrix A row
		{37, 54, -38, 40, -88, 0, -43, -67, -52, -80, -48, 111, -98, 112, 27, 78}, // Matrix A row
		{-119, -42, -127, 95, 121, -116, 80, 33, -48, 50, 112, -98, -42, 85, -72, 27}, // Matrix A row
		{-92, 13, -1, -72, -15, 52, 17, -65, -113, 0, -19, -14, 0, -86, 0, 15}, // Matrix A row
		{-14, 112, -87, 35, -35, -18, 59, -87, -119, 62, -84, 57, -33, 18, -85, -18}, // Matrix A row
		{-37, -27, 48, -116, -39, 11, -60, 32, -58, -79, -71, -62, -10, 59, 117, -65}, // Matrix A row
		{-109, -9, 33, -21, -119, 64, 112, 107, 33, 67, 52, -85, -48, 114, -23, 32} // Matrix A row
	}; // Matrix A row

  // Matrix B, elem_t, B_rows, B_cols Setup
	elem_t B[B_rows][B_cols] = { // Matrix B row
		{33}, // Matrix B row
		{-9}, // Matrix B row
		{66}, // Matrix B row
		{43}, // Matrix B row
		{124}, // Matrix B row
		{114}, // Matrix B row
		{-28}, // Matrix B row
		{-59}, // Matrix B row
		{-51}, // Matrix B row
		{51}, // Matrix B row
		{-32}, // Matrix B row
		{21}, // Matrix B row
		{-4}, // Matrix B row
		{72}, // Matrix B row
		{-8}, // Matrix B row
		{49} // Matrix B row
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