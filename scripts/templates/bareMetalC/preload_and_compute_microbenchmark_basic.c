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
		{124, 100, -32, -22, -55, -85, -3, 122, -119, 16, -109, -53, 6, 71, -2, -19}, // Matrix A row
		{111, 36, -31, 109, 124, 56, -60, 1, 51, 120, -119, 68, -14, 106, 34, 2}, // Matrix A row
		{56, -1, 74, -24, 21, 17, -109, -10, 36, 101, 4, 79, -15, -103, 103, -2}, // Matrix A row
		{55, -51, 82, 10, 48, -2, -86, 105, 88, -68, 11, 16, 118, 120, 32, 25}, // Matrix A row
		{-16, 96, 119, 124, -16, -20, 119, 73, 16, 82, 4, 80, -100, -62, 2, 29}, // Matrix A row
		{73, 62, -29, 19, -3, -105, -91, -120, 69, 67, -62, 68, 100, 49, -112, 49}, // Matrix A row
		{-24, 25, -112, 45, -5, -128, 23, -27, 103, 107, 122, -14, -103, -63, 18, 52}, // Matrix A row
		{-2, 77, 23, -38, 42, 106, 43, 108, -46, 59, 22, 35, 6, 67, -103, 76}, // Matrix A row
		{17, -60, -108, -62, -97, 102, 37, -4, 73, 27, -19, -123, -42, 41, 0, 78}, // Matrix A row
		{29, -118, 5, -20, -54, -125, -29, 38, -61, -109, 0, 46, 27, 47, -25, 33}, // Matrix A row
		{105, -22, -79, 33, 11, 60, -15, -68, -115, -57, -79, 110, 39, 62, -79, -103}, // Matrix A row
		{64, -117, 84, 54, 79, -19, -70, 126, -6, 52, 70, 2, 53, -47, -4, -1}, // Matrix A row
		{-58, -52, 72, -123, -49, -96, -105, 5, 7, -56, 19, 81, -126, -110, 86, 97}, // Matrix A row
		{21, -11, -38, -128, 123, 24, -127, -40, 100, -48, -29, 85, -46, 108, -89, -94}, // Matrix A row
		{22, -32, -98, -23, 33, -125, -79, -102, 41, 101, 42, -4, 28, 55, 65, 119}, // Matrix A row
		{62, 115, -33, 23, -35, -18, 36, 116, 86, 69, -88, 40, -93, 51, 98, 121} // Matrix A row
	}; // Matrix A row

  // Matrix B, elem_t, DIM, DIM Setup
	elem_t B[DIM][DIM] = { // Matrix B row
		{-61, 124, 103, -47, 109, 117, -99, -86, -10, -72, -26, -24, -26, -28, -122, 85}, // Matrix B row
		{47, 73, 83, -35, -15, -109, 38, 64, 24, 20, -35, 49, 106, -36, 127, -53}, // Matrix B row
		{-56, 97, -49, 72, -25, 64, 52, 59, -101, 91, 124, -14, -23, -108, -77, 97}, // Matrix B row
		{-41, -46, 43, 8, 95, -40, 38, -48, 110, -94, -81, -42, -40, -28, -24, -52}, // Matrix B row
		{14, -2, 109, -16, -38, 43, 28, 42, -96, -23, 84, 33, 111, 111, 29, 112}, // Matrix B row
		{-85, -122, -77, 117, 125, -93, 92, 90, 8, -91, 53, -57, 124, -120, -73, -22}, // Matrix B row
		{67, 82, 53, 42, -110, -10, -7, -99, -28, 69, 103, -29, 68, 118, -80, -110}, // Matrix B row
		{65, -105, 102, -122, 54, 122, -113, 34, 48, 77, 52, -2, -73, 33, 16, -33}, // Matrix B row
		{-119, 120, -33, -22, 114, -85, -30, -84, -18, -104, -44, 25, 19, 91, -18, 88}, // Matrix B row
		{-40, 119, 20, -16, -4, -38, -101, 98, -23, -59, -42, 37, -44, -32, -102, -50}, // Matrix B row
		{25, -62, 68, -24, -125, 83, -111, -92, 51, 91, -62, -92, -110, -50, -49, 7}, // Matrix B row
		{47, -92, -116, 45, -6, 65, 113, -45, 90, 88, 4, 90, 82, 104, -38, -48}, // Matrix B row
		{-103, 1, 69, 50, 23, -106, -45, 123, -13, -39, -71, -46, 33, 109, -26, 111}, // Matrix B row
		{109, -74, 79, 72, 27, -61, 124, -68, 125, 67, 98, -82, -109, -10, 114, -35}, // Matrix B row
		{-69, 103, 98, -62, 8, -69, -19, -112, 76, 96, -9, -82, -34, -72, -90, -119}, // Matrix B row
		{98, 62, 28, 11, -81, -20, 56, -92, -65, -4, -117, -38, -37, 74, 104, -66} // Matrix B row
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

  // Main microbenchmark code
  for(int i = 0; i < iterations; i++) {
    gemmini_extended_preload(B_sp_addr, C_sp_addr, 16, 16, 16, 16);
    gemmini_extended_compute_preloaded(A_sp_addr, GARBAGE_ADDR, 16, 16, 16, 16);
	  // printf("gemmini_extended_preload(%p, 0x%x, 16, 16, 16, 16);\n", B_sp_addr, C_sp_addr);
  	// printf("gemmini_extended_compute_preloaded(%p, 0x%x, 16, 16, 16, 16);\n", A_sp_addr, GARBAGE_ADDR);
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