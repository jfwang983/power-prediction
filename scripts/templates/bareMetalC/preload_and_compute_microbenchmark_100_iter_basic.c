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

  int iterations = 100;

  // Matrix A, elem_t, A_rows, A_cols Setup
	elem_t A[A_rows][A_cols] = { // Matrix A row
		{-28, -4, 103, -46, 60, -117, 59, 125, -106, -54, 68, 89, -95, 56, -8, 89}, // Matrix A row
		{22, 115, 41, -54, 15, -101, -5, 88, 100, -99, -66, 106, 69, -94, 55, 7}, // Matrix A row
		{100, 72, 33, 12, -122, -120, 30, -19, 100, 64, 78, -30, 70, 13, 104, 27}, // Matrix A row
		{107, 48, -31, 118, 63, 17, 82, -29, 65, -103, -66, -57, -93, -32, -74, -98}, // Matrix A row
		{-99, 42, -65, 100, 24, 3, 108, -80, 34, 118, -127, 110, 14, -65, 44, 77}, // Matrix A row
		{55, 45, -115, 120, 84, -62, -77, -114, -73, -57, 127, -5, -21, 12, -120, 86}, // Matrix A row
		{-55, -92, -95, -17, -21, -37, 103, -19, -89, 125, -88, 59, -102, 45, -25, 107}, // Matrix A row
		{118, -24, 65, 23, 19, 33, 2, 41, -47, 55, 15, 83, 35, -61, 39, -23}, // Matrix A row
		{57, -30, 64, 114, 65, 72, -7, 43, -86, 17, -75, -99, 68, -90, -49, 1}, // Matrix A row
		{-51, 97, 92, 84, 2, 81, -100, -115, 113, 43, -64, -110, -43, -71, 42, 125}, // Matrix A row
		{100, 26, 105, 93, 60, 72, -127, -81, 77, -107, 94, 83, -102, -35, -109, 28}, // Matrix A row
		{18, -73, 60, -20, 89, -109, 39, -117, 102, 51, -82, -109, -71, 19, 105, 62}, // Matrix A row
		{78, -121, 60, -30, 14, 43, 41, 90, -87, -44, 70, -20, 79, -82, 126, 29}, // Matrix A row
		{42, 87, -126, 23, -20, -24, -34, 40, -41, 126, 15, -122, -63, -116, 2, -91}, // Matrix A row
		{-2, -106, 115, -87, -38, -106, 44, 39, 79, -124, -42, 87, 125, -18, 77, -78}, // Matrix A row
		{43, -69, -73, -104, 102, 120, 6, 40, -39, 100, 127, 35, 97, 19, -107, 80} // Matrix A row
	}; // Matrix A row

  // Matrix B, elem_t, B_rows, B_cols Setup
	elem_t B[B_rows][B_cols] = { // Matrix B row
		{-90, -110, -88, -39, 0, 71, 33, 37, -72, 88, -16, -110, 45, 33, 105, -64}, // Matrix B row
		{-24, -121, 8, 26, -63, 54, -67, -46, 112, 95, 10, 52, -62, 53, 59, -29}, // Matrix B row
		{-7, 111, -63, 111, 4, -48, 62, -64, -31, 27, -77, -78, 53, -55, 16, -45}, // Matrix B row
		{-53, -52, 29, 6, 14, -93, 72, 52, -16, -44, -9, 76, 44, 104, -67, 73}, // Matrix B row
		{-46, 94, 95, -45, -12, -67, -105, 51, 126, -65, 47, 6, 74, -83, 103, -100}, // Matrix B row
		{11, -7, 46, 87, -74, 9, 15, 54, -32, -17, -28, 62, 24, -103, 108, 37}, // Matrix B row
		{93, -23, -88, -98, -4, -68, 86, -11, 122, -118, 116, 64, 76, -3, 83, -68}, // Matrix B row
		{-33, -56, 18, 70, 37, -30, 119, -74, -127, -4, 39, 1, 87, -71, 73, -5}, // Matrix B row
		{102, 98, -107, -41, 110, -19, -63, 70, -13, -30, 99, -70, -59, 105, 101, 96}, // Matrix B row
		{-3, 25, -77, 21, 7, 44, 2, 87, 97, -97, 70, -45, 109, -121, -126, 44}, // Matrix B row
		{42, -83, -92, 124, 26, 9, -59, 75, -97, -56, -98, 118, -73, 26, 9, 17}, // Matrix B row
		{13, -22, -123, -64, -11, -29, -114, -77, -92, 122, 71, 35, -5, 125, 33, 14}, // Matrix B row
		{-82, -24, 63, -101, -93, 78, -50, 123, 55, -112, 10, 101, -48, 89, -52, -125}, // Matrix B row
		{-66, 20, -37, -26, 121, -114, -24, 109, 126, -75, 91, 72, -88, -18, -55, -90}, // Matrix B row
		{-64, -9, 59, 113, 92, 89, -5, 83, 12, 37, -98, 27, 27, -95, -41, 74}, // Matrix B row
		{100, -85, -92, -51, -33, 72, 42, -57, 0, 47, 106, 107, -22, -1, 87, -72} // Matrix B row
	}; // Matrix B row

  elem_t C[C_rows][C_cols];

  uint32_t A_sp_addr = 0;
  uint32_t B_sp_addr = DIM;
  uint32_t C_sp_addr = 1 << 31 | 1 << 30;

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