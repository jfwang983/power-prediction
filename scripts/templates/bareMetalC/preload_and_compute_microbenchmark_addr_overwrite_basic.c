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
		{-11, 104, -5, -49, 64, 62, 15, -96, -40, 5, 53, -5, -100, -16, -56, 70}, // Matrix A row
		{7, 68, 121, -126, -14, -55, 17, -111, -1, -52, -28, 25, -82, -57, 113, 13}, // Matrix A row
		{-59, -77, -123, -58, 0, -9, 48, -120, -17, -15, 17, -26, -1, -73, 37, -61}, // Matrix A row
		{-114, 112, -42, -57, 13, 111, 64, 37, 124, -121, 101, 51, 116, -108, 43, 17}, // Matrix A row
		{36, 68, 92, -59, -6, 17, 125, -38, -100, 48, -68, -55, -125, -103, 8, 90}, // Matrix A row
		{-48, -120, 56, -42, -12, -68, 25, -119, 103, 50, -121, 118, -121, -15, -95, 97}, // Matrix A row
		{40, 65, -76, 70, -106, 29, -110, 70, -35, -15, -87, -40, -121, -126, -62, 31}, // Matrix A row
		{110, 100, -74, 45, -66, -58, 122, 20, -59, 78, 101, 87, -60, 41, 21, 75}, // Matrix A row
		{34, -83, 68, -67, -94, -115, 110, -30, 105, -40, 102, 73, -74, -1, 98, -108}, // Matrix A row
		{43, -16, 3, -27, -40, -32, 6, -116, 113, 84, -55, -47, -94, 70, 25, -116}, // Matrix A row
		{-78, 60, 6, -91, -55, 75, 85, 53, 65, -64, -64, 91, 10, -92, 125, 52}, // Matrix A row
		{-82, -11, 7, -74, -37, -25, -91, -58, -36, 84, -63, 23, 11, 95, 104, -34}, // Matrix A row
		{-115, 18, -74, 64, -91, -98, 53, 38, 20, 33, -27, -34, 1, 3, 100, -121}, // Matrix A row
		{21, -113, -85, -69, 1, 49, 70, -37, -19, 28, 5, 41, 40, 51, 28, 79}, // Matrix A row
		{-21, 6, -111, 46, 23, 32, -16, 101, -19, 56, -115, -122, -100, -78, -22, -44}, // Matrix A row
		{-87, 91, 58, -57, -65, 104, 5, -24, 27, -67, 77, 8, -92, 8, 6, 15} // Matrix A row
	}; // Matrix A row

  // Matrix B, elem_t, B_rows, B_cols Setup
	elem_t B[B_rows][B_cols] = { // Matrix B row
		{113, 64, -39, 48, 109, -116, -102, 40, 68, 108, -44, 71, 32, -21, -41, 108}, // Matrix B row
		{2, 43, -116, -54, -32, -75, 29, -17, -86, 55, 56, -123, 31, 71, -75, -73}, // Matrix B row
		{92, 10, 124, 82, -80, 57, 39, 107, 22, 22, -79, -90, 93, 59, -118, 51}, // Matrix B row
		{-109, 15, -56, -24, -62, 77, -84, -13, -48, 55, -32, -53, 31, 126, 69, 100}, // Matrix B row
		{58, 59, -53, -115, -46, 69, -10, -49, -69, 123, 50, -91, 38, -4, -42, -49}, // Matrix B row
		{8, 55, -64, -61, 106, 54, 59, 87, -68, 62, -26, 6, -119, -95, -69, -128}, // Matrix B row
		{122, -17, 105, 25, 84, -118, 118, -94, 6, 78, 102, 31, -75, 27, 40, 54}, // Matrix B row
		{-64, 113, 43, 124, 17, -92, -120, -50, -32, 65, 110, 94, 87, 102, 27, 71}, // Matrix B row
		{57, -31, -72, 125, 10, 84, -32, -18, -55, -58, -55, 23, -77, -91, 53, -118}, // Matrix B row
		{23, -55, 52, -33, 85, 24, 12, 69, -34, -42, 52, 127, -46, 22, 117, 82}, // Matrix B row
		{-84, 57, -17, 10, -40, -38, 9, -17, 68, -56, 59, -66, -19, 52, -31, -6}, // Matrix B row
		{32, -36, -51, 93, 2, 91, 22, 18, 16, 95, 86, 23, 42, 0, -57, 94}, // Matrix B row
		{-11, -63, -81, 34, -39, -83, -125, 126, 34, -120, -30, 75, -65, 97, 42, 7}, // Matrix B row
		{80, -118, -23, -110, -73, -71, 87, 41, 124, -60, -88, -55, -85, 94, 127, 2}, // Matrix B row
		{84, -102, 67, -40, 89, 112, 14, 118, 107, -75, -116, 67, 22, -86, 67, -76}, // Matrix B row
		{101, -88, -32, -102, 26, -6, -37, 109, 6, -112, -11, -66, -108, -114, -23, -35} // Matrix B row
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