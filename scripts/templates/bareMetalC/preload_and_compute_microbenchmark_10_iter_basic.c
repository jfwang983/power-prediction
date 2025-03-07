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

  int iterations = 10;

  // Matrix A, elem_t, A_rows, A_cols Setup
	elem_t A[A_rows][A_cols] = { // Matrix A row
		{5, 44, -57, 76, 42, -85, -118, 115, 100, -88, 16, -119, 32, -74, 61, -6}, // Matrix A row
		{-102, -91, -91, 57, 90, -28, 22, 0, -18, -66, -5, -5, -50, 64, -50, 121}, // Matrix A row
		{51, -35, -95, 49, -64, -18, -30, -25, -90, -12, 54, 16, 54, -118, -6, -34}, // Matrix A row
		{-9, 108, 69, 88, -6, -48, 47, -123, 31, -27, -107, 80, 48, -109, -58, -16}, // Matrix A row
		{103, 2, -124, 35, 88, 69, -96, 33, 82, 17, 28, 126, -128, -87, -48, 111}, // Matrix A row
		{-78, 72, -108, 24, -14, -57, -105, -16, 35, 11, -113, 25, -78, 57, 125, -38}, // Matrix A row
		{-26, 12, -9, 10, -34, -15, 39, -18, -108, 66, 31, 65, -56, 3, 45, -62}, // Matrix A row
		{-123, 80, 37, 4, 39, 2, -118, 123, -46, -30, -59, 75, 68, -5, -29, -38}, // Matrix A row
		{16, -20, -51, 18, 56, -56, 49, 24, -37, 94, 32, -87, -64, 50, 107, 93}, // Matrix A row
		{56, 42, -121, -72, 42, 79, -100, 100, -34, -119, -66, 118, 33, -3, 92, -123}, // Matrix A row
		{97, -19, -124, 101, -11, -123, 1, 41, 75, 29, -76, -4, -119, 13, 98, -126}, // Matrix A row
		{-115, 5, 23, -78, 46, -1, -95, 99, 69, -31, 9, 79, -32, -63, 72, -68}, // Matrix A row
		{-96, 122, 43, -71, 42, -122, 94, -16, -95, 118, -79, -44, -54, -44, 46, 85}, // Matrix A row
		{114, -55, -41, -43, 117, -48, 121, 39, -65, -100, 101, 56, -101, 127, -59, -123}, // Matrix A row
		{-48, -99, 17, 9, 104, 27, 30, 9, 49, 34, -57, 117, 17, 31, 94, -106}, // Matrix A row
		{70, -80, -92, -3, -91, 88, 69, 18, 95, 97, -55, 26, 9, 33, -100, -115} // Matrix A row
	}; // Matrix A row

  // Matrix B, elem_t, B_rows, B_cols Setup
	elem_t B[B_rows][B_cols] = { // Matrix B row
		{-83, 90, 108, -43, 116, 119, 78, 9, -119, -31, 4, 101, -126, 17, -83, 16}, // Matrix B row
		{-116, -50, 2, -120, 8, -75, -10, 19, 58, -6, 113, -83, -100, -100, 61, -88}, // Matrix B row
		{-86, -93, -101, -56, 90, 24, -113, 122, -83, 46, -78, -94, -121, 23, -45, -25}, // Matrix B row
		{115, -25, -3, -1, -116, -75, 2, 65, 74, -1, 6, 103, -51, 58, 91, -57}, // Matrix B row
		{51, 60, -74, -93, -128, -31, 36, 111, 0, -83, -42, 116, 124, 111, -103, 108}, // Matrix B row
		{62, -27, -32, 69, -98, -4, 100, -54, 27, -43, 8, 55, -62, 12, 79, -45}, // Matrix B row
		{12, -84, -42, 41, -40, -50, -106, 25, 122, -25, 14, 37, -92, 100, 82, -82}, // Matrix B row
		{112, -40, 19, -32, 82, -91, -29, -48, -30, -22, -128, 80, -13, 53, -67, -66}, // Matrix B row
		{16, 22, -2, -36, 99, -5, 2, -100, 119, 66, -94, 113, -113, -20, 116, 70}, // Matrix B row
		{-117, -55, -100, 86, -8, -46, -14, -46, -3, 1, -30, 59, 82, 43, 101, 100}, // Matrix B row
		{-54, 115, 125, -104, 45, 74, -117, 15, 46, -19, 66, -39, 67, -97, -43, 125}, // Matrix B row
		{24, -91, 124, 100, -104, -116, 14, 21, -89, 18, 56, -5, 22, 19, -87, -120}, // Matrix B row
		{28, -33, -17, -97, 109, -39, -95, 48, 101, 80, 31, -82, -14, 98, -63, -111}, // Matrix B row
		{121, 119, -98, -38, -69, 94, -97, -26, -25, 104, 62, 2, 57, 99, -49, 45}, // Matrix B row
		{-85, 112, 44, 31, 53, 24, -127, -78, -85, -81, -106, 23, -127, -87, -85, 107}, // Matrix B row
		{41, 113, -31, -65, -62, -41, 118, 0, -83, 1, 90, 117, -127, -79, 74, 60} // Matrix B row
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