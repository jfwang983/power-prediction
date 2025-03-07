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
#define B_cols 4
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
		{77, -116, 34, -78, 35, 14, -78, -122, 84, -24, 89, 9, 87, -113, 74, 9}, // Matrix A row
		{-64, 92, 52, 6, -79, -123, 126, -77, 20, 115, 83, -18, -84, -114, -39, 19}, // Matrix A row
		{62, 114, -38, -28, -46, 7, 116, -105, 20, 110, -46, -85, 14, 78, 45, 56}, // Matrix A row
		{45, -33, 33, 84, 23, 125, 29, -13, -31, 20, 97, 50, -91, -51, -30, 81}, // Matrix A row
		{-128, -127, -34, -127, 31, 119, 126, 82, -60, -113, -32, 99, -84, 112, 66, 45}, // Matrix A row
		{22, 66, 50, -125, -17, -3, 30, 49, 99, 72, 120, 83, -12, 105, -24, 18}, // Matrix A row
		{-94, -116, -86, 73, 5, -93, -88, 7, 57, -59, 126, -100, 69, 43, -62, 112}, // Matrix A row
		{-60, 76, -75, -34, -95, -23, -100, 60, -34, -57, -37, -10, -107, 40, -12, 121}, // Matrix A row
		{-17, -22, -65, 64, -55, 106, 62, -23, 4, -105, 106, 49, -56, -49, 75, -44}, // Matrix A row
		{-47, -86, 99, 14, 44, 88, -115, -92, 34, 116, 10, -23, -32, -113, 79, -9}, // Matrix A row
		{-30, -41, -23, -32, -79, -118, -102, -20, 26, 13, -114, -2, 81, -38, -122, 63}, // Matrix A row
		{-22, 23, 119, -5, -108, -100, 80, -55, 122, 11, 17, -32, 4, -58, 88, 56}, // Matrix A row
		{-108, -76, -10, 64, 37, 58, -84, 53, 19, -25, 5, -115, 86, 28, 76, -67}, // Matrix A row
		{-76, -39, -120, 76, -12, -32, 127, 29, 64, 76, 112, -49, 71, 9, -13, -49}, // Matrix A row
		{-67, 34, -44, 83, 98, -17, -81, 41, -64, 120, -49, -124, -39, -119, -52, 98}, // Matrix A row
		{-75, -41, 10, 40, -89, -115, -67, -99, -122, 119, -46, -93, 86, -116, -117, -113} // Matrix A row
	}; // Matrix A row

  // Matrix B, elem_t, B_rows, B_cols Setup
	elem_t B[B_rows][B_cols] = { // Matrix B row
		{78, 100, -121, 122}, // Matrix B row
		{61, 52, -110, 106}, // Matrix B row
		{96, 126, -97, -47}, // Matrix B row
		{86, -94, -82, 56}, // Matrix B row
		{74, 118, 57, 69}, // Matrix B row
		{-124, 15, 24, -19}, // Matrix B row
		{-12, 56, 110, -15}, // Matrix B row
		{28, 79, -9, -76}, // Matrix B row
		{107, -38, 12, -73}, // Matrix B row
		{-114, 55, -1, -95}, // Matrix B row
		{-94, 56, -26, 77}, // Matrix B row
		{115, -96, -113, -41}, // Matrix B row
		{78, 103, 23, 99}, // Matrix B row
		{-38, 124, 5, 32}, // Matrix B row
		{-73, -48, -14, -114}, // Matrix B row
		{-39, 53, 100, 66} // Matrix B row
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