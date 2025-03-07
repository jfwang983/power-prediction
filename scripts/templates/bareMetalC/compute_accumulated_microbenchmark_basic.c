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

  // Matrix A, elem_t, DIM, DIM Setup
	elem_t A[DIM][DIM] = { // Matrix A row
		{105, 87, 70, 95, 114, -27, 125, 106, 42, -38, -87, -64, 107, 80, 101, -3}, // Matrix A row
		{-124, 102, 35, 125, -48, 104, -128, 90, 100, -68, -99, 95, 127, -28, -27, 103}, // Matrix A row
		{-126, -18, -80, 33, -95, 6, 6, 68, -124, -73, -99, -98, -10, -43, -56, 55}, // Matrix A row
		{-84, 108, 115, -83, 52, -25, 74, 98, 102, -55, -75, -6, -60, 88, 32, 91}, // Matrix A row
		{-51, 109, -2, 43, -72, -82, -40, 119, -47, -126, 94, -16, -88, -14, 72, 29}, // Matrix A row
		{-8, -47, 114, 113, 61, -101, 46, -52, -57, -97, -18, -121, 17, -6, 0, -12}, // Matrix A row
		{-77, -5, -115, 46, -24, 65, -10, -44, 23, -126, -22, -46, 2, -29, 41, -48}, // Matrix A row
		{-82, -38, -23, -73, -68, -80, 43, 120, 109, -112, -29, -101, -25, -118, 63, 40}, // Matrix A row
		{71, 98, 124, -10, 19, 46, -1, -76, 94, 96, 53, -24, -99, 20, -96, 80}, // Matrix A row
		{112, 63, 105, 68, 28, 84, 115, -92, -31, -70, 66, -75, 24, 35, -47, -16}, // Matrix A row
		{-116, -84, -126, -87, -77, -76, -7, -101, 93, -60, -63, 72, -35, 95, 16, -41}, // Matrix A row
		{-120, -70, -80, -69, 56, -118, 115, -98, 44, 88, -122, 25, -83, -91, 127, 36}, // Matrix A row
		{-10, -96, 37, -70, -48, 82, -6, -88, 90, -77, 112, 59, -39, 11, 59, -89}, // Matrix A row
		{-73, -50, 82, 121, 17, 59, -93, -51, -120, -59, 84, -70, 27, 61, -8, -44}, // Matrix A row
		{-32, 106, -100, -117, 11, 53, 46, 78, -26, 56, -95, -80, -122, -94, -10, -85}, // Matrix A row
		{32, 51, -12, 106, 98, 118, 114, -14, 5, 26, 37, -100, 101, -66, -106, 107} // Matrix A row
	}; // Matrix A row

  // Matrix B, elem_t, DIM, DIM Setup
	elem_t B[DIM][DIM] = { // Matrix B row
		{111, 24, 4, 56, 42, 88, 49, 109, 86, -69, 35, -37, -35, -53, 125, -2}, // Matrix B row
		{48, 40, 61, 37, 94, -118, 82, 58, 92, 66, -37, -64, -3, 126, 71, -13}, // Matrix B row
		{-119, 82, 7, -104, 105, -61, 36, 29, 86, 36, -49, 28, -120, -99, 75, 31}, // Matrix B row
		{-88, -120, 72, 94, -44, 1, 94, -5, 19, -61, -107, 126, -66, 57, 39, 124}, // Matrix B row
		{1, 80, -15, -78, 69, 103, -92, -94, -29, 61, 30, -33, -121, -5, -92, 96}, // Matrix B row
		{-78, -114, -94, 17, 13, 119, 61, 25, -33, -9, -42, -32, 66, -100, -101, 112}, // Matrix B row
		{124, -105, 56, 21, -68, 29, 41, -15, -24, 120, 81, -34, -89, -50, 127, -66}, // Matrix B row
		{1, -87, -115, -75, 61, -25, 120, -30, -102, 57, 19, 13, 120, -42, -14, 6}, // Matrix B row
		{34, -46, 1, -15, 99, -21, 58, -38, 33, -20, -52, -84, 99, 94, 82, 33}, // Matrix B row
		{-12, -65, 70, -79, -72, 35, 6, 31, -35, -109, 34, -44, -58, -3, 19, 56}, // Matrix B row
		{17, 41, -108, 44, 36, 112, -95, 22, -81, -10, -69, -17, -9, -44, -51, 23}, // Matrix B row
		{-87, 66, 1, 107, -113, 116, -29, -14, -58, 19, 42, 120, 61, -114, 23, -52}, // Matrix B row
		{77, -10, 8, 4, -78, -83, 111, -66, 19, 44, -27, 74, -9, 25, -23, 2}, // Matrix B row
		{-55, -89, 1, -40, -42, -106, 101, 112, 38, 66, -47, -26, -10, -109, -18, -12}, // Matrix B row
		{63, 119, 105, -57, -3, -11, -115, -8, -58, 20, -125, -107, 107, -58, -22, 88}, // Matrix B row
		{31, 80, -74, 121, -58, 46, 84, 10, -107, 81, 86, -64, 90, 42, -51, 101} // Matrix B row
	}; // Matrix B row
  
  elem_t C[DIM][DIM];

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
  gemmini_config_ld(DIM);
  gemmini_config_ex(WS, NO_ACTIVATION, 0);

  // Move in matrices for initialization
  gemmini_extended_mvin(A, A_sp_addr, DIM, DIM);
  gemmini_extended_mvin(B, B_sp_addr, DIM, DIM);
  // printf("gemmini_extended_mvin(A, %p, %u, %u);\n", A_sp_addr, DIM, DIM);
  // printf("gemmini_extended_mvin(B, %p, %u, %u);\n", B_sp_addr, DIM, DIM);

  gemmini_extended_preload(B_sp_addr, C_sp_addr, 16, 16, 16, 16);
  gemmini_extended_compute_preloaded(A_sp_addr, GARBAGE_ADDR, 16, 16, 16, 16);
	// printf("gemmini_extended_preload(%p, 0x%x, 16, 16, 16, 16);\n", B_sp_addr, C_sp_addr);
  // printf("gemmini_extended_compute_preloaded(%p, 0x%x, 16, 16, 16, 16);\n", A_sp_addr, GARBAGE_ADDR);

  // Main microbenchmark code
  for(int i = 0; i < iterations; i++) {
    gemmini_extended_preload(GARBAGE_ADDR, C_sp_addr, 16, 16, 16, 16);
    gemmini_extended_compute_accumulated(A_sp_addr, GARBAGE_ADDR, 16, 16, 16, 16);
  	// printf("gemmini_extended_preload(0x%x, %p, 16, 16, 16, 16);\n", GARBAGE_ADDR, C_sp_addr);
  	// printf("gemmini_extended_compute_accumulated(%p, 0x%x, 16, 16, 16, 16);\n", A_sp_addr, GARBAGE_ADDR);
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