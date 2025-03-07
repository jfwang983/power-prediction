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
		{18, -121, -84, -82, 53, 47, 18, -4, -101, -114, -48, -81, 122, -4, 32, -59}, // Matrix A row
		{71, 84, 120, 120, 56, 113, 117, 74, 97, 42, 123, -88, -14, -13, 38, -47}, // Matrix A row
		{6, -73, 94, 39, -30, 110, 124, -127, -11, -85, -99, -66, 96, -19, 110, -58}, // Matrix A row
		{127, -89, 40, -116, 15, 92, 70, -59, -13, -69, 2, 19, 76, -101, -55, -69}, // Matrix A row
		{41, 11, 89, 40, 22, 115, -66, -45, -117, -49, -105, -103, -50, 118, -83, 71}, // Matrix A row
		{108, 36, -24, 12, -1, -94, -14, 13, 101, 46, 22, 77, 72, 67, 82, -8}, // Matrix A row
		{-73, -86, 93, 83, 21, -119, 39, 43, -51, -40, -32, -116, -75, 12, 58, -100}, // Matrix A row
		{117, -70, -62, -44, 89, 93, 68, 115, 18, -23, -8, 72, -34, -92, 32, 122}, // Matrix A row
		{115, -109, 32, -71, 17, -51, -97, -86, -120, -69, 120, -30, 0, 81, 91, 127}, // Matrix A row
		{46, 60, 42, 49, 52, -117, -21, 16, 92, -29, -54, -81, -65, 84, 97, -82}, // Matrix A row
		{36, 77, 45, 97, -106, -82, 109, -65, 115, -32, -66, 116, -44, -109, 44, -121}, // Matrix A row
		{6, 40, 67, -70, 27, -10, 70, 86, 126, -85, -14, 71, 4, -109, 32, 96}, // Matrix A row
		{-75, -56, -126, -128, -9, 54, -112, 124, -120, 41, -85, 63, 83, -119, 113, 31}, // Matrix A row
		{77, -104, 43, 126, 89, -51, -104, 104, -45, 33, -30, 6, 25, 94, 96, -64}, // Matrix A row
		{73, 7, -99, 64, -116, -2, -49, -68, -116, -14, -66, -51, -108, 37, -92, 88}, // Matrix A row
		{63, 41, -7, -112, -18, -55, -83, -102, 85, -5, -10, -58, -101, 25, 44, -43} // Matrix A row
	}; // Matrix A row

  // Matrix B, elem_t, B_rows, B_cols Setup
	elem_t B[B_rows][B_cols] = { // Matrix B row
		{40, -71, -84, -42, 82, 6, -74, 127, -90, -104, 101, 93, 8, 100, -76, -29}, // Matrix B row
		{-79, -71, -4, -11, -38, -30, -121, -89, 21, 73, 97, -24, -104, -18, -59, 58}, // Matrix B row
		{-76, -124, 81, 45, 32, 21, 55, -104, 81, 98, -78, -72, 103, -123, -78, -47}, // Matrix B row
		{38, -53, -37, 89, -75, -36, -91, 117, -25, 100, 55, -32, 82, 89, -40, -29}, // Matrix B row
		{93, 122, 45, 64, -42, -85, -103, 83, -96, 61, 77, -51, -91, 47, 93, -79}, // Matrix B row
		{-92, 21, 109, -26, 101, -14, 5, -98, 110, -38, -4, 113, 126, 16, 82, 113}, // Matrix B row
		{-114, 76, 126, 80, -59, -120, -27, -86, -121, -65, 15, -82, 80, 83, 69, 110}, // Matrix B row
		{77, -82, 111, -23, 14, -39, -8, -15, -107, -22, 114, -89, -1, -48, 49, -91}, // Matrix B row
		{-81, -83, -42, -56, 98, 87, -38, -71, 45, -17, 108, -62, 71, -127, -63, -70}, // Matrix B row
		{85, -58, 96, -77, -76, 84, -17, -91, 78, 120, 97, -99, 13, -88, -118, -74}, // Matrix B row
		{99, -43, -34, -37, 54, -23, -17, 103, 51, -70, -91, -127, 65, 114, 53, 53}, // Matrix B row
		{-49, 58, 71, 68, -4, 3, 123, -74, -66, 21, 33, 51, -39, 43, -110, 99}, // Matrix B row
		{7, -54, -108, -7, -45, -14, 55, 112, -26, 124, -4, -48, 54, 115, -103, 66}, // Matrix B row
		{21, 126, -15, 119, -99, -6, -50, 89, 114, 86, -25, -23, 122, 16, 85, 63}, // Matrix B row
		{-41, -66, -122, 112, -76, -50, -14, -115, 88, 0, 43, 101, -48, 71, -88, -68}, // Matrix B row
		{3, -29, -71, -82, -92, 122, 8, -58, 41, 98, 7, -92, -81, 92, -2, -63} // Matrix B row
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