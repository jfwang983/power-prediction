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
		{-74, 63, 57, -56, -34, -15, -6, -99, 114, -103, 54, 94, -73, 47, -32, 113}, // Matrix A row
		{-71, 44, -48, -55, 24, -14, -113, 68, 42, -79, -61, -79, -11, 48, -36, 77}, // Matrix A row
		{-6, 88, 10, 76, 43, -15, 23, 119, -98, -6, -43, 41, -53, 93, 76, -103}, // Matrix A row
		{-8, -18, 85, -87, -66, -55, -60, 99, 24, 34, 114, -27, 59, 97, -78, -71}, // Matrix A row
		{-81, -42, 119, -40, 31, 76, -67, -68, -15, 15, -125, -118, -61, -105, -79, 39}, // Matrix A row
		{-91, -52, -122, -43, -124, -72, 104, 48, -10, -30, -11, 79, -46, -17, -10, -95}, // Matrix A row
		{-80, 97, -28, 65, 22, 27, 60, -105, -64, 35, 37, -54, -65, 28, -36, -63}, // Matrix A row
		{32, -87, 4, -126, 1, 59, 3, -52, 7, 11, -77, 117, -12, -38, -84, -39}, // Matrix A row
		{101, -107, -62, 126, -76, 81, 0, -81, -47, 124, -96, 61, 70, 45, -119, 52}, // Matrix A row
		{-20, 48, 125, -30, 42, -21, 69, 14, -83, -34, -26, -76, 49, -121, 9, -18}, // Matrix A row
		{71, 55, 79, 112, -128, -42, 1, -111, 103, 65, -29, 57, -2, -72, 78, -67}, // Matrix A row
		{-56, 10, 37, -74, 10, 106, 64, 70, -43, -61, -112, -42, -21, -3, -63, -64}, // Matrix A row
		{12, 65, -66, 6, 7, -28, -49, -111, -91, 92, -120, 67, 26, -117, 18, 20}, // Matrix A row
		{97, -75, -32, -27, 105, 68, 122, 85, -88, -95, 93, 97, 102, 48, -27, -31}, // Matrix A row
		{-116, 78, -43, -100, -51, 33, 83, 104, -19, -23, 123, -81, -95, -54, 77, -120}, // Matrix A row
		{70, 44, -45, 54, -113, 124, 0, -33, -111, 35, 28, 82, 53, -80, -19, -77} // Matrix A row
	}; // Matrix A row

  // Matrix B, elem_t, B_rows, B_cols Setup
	elem_t B[B_rows][B_cols] = { // Matrix B row
		{-38, -7}, // Matrix B row
		{10, -120}, // Matrix B row
		{29, -38}, // Matrix B row
		{-60, 118}, // Matrix B row
		{0, 34}, // Matrix B row
		{-6, 94}, // Matrix B row
		{86, 19}, // Matrix B row
		{85, 114}, // Matrix B row
		{10, -115}, // Matrix B row
		{30, 82}, // Matrix B row
		{-105, -5}, // Matrix B row
		{71, 18}, // Matrix B row
		{40, 84}, // Matrix B row
		{-77, 101}, // Matrix B row
		{-121, 45}, // Matrix B row
		{59, -79} // Matrix B row
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
  gemmini_config_st(C_cols);
  // printf("gemmini_extended3_config_ld(%u, MVIN_SCALE_IDENTITY, false, 0);\n", A_cols);
  // printf("gemmini_extended3_config_ld(%u, MVIN_SCALE_IDENTITY, false, 1);\n", B_cols);
  // printf("gemmini_config_ex(WS, NO_ACTIVATION, 0);\n");
  // printf("gemmini_config_st(%u);\n", C_cols);

  // Move in matrices for initialization
  gemmini_extended_mvin(A, A_sp_addr, A_cols, A_rows);
  gemmini_extended_mvin2(B, B_sp_addr, B_cols, B_rows);
  // printf("gemmini_extended_mvin(A, %p, %u, %u);\n", A_sp_addr, A_cols, A_rows);
  // printf("gemmini_extended_mvin(B, %p, %u, %u);\n", B_sp_addr, B_cols, B_rows);

  gemmini_extended_preload(B_sp_addr, C_sp_addr, B_cols, B_rows, C_cols, C_rows);
  gemmini_extended_compute_preloaded(A_sp_addr, GARBAGE_ADDR, A_cols, A_rows, B_cols, B_rows);
	// printf("gemmini_extended_preload(%p, 0x%x, %u, %u, %u, %u);\n", B_sp_addr, C_sp_addr, B_cols, B_rows, C_cols, C_rows);
	// printf("gemmini_extended_compute_preloaded(%p, 0x%x, %u, %u, %u, %u);\n", A_sp_addr, GARBAGE_ADDR, A_cols, A_rows, B_cols, B_rows);

  // Main microbenchmark code
  for(int i = 0; i < iterations; i++) {
    gemmini_extended_mvout(C, C_sp_addr, C_cols, C_rows);
  	// printf("gemmini_extended_mvout(C, %p, %u, %u);\n", C_sp_addr, C_cols, C_rows);
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