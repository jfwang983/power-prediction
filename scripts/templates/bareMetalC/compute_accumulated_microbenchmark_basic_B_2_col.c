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
#define B_cols 2
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
		{-45, 74, 58, -57, 82, -20, -113, 57, 70, -15, 86, 103, -49, -39, 76, 31}, // Matrix A row
		{57, -111, -127, 53, -7, -2, -5, -4, 78, 106, -36, 64, 35, 66, -37, 2}, // Matrix A row
		{-29, 30, -101, 100, 43, 113, -50, 87, 55, 80, 44, 80, 122, -87, 102, -22}, // Matrix A row
		{34, -42, 34, 124, 5, 60, -123, 90, 23, 88, -40, 122, 122, 54, 49, 100}, // Matrix A row
		{8, 22, 18, 99, -20, 83, -117, 27, -113, 81, -2, -7, -77, -117, 57, 80}, // Matrix A row
		{-103, -64, 77, 67, 2, -80, -61, 100, -84, 27, 73, -68, -88, 115, 119, -26}, // Matrix A row
		{-59, -17, -32, 80, -92, -91, -7, 54, -16, -64, 102, 55, 55, -121, -64, 29}, // Matrix A row
		{21, 96, 17, 94, -108, 40, 39, 65, -82, -67, -124, 114, 52, -30, 105, 43}, // Matrix A row
		{96, -31, -81, 72, -2, -43, 5, -19, 70, 74, 113, 14, -73, 36, -70, -118}, // Matrix A row
		{19, 58, -50, -6, 92, -68, -18, -19, 123, 67, -74, 106, 34, -84, 51, -1}, // Matrix A row
		{91, 106, -73, 67, 23, -1, -85, -44, -91, 125, 40, -2, 15, 5, -5, 26}, // Matrix A row
		{7, -128, 56, -49, 46, 22, 49, -80, -66, 66, 69, -58, -128, 15, 8, 28}, // Matrix A row
		{124, -44, -71, -60, 4, 15, -43, -108, 27, 98, -107, 117, -81, 76, -108, 91}, // Matrix A row
		{46, -104, 87, 32, 82, 94, -49, 28, -30, 7, -56, 110, -60, -114, -14, -102}, // Matrix A row
		{16, -45, -108, -32, -9, -83, 43, 51, 84, -20, -108, 104, -49, 110, 23, -126}, // Matrix A row
		{9, -86, -21, 76, -57, -28, 1, 40, -4, 58, -53, -47, -100, 56, 26, -86} // Matrix A row
	}; // Matrix A row

  // Matrix B, elem_t, B_rows, B_cols Setup
	elem_t B[B_rows][B_cols] = { // Matrix B row
		{78, 54}, // Matrix B row
		{-61, -69}, // Matrix B row
		{97, -18}, // Matrix B row
		{77, 26}, // Matrix B row
		{-67, 9}, // Matrix B row
		{-97, 48}, // Matrix B row
		{95, 47}, // Matrix B row
		{40, 23}, // Matrix B row
		{28, 103}, // Matrix B row
		{-97, -6}, // Matrix B row
		{88, 47}, // Matrix B row
		{-71, -102}, // Matrix B row
		{-89, -49}, // Matrix B row
		{-96, -15}, // Matrix B row
		{52, 37}, // Matrix B row
		{-116, -53} // Matrix B row
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