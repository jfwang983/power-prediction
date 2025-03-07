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
		{-32, -19, 11, -44, 115, -56, -128, -127, -74, 95, 120, -16, 94, -60, 82, 101} // Matrix A row
	}; // Matrix A row

  // Matrix B, elem_t, B_rows, B_cols Setup
	elem_t B[B_rows][B_cols] = { // Matrix B row
		{-108, 92, 59, 84, -61, -53, 99, -50, -86, -80, -98, -4, -114, -9, -29, -34}, // Matrix B row
		{-107, 81, 11, 76, -117, -6, -16, -127, 97, -64, 73, 111, 62, -88, -128, 29}, // Matrix B row
		{-82, 68, 60, 41, -15, -94, 104, 9, 116, -105, -25, -90, -5, -99, 46, -26}, // Matrix B row
		{82, 60, -18, 63, -12, 111, 57, -13, 28, -55, -4, -58, -4, -43, 48, -81}, // Matrix B row
		{77, -63, 36, 11, -116, 84, 67, -120, 26, 122, -92, 39, -25, 71, -37, -24}, // Matrix B row
		{57, -62, 36, -59, -26, -107, -37, 41, -2, -16, 90, -58, 119, -85, 101, -113}, // Matrix B row
		{-63, 82, 42, -74, 28, 59, -42, 101, -42, -6, 61, -45, -14, -39, -108, -31}, // Matrix B row
		{-110, -19, -31, 63, -32, -98, -76, 107, 50, -69, 112, 100, -100, -87, -89, 37}, // Matrix B row
		{87, 106, -115, 29, 72, -18, -51, -44, 99, -48, -21, -75, 87, 60, 118, 17}, // Matrix B row
		{-101, -96, -12, -37, 91, 43, 50, 126, -87, 89, 113, 48, -120, -125, 87, 19}, // Matrix B row
		{111, 31, -89, -103, 54, -81, 58, -17, 9, -112, -102, -17, 103, 42, 100, 72}, // Matrix B row
		{62, 112, -112, 36, 70, 77, -28, 86, 37, 46, 102, -46, -86, 90, -42, -117}, // Matrix B row
		{22, -23, 123, 35, -95, 120, -1, -44, -75, -83, 51, -91, -39, -39, 50, 96}, // Matrix B row
		{-121, -55, 40, -8, 90, -124, -69, -53, -49, -10, -125, -31, 40, -21, -102, 74}, // Matrix B row
		{-57, -51, 113, 60, 14, 54, 15, -89, -32, -47, -84, -107, 106, 78, 101, 122}, // Matrix B row
		{-2, -11, -56, -35, 55, -75, -3, 69, 18, 82, -82, -38, -6, 14, 100, -109} // Matrix B row
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