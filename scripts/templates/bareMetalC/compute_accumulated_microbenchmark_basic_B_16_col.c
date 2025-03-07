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
		{39, 55, -37, 60, -36, -22, -93, 55, 104, 64, -44, -65, -15, 28, -66, -8}, // Matrix A row
		{-67, 52, 102, 29, 73, -100, 10, -53, 99, 17, 33, -102, -28, -76, 62, -15}, // Matrix A row
		{-45, 3, 95, -127, -42, 24, 98, 9, -82, -88, -75, 110, 78, 127, -61, -101}, // Matrix A row
		{-113, -128, -16, -94, 21, -98, -76, -104, 104, 118, -122, 102, -9, -5, -4, -20}, // Matrix A row
		{-59, 28, 119, 77, 112, -126, -3, 23, -95, 60, 18, -121, 94, 4, 11, 35}, // Matrix A row
		{26, 25, -45, -60, 34, 51, 125, -101, 44, -78, 22, 79, 115, -15, -15, 23}, // Matrix A row
		{-10, 55, -39, 61, 72, -59, -64, -25, -19, 79, -67, 114, 43, 45, 29, -45}, // Matrix A row
		{-59, 59, -22, 86, -99, -72, 110, 4, -43, -20, -52, 99, -127, 78, 88, -34}, // Matrix A row
		{11, 28, 36, -109, -29, -124, 44, -101, 104, -88, 76, 110, 64, -42, -82, -6}, // Matrix A row
		{-60, 54, 42, -6, 83, 102, 70, -95, 66, 90, -126, -28, -65, -32, -62, 88}, // Matrix A row
		{108, 108, 29, 88, 36, 30, -54, -115, -106, -23, -102, -121, 76, -74, -113, 23}, // Matrix A row
		{54, -14, -75, -47, -74, -2, 34, 20, 51, 86, 8, 110, -127, -11, 86, -50}, // Matrix A row
		{41, -16, 120, -96, -42, -118, 57, -87, -108, -92, 123, -53, 69, 54, 58, 77}, // Matrix A row
		{-33, 111, 42, -83, -93, -109, 23, -2, 22, 77, -119, -75, -94, -12, -54, 76}, // Matrix A row
		{-68, -114, 113, 2, -6, 67, 44, 69, -121, -125, 15, 74, 64, -111, 27, -34}, // Matrix A row
		{-80, 58, 64, 8, -118, -79, -74, -61, -10, -87, -119, 119, 108, 74, -70, -22} // Matrix A row
	}; // Matrix A row

  // Matrix B, elem_t, B_rows, B_cols Setup
	elem_t B[B_rows][B_cols] = { // Matrix B row
		{121, -120, 103, 99, -45, -122, 47, 67, 74, -27, 89, -8, -95, -112, 53, -110}, // Matrix B row
		{-125, 91, -21, -71, -14, 39, -42, -45, -26, 27, -82, 8, -37, -16, 28, -60}, // Matrix B row
		{88, 107, -76, -99, -49, 58, -31, 33, 11, -83, 111, -120, -101, -85, -106, 1}, // Matrix B row
		{105, 117, 115, -61, -105, -104, 102, -2, 88, -19, 81, 5, 86, -118, -89, 24}, // Matrix B row
		{-125, 32, -117, 20, -9, 23, -17, -89, -103, 126, 39, 13, -65, 97, 15, 20}, // Matrix B row
		{68, 114, 76, 101, 109, 112, -42, -97, 117, 83, -119, 126, -116, 86, -113, 61}, // Matrix B row
		{-43, -8, 22, -64, 10, 39, -91, 83, 105, -87, 84, -2, -117, 0, 19, -45}, // Matrix B row
		{108, -80, -23, 49, 104, -23, 124, 37, 3, -74, 95, 20, 2, 28, -92, -104}, // Matrix B row
		{-25, -94, -13, 54, 18, -46, 43, 41, 103, -86, -75, 14, -113, 74, 59, 27}, // Matrix B row
		{33, -92, 122, 116, 27, 84, 97, 16, 69, 2, 71, 19, 103, -16, -63, -72}, // Matrix B row
		{-19, -48, -57, 55, 111, 60, -62, 105, -65, 34, -23, 64, -57, -97, 70, 40}, // Matrix B row
		{87, 87, -120, 9, -92, 8, 29, 121, -53, -72, -41, -31, 80, 84, 5, 15}, // Matrix B row
		{29, 100, -81, 42, 57, -74, -103, 66, -100, -96, 110, 55, -79, -126, 116, 35}, // Matrix B row
		{89, 115, 107, 113, 31, 46, 30, -95, 112, -104, -104, 75, -79, 30, 21, 52}, // Matrix B row
		{-14, 103, -35, 103, -46, -116, -54, -123, 47, -99, -86, 108, -85, 22, 87, 40}, // Matrix B row
		{-17, 30, -40, 13, 102, 25, 88, -93, -60, -10, -79, 43, 80, 73, 9, -73} // Matrix B row
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