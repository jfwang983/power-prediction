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
		{75, 105, 73, 69, 94, -119, 31, -19, -37, -98, 117, -4, 119, -1, 58, 48}, // Matrix A row
		{84, -117, 35, 73, 35, 45, -8, 11, 118, -29, 23, -118, -93, -103, -24, -38}, // Matrix A row
		{-128, -54, 57, -58, -65, 77, 45, 13, 78, -99, -63, 16, -15, 118, 83, 123}, // Matrix A row
		{-73, -115, -14, -45, -22, -16, -88, 96, -74, 5, 125, -10, -54, -17, -82, 116}, // Matrix A row
		{41, -75, -63, -17, 113, -33, -40, 93, 4, 73, -56, 10, -4, 124, -6, -114}, // Matrix A row
		{-64, -66, 87, -125, 104, 98, 38, -91, -75, 101, 114, -110, -72, 68, 40, 66}, // Matrix A row
		{-55, -30, -88, 52, -88, 77, -49, -119, 99, -71, -36, -12, 32, -108, -33, -112}, // Matrix A row
		{90, 5, 21, -127, 60, 68, 16, 69, -12, -13, -57, 91, 81, 53, 30, 53}, // Matrix A row
		{-55, 74, 8, -46, 21, -90, 58, -69, -63, -82, 106, 126, -104, 59, 116, 120}, // Matrix A row
		{0, 63, -96, -9, -53, -117, -43, 125, 66, 67, -25, -53, 99, 104, -105, -31}, // Matrix A row
		{-72, -106, 49, 70, 72, -2, -49, 65, -77, 71, 64, 119, -36, -16, 25, 47}, // Matrix A row
		{-88, 104, 55, -12, 7, -71, -108, 9, 78, 113, -96, 76, -114, -8, -15, 63}, // Matrix A row
		{31, -12, -62, 51, -49, -36, -97, -114, -91, 120, 83, 122, -106, -35, 96, -16}, // Matrix A row
		{86, 54, -40, -99, -74, -84, 66, -57, -44, 68, -128, 105, 62, 4, 9, -64}, // Matrix A row
		{111, 93, 2, 116, -84, 52, -12, -52, -79, 57, 3, 19, 23, -47, 126, 97}, // Matrix A row
		{-127, 87, 83, 89, -54, 102, -58, -120, -81, 17, -20, -123, -120, -76, -102, -52} // Matrix A row
	}; // Matrix A row

  // Matrix B, elem_t, B_rows, B_cols Setup
	elem_t B[B_rows][B_cols] = { // Matrix B row
		{-20}, // Matrix B row
		{-10}, // Matrix B row
		{35}, // Matrix B row
		{86}, // Matrix B row
		{100}, // Matrix B row
		{83}, // Matrix B row
		{126}, // Matrix B row
		{19}, // Matrix B row
		{-35}, // Matrix B row
		{95}, // Matrix B row
		{0}, // Matrix B row
		{-65}, // Matrix B row
		{-33}, // Matrix B row
		{-18}, // Matrix B row
		{-101}, // Matrix B row
		{-79} // Matrix B row
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