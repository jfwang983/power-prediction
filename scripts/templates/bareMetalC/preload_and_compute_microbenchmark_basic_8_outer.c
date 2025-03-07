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
#define A_rows 8
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
		{-6, -76, -91, 5, -52, 127, -41, -105, -12, -107, 6, -55, 112, -103, -56, -58}, // Matrix A row
		{100, 29, 79, 126, -28, 40, -23, -45, -17, -63, 99, 3, -76, 31, 127, -33}, // Matrix A row
		{12, -33, -119, -72, 27, 36, 26, 57, 81, 61, -2, 116, -9, 113, 36, 16}, // Matrix A row
		{21, -55, 53, -35, -69, -65, 51, -73, 16, 56, 17, 23, 111, 96, 127, 101}, // Matrix A row
		{53, -90, -4, 95, 49, -63, 84, 56, -121, 19, 100, -55, 31, 120, 116, -98}, // Matrix A row
		{53, 9, -87, 45, 51, 124, -97, 122, -107, 124, -23, 123, -4, 45, -58, 4}, // Matrix A row
		{-13, -16, 56, -93, -68, -28, -60, -35, -104, -80, 56, 33, 120, 116, -106, 30}, // Matrix A row
		{-102, 21, 29, -122, 12, -72, 91, 84, -127, 113, 103, -54, -29, -38, 41, 93} // Matrix A row
	}; // Matrix A row

  // Matrix B, elem_t, B_rows, B_cols Setup
	elem_t B[B_rows][B_cols] = { // Matrix B row
		{-101, -59, 15, -26, 5, 86, 21, -34}, // Matrix B row
		{49, -4, -86, -119, 53, -108, 70, -100}, // Matrix B row
		{34, 84, 103, -82, -51, 45, -3, -77}, // Matrix B row
		{-59, 3, 13, -59, 1, -43, -44, -10}, // Matrix B row
		{-20, 85, 117, 47, 31, 57, 46, -85}, // Matrix B row
		{-62, 68, 106, 119, 124, -13, -58, 108}, // Matrix B row
		{63, 83, 40, 73, 63, 122, 35, -34}, // Matrix B row
		{56, -126, 112, -101, 94, -10, -92, -50}, // Matrix B row
		{-56, 74, -8, 90, -96, 104, 86, -89}, // Matrix B row
		{31, 81, 125, -27, 51, 58, 43, 82}, // Matrix B row
		{42, 29, -118, 1, 99, -70, -58, -27}, // Matrix B row
		{20, 29, 63, 49, -24, 15, -41, -11}, // Matrix B row
		{-5, 54, -81, 101, 66, -24, -77, 0}, // Matrix B row
		{82, -53, 62, 21, -64, -4, -95, -19}, // Matrix B row
		{68, -109, -68, -40, 53, -50, 4, -51}, // Matrix B row
		{126, -6, 54, 18, 64, -103, -101, -3} // Matrix B row
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