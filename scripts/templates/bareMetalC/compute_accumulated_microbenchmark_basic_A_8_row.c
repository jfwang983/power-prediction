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
		{-50, -23, 110, 14, 61, 64, 92, -69, -125, -25, 75, -88, -97, 57, 102, 65}, // Matrix A row
		{95, -13, 15, 113, -4, -124, -116, -62, -14, -12, 50, -93, -22, -24, -114, -85}, // Matrix A row
		{-105, 127, -7, 49, 41, 42, -109, 125, 94, 57, -41, 32, -92, 21, -24, 68}, // Matrix A row
		{-1, 53, -1, -61, -92, 77, 61, -5, 125, -71, -70, -107, 33, -89, -40, -82}, // Matrix A row
		{-15, -106, -81, 110, -120, -10, -123, 115, 85, 71, -6, -72, 104, -85, 61, 69}, // Matrix A row
		{-107, -31, -96, -81, -12, -115, -100, 47, 88, 115, -72, 10, 94, 50, 110, 37}, // Matrix A row
		{-57, 20, 71, 69, 42, 64, -22, -21, -104, 33, 113, 8, -77, -112, 41, 12}, // Matrix A row
		{-11, 21, 103, -97, -51, 48, 21, 124, 64, 5, 106, -109, -61, -94, 104, 13} // Matrix A row
	}; // Matrix A row

  // Matrix B, elem_t, B_rows, B_cols Setup
	elem_t B[B_rows][B_cols] = { // Matrix B row
		{-57, 21, 118, -59, -22, -112, -127, 100, 19, -110, 9, -66, 77, -114, 69, -89}, // Matrix B row
		{14, 97, 113, -108, 82, 76, 101, 36, 98, -118, 68, -30, -110, -21, -34, 8}, // Matrix B row
		{83, -25, 120, 48, -43, 107, 44, -34, -88, 57, -38, 67, -19, -19, 72, -24}, // Matrix B row
		{-116, -122, 82, 55, 104, -114, -61, -37, -122, -99, -54, -121, 86, 31, 89, 95}, // Matrix B row
		{-21, 68, 127, 46, -37, -95, -111, -13, -28, -126, -115, 119, 28, -71, 93, 46}, // Matrix B row
		{-48, 49, 9, 80, -59, 33, 78, 67, -9, -11, -115, -81, 46, 124, 98, -118}, // Matrix B row
		{101, -61, 121, 68, -22, 110, 123, -112, 21, 6, -106, -34, 89, 49, -53, 118}, // Matrix B row
		{90, 97, 114, 34, 4, -30, 27, -34, -33, -103, -43, 87, 97, 43, 82, 54}, // Matrix B row
		{-50, 36, -27, 55, -15, -58, -126, -118, -121, -123, 35, -75, 6, -117, -43, 120}, // Matrix B row
		{-118, -43, -21, -42, 41, 98, -37, 13, 48, -39, 83, -27, 55, -91, -128, -55}, // Matrix B row
		{-51, -26, -108, 98, 5, -108, -122, 0, -104, -26, 97, 28, 100, 21, 9, 117}, // Matrix B row
		{-46, -17, 59, -69, 23, -89, 98, 35, -16, 40, -128, 57, 67, -123, 48, -67}, // Matrix B row
		{-52, -71, -19, 126, 73, 121, 55, 24, -15, -103, 120, -120, 125, -8, 100, 62}, // Matrix B row
		{-89, -51, -29, 100, 38, -51, -76, 37, 21, -46, -9, -85, -1, -19, 28, 21}, // Matrix B row
		{85, -31, -26, -51, 92, -104, -87, -96, -41, 117, -119, 47, 124, -39, -19, 105}, // Matrix B row
		{125, -100, 50, -38, -66, -50, -128, 119, 10, 75, -13, -54, -86, -72, -29, 61} // Matrix B row
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