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
		{76, -11, -99, 11, -127, -77, 112, 97, -27, 63, -115, 109, 34, -35, -70, 43}, // Matrix A row
		{62, -114, -96, -112, 26, -44, -43, 42, -4, -75, -45, -98, 36, -107, -20, 91}, // Matrix A row
		{30, -107, -96, 92, 67, 86, -99, -50, 30, -107, -29, 5, 89, 18, 8, 31}, // Matrix A row
		{-5, -123, 3, -123, -24, -17, -5, 86, 30, -110, 79, -118, 89, 53, 16, 45}, // Matrix A row
		{-93, 15, -66, -36, 31, 58, -19, 33, 4, -33, 108, 59, -55, 126, 25, 7}, // Matrix A row
		{-117, 15, 30, 84, 95, 56, 88, 9, -107, -4, 16, -82, -65, -114, 87, -53}, // Matrix A row
		{113, 103, 23, 40, 4, 63, 0, 54, 2, -125, -42, -95, -119, -104, 45, 61}, // Matrix A row
		{-7, 41, -38, 14, -105, -31, 39, -23, 109, 41, 87, -61, -122, -22, -6, -55}, // Matrix A row
		{72, -62, -17, -57, 67, 19, 55, -5, -20, -41, 88, 33, -62, 78, 19, 108}, // Matrix A row
		{111, 55, 29, -30, 113, -25, 35, -7, 57, -110, -30, 56, 116, -121, 43, -94}, // Matrix A row
		{-75, 28, 101, 50, 96, -1, 64, -23, -59, -123, -11, 39, 3, 109, -36, 64}, // Matrix A row
		{-89, -11, 102, -62, 95, -116, 18, 120, -77, 19, -34, 126, -112, 34, 8, -116}, // Matrix A row
		{109, -88, -54, -93, 12, -96, -23, 16, -93, -45, 106, 35, 32, 101, -17, 113}, // Matrix A row
		{-114, 19, -5, 27, 14, 87, -38, -37, -36, 108, -127, 115, 99, -117, -59, -16}, // Matrix A row
		{-91, 81, 97, -122, -36, -7, -42, -91, -85, -70, -7, -81, 109, -60, 59, 8}, // Matrix A row
		{-43, -69, 8, 83, -88, 109, 100, 12, -66, 38, 71, -74, 23, 38, -53, 62} // Matrix A row
	}; // Matrix A row

  // Matrix B, elem_t, B_rows, B_cols Setup
	elem_t B[B_rows][B_cols] = { // Matrix B row
		{3, 120, -42, -77, -112, 105, 123, -46, 85, 108, -39, 25, -97, -89, -65, 21}, // Matrix B row
		{-96, 43, -78, -88, -93, -97, -22, 31, -72, -118, 36, 114, 48, 6, -72, 105}, // Matrix B row
		{116, -66, 82, 45, 52, -63, -49, -16, -122, -118, -98, -72, -37, 65, 110, 101}, // Matrix B row
		{-29, -50, -54, -118, -76, -59, -101, -24, -55, 84, 66, 61, 18, 111, 96, 29}, // Matrix B row
		{5, -64, -49, 1, -17, -56, -8, 9, 37, 95, 48, 7, 115, 70, 32, -61}, // Matrix B row
		{6, -73, 120, -124, 79, 7, 94, -119, 63, -37, 44, -17, 86, 101, 108, 52}, // Matrix B row
		{-34, -26, -59, 37, -58, 67, 13, 21, 94, -88, -58, 57, -33, 47, -78, -103}, // Matrix B row
		{-60, -109, 118, -77, 85, -58, 58, 23, 89, 30, 49, -1, -72, -18, 24, 41}, // Matrix B row
		{-101, 64, 111, -51, -31, 66, -73, 87, 26, -87, -113, -24, -88, -93, 4, -71}, // Matrix B row
		{-108, -110, -52, -113, 59, -113, 43, -87, 25, -28, 127, -82, 113, -113, 14, -73}, // Matrix B row
		{-81, 51, 98, 9, 100, -95, -93, 41, -63, -6, -38, -51, -101, -119, 119, 100}, // Matrix B row
		{62, 90, -5, -88, 66, -61, 10, 101, 93, 56, -11, -9, 121, -20, 122, -95}, // Matrix B row
		{-75, -60, 92, -3, 14, 37, 116, -51, 89, 107, 70, -17, 47, -120, 7, -53}, // Matrix B row
		{-113, 87, -121, -45, 88, -107, -79, -43, -127, -21, -24, -15, -116, 55, 13, 123}, // Matrix B row
		{-121, 93, 14, -9, 63, -119, 125, 45, 65, 53, 81, -24, 35, -9, -23, 26}, // Matrix B row
		{75, 123, -90, 126, 84, 30, 73, 64, -29, 55, 44, -59, -127, -25, -38, -126} // Matrix B row
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