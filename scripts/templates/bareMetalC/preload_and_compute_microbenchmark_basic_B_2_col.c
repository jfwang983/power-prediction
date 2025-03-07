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
		{3, -120, -103, 77, 106, 65, 55, -17, -29, -77, -107, 123, -40, 116, 74, -66}, // Matrix A row
		{100, 109, -91, -30, -89, -75, 51, 126, 110, 5, -47, -59, 107, 80, -68, -107}, // Matrix A row
		{87, -35, 127, -113, -45, -106, 93, 6, 57, -43, -9, 15, 81, -20, 43, 102}, // Matrix A row
		{86, 63, 56, -41, -23, -2, -7, 27, 111, -78, -104, 111, 105, -6, -35, 96}, // Matrix A row
		{-28, -47, -83, 29, 46, 109, -99, -123, 5, -105, -77, -47, -80, 35, -78, -117}, // Matrix A row
		{-62, -24, 87, 62, -23, 43, -108, 36, 74, 49, -74, -7, 31, 105, -125, 1}, // Matrix A row
		{-30, 24, 39, 4, -93, 89, -98, 57, 93, 41, 70, -18, 2, 33, -63, -38}, // Matrix A row
		{100, -2, -14, 85, -43, 14, 1, -103, -124, -7, -23, -38, 119, -41, -114, 22}, // Matrix A row
		{-119, -4, -38, 114, -99, -71, -85, -106, -21, 26, -88, -102, -122, -111, 64, 107}, // Matrix A row
		{-106, -68, -4, -30, -88, -109, -68, -115, 24, 114, -52, -57, 52, -124, 117, 92}, // Matrix A row
		{-54, 64, 93, 60, -52, -9, 51, 107, -21, 125, -56, -111, -84, 52, -30, 29}, // Matrix A row
		{-94, -85, -41, -70, 84, 116, -17, -58, -67, 123, 118, 97, 122, -24, 82, -45}, // Matrix A row
		{113, -36, -40, -117, 117, 62, -126, 38, -126, 111, 106, -88, 50, 64, -68, 3}, // Matrix A row
		{90, 100, -84, 102, 25, -3, 100, -121, -97, 101, 6, 41, 16, -32, 70, 54}, // Matrix A row
		{82, -23, 88, -3, 122, -19, 32, 20, -96, 101, -76, -33, 64, -17, -94, 114}, // Matrix A row
		{113, -19, 72, -7, 26, -41, -95, -95, 21, 92, -61, -109, 80, -21, -108, 50} // Matrix A row
	}; // Matrix A row

  // Matrix B, elem_t, B_rows, B_cols Setup
	elem_t B[B_rows][B_cols] = { // Matrix B row
		{63, -127}, // Matrix B row
		{99, 40}, // Matrix B row
		{-4, 78}, // Matrix B row
		{-6, -67}, // Matrix B row
		{-116, -31}, // Matrix B row
		{-108, 81}, // Matrix B row
		{94, -62}, // Matrix B row
		{-59, 44}, // Matrix B row
		{2, -125}, // Matrix B row
		{-111, -85}, // Matrix B row
		{17, 104}, // Matrix B row
		{29, -82}, // Matrix B row
		{-110, 76}, // Matrix B row
		{-20, -21}, // Matrix B row
		{-82, 95}, // Matrix B row
		{117, -3} // Matrix B row
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