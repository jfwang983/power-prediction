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
#define B_cols 4
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
		{-116, -77, 88, 0, 108, 110, 40, -80, -28, -83, -55, -20, 89, 92, -51, 40}, // Matrix A row
		{-60, -31, -99, -27, 16, 27, -52, -86, 87, 48, 13, 57, -16, 67, 65, -125}, // Matrix A row
		{76, -48, -109, 62, -10, -30, 89, 119, 88, 96, 41, 118, -64, 77, 93, 107}, // Matrix A row
		{108, -34, 45, 29, -109, -82, 122, 9, 26, -113, -100, 48, 10, 115, 94, 1}, // Matrix A row
		{-14, 14, 50, -23, 69, 19, 101, 58, 75, -121, -34, 20, 57, 26, 24, 51}, // Matrix A row
		{118, 75, 111, -16, 98, -104, 83, 126, -16, 67, 15, 12, -90, 105, -32, -88}, // Matrix A row
		{17, -116, 108, -84, 8, 19, -69, -107, -99, 23, -13, 88, 115, -50, -94, -44}, // Matrix A row
		{-90, 52, -41, 24, -21, -11, -9, -49, -115, -89, -16, -125, -27, 63, 42, 15}, // Matrix A row
		{4, -19, -123, -65, -27, 27, -81, 41, 20, 77, -3, 45, -57, -94, -47, -70}, // Matrix A row
		{-101, 111, -44, 126, 12, 39, -114, -61, -94, 121, 116, -76, -63, -34, -22, 85}, // Matrix A row
		{82, 33, -55, 68, -126, 46, -99, -53, 93, 40, 3, 16, 121, 48, 28, -49}, // Matrix A row
		{-111, 107, -90, -87, -20, -113, 102, -9, 66, 52, 125, -69, -21, 14, 16, -115}, // Matrix A row
		{107, -6, -32, 41, 105, -79, 97, -117, 72, 105, -44, -127, -82, -3, -120, -24}, // Matrix A row
		{97, 121, 104, 61, 7, -70, -52, 72, -59, -16, 82, 121, -127, -32, -124, -43}, // Matrix A row
		{-108, 115, 120, 62, 36, 78, 72, 35, 2, 112, -59, 47, -41, -90, 112, -94}, // Matrix A row
		{-127, 116, 108, 88, -122, -116, -84, 73, -46, -16, 125, -11, 6, -67, -102, -30} // Matrix A row
	}; // Matrix A row

  // Matrix B, elem_t, B_rows, B_cols Setup
	elem_t B[B_rows][B_cols] = { // Matrix B row
		{42, 82, -65, -79}, // Matrix B row
		{45, -6, -42, -26}, // Matrix B row
		{115, -78, 43, 67}, // Matrix B row
		{-120, -90, 53, 22}, // Matrix B row
		{-91, 22, 10, -28}, // Matrix B row
		{-16, 22, -87, 114}, // Matrix B row
		{88, 122, 33, -14}, // Matrix B row
		{5, -7, 56, 109}, // Matrix B row
		{-106, -26, -83, -123}, // Matrix B row
		{-111, -70, 30, -101}, // Matrix B row
		{-114, 4, 19, -12}, // Matrix B row
		{-62, -53, 1, 49}, // Matrix B row
		{-19, -80, -104, 65}, // Matrix B row
		{-102, 45, 66, 15}, // Matrix B row
		{-67, 14, 11, -60}, // Matrix B row
		{68, 55, -42, 38} // Matrix B row
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