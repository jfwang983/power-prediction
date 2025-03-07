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
#define A_rows 1
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
		{-53, 80, -69, 29, -24, 18, 30, -65, -57, 100, 28, -10, 29, -4, -2, 102} // Matrix A row
	}; // Matrix A row

  // Matrix B, elem_t, B_rows, B_cols Setup
	elem_t B[B_rows][B_cols] = { // Matrix B row
		{-92, -2, -26, -2, -18, 22, 89, -122, 96, 63, 13, 34, 112, 97, -32, -49}, // Matrix B row
		{-128, 33, 29, 40, 122, 114, -82, -123, 110, 23, -1, -88, 31, 96, 41, -60}, // Matrix B row
		{24, -1, -63, 46, 99, -112, 74, -109, -115, 3, -120, 20, -81, -46, -54, 83}, // Matrix B row
		{-50, 113, 58, 72, 46, -40, -39, 49, -2, -14, -45, -78, 1, 92, -101, 88}, // Matrix B row
		{77, -106, 83, 10, -20, 119, -67, 88, -84, -93, 6, 31, 112, 49, 61, -76}, // Matrix B row
		{-101, 110, -109, -50, 57, -80, 108, 100, -100, 52, -96, -18, -126, 112, 88, 95}, // Matrix B row
		{107, -110, 70, 21, -99, -32, -126, -22, 17, -78, -62, 12, -73, 26, 109, -109}, // Matrix B row
		{78, 92, 56, -105, 54, 127, 101, -109, -45, -118, 35, 16, -28, 8, 50, 15}, // Matrix B row
		{-21, -36, 94, -95, 29, -75, -39, -101, 63, 118, -19, -87, 70, -79, 119, -9}, // Matrix B row
		{-80, 22, 40, -123, -72, 10, 9, 14, -93, 6, -73, -7, -27, 56, 93, 54}, // Matrix B row
		{37, 97, -31, 1, 56, 39, -89, -1, -59, 18, -123, -112, 62, -113, 122, -117}, // Matrix B row
		{44, -110, -110, 119, -29, 68, -32, -1, -90, 68, 91, -51, -85, -53, -60, 42}, // Matrix B row
		{-3, -74, 58, 45, -17, 32, 113, 56, 101, -58, -119, 57, -118, -71, 83, 71}, // Matrix B row
		{-79, -103, -59, 60, -55, -112, -40, 3, -111, -105, -90, 28, 21, 86, -72, 20}, // Matrix B row
		{-76, -53, 109, 37, -32, -60, 76, 61, 59, 6, 27, -73, -39, -31, -79, 87}, // Matrix B row
		{90, -1, -124, -33, 108, 20, -119, 30, 69, 121, 100, 124, -114, 23, 54, 3} // Matrix B row
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