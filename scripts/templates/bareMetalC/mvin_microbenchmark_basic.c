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

  // Matrix A, DIM, DIM Setup
	elem_t A[DIM][DIM] = { // Matrix A row
		{-26, -34, -21, -73, -84, -79, -88, 73, 28, 43, -89, 79, 25, 91, -109, -94}, // Matrix A row
		{-26, 28, -82, -116, 123, -56, -48, 33, 123, 0, -87, -22, -111, -63, 78, -118}, // Matrix A row
		{-4, 96, 108, 123, -84, 11, 7, 101, -104, 26, -124, -116, -66, 84, 63, -17}, // Matrix A row
		{22, 2, 120, 51, 106, 74, 82, -48, -39, -56, -73, -112, -118, 126, 5, 121}, // Matrix A row
		{0, -111, 55, 11, -110, -45, -64, 54, 83, -16, 127, -54, 117, 116, -78, -126}, // Matrix A row
		{-18, 98, 61, -46, 42, 24, 72, 30, 32, 1, 8, -2, -26, -117, -27, 126}, // Matrix A row
		{96, -75, -21, 87, 100, 108, 41, 40, 104, -90, 18, 107, -1, -77, 111, -117}, // Matrix A row
		{-71, -123, 91, 86, -29, -121, -58, 67, -70, 105, -60, -20, 119, 3, 72, -72}, // Matrix A row
		{-124, -95, 110, 123, -32, -96, -85, 89, 109, -54, -96, 119, 58, 112, -59, -39}, // Matrix A row
		{59, 1, 33, 87, 111, 77, 3, -13, 47, 31, 69, 87, -11, -10, 92, -67}, // Matrix A row
		{6, 126, -63, 64, -56, -9, -106, -66, 106, 33, -8, 105, 72, -119, 68, -101}, // Matrix A row
		{38, 68, 114, -126, -115, 120, 58, 82, 85, -18, 34, 65, 28, -104, -67, -108}, // Matrix A row
		{62, 22, -117, -118, 104, 33, 113, -38, 85, -58, 17, 44, 125, -41, -97, 110}, // Matrix A row
		{44, 69, 3, -97, -116, -19, -112, 70, -22, -22, -2, -97, 76, -98, -78, 5}, // Matrix A row
		{-121, -16, 36, 106, 2, -71, 103, -72, 4, -67, 63, 104, -72, 16, 89, -72}, // Matrix A row
		{-16, 85, -16, -4, -83, -2, 110, -76, -47, -89, -121, -74, 28, 40, 6, 16} // Matrix A row
	}; // Matrix A row

  // Matrix B, DIM, DIM Setup
	elem_t B[DIM][DIM] = { // Matrix B row
		{117, 42, -111, -107, -4, -50, -24, 107, -107, -44, -82, 51, -27, -79, 122, -3}, // Matrix B row
		{99, 64, -16, -86, 120, -80, 125, 54, -59, -12, -87, -121, -17, 93, -125, 7}, // Matrix B row
		{-26, 75, 92, -58, 116, -83, 119, 75, 60, -64, 51, -6, -106, 31, 84, -66}, // Matrix B row
		{65, -9, 88, -41, -116, 115, 112, 86, -56, 1, 82, -3, 72, 61, -17, -43}, // Matrix B row
		{115, 55, 48, 94, -34, 52, -55, 25, -41, 48, 83, 77, -18, 79, -99, 108}, // Matrix B row
		{-101, -67, 76, 34, -42, 78, 58, 12, 106, -79, -64, 43, -69, -66, -37, -39}, // Matrix B row
		{-88, 48, -24, 91, -56, -48, -121, 10, -115, 15, 14, -113, 82, -36, -23, -16}, // Matrix B row
		{43, -10, 64, 1, -59, -58, 64, -52, 14, 91, 45, 14, -64, -16, -107, -55}, // Matrix B row
		{-20, 120, 95, 2, -106, -120, 119, 109, 78, 117, 86, -18, 29, 11, -48, -14}, // Matrix B row
		{56, -114, -7, -26, -34, 68, -118, -77, -23, 91, -44, 113, -92, 38, 65, 107}, // Matrix B row
		{68, 124, -79, 32, 99, -46, 51, -79, -4, 85, -38, -19, -51, -78, 52, -102}, // Matrix B row
		{-62, -53, 118, 82, -89, 125, -88, 32, -26, -5, 65, -127, 40, -97, -82, -86}, // Matrix B row
		{59, 103, -89, 13, -54, 72, 51, 90, 1, 95, 106, -120, -46, 48, 4, 119}, // Matrix B row
		{117, -126, 4, 119, -95, -112, -74, 116, 54, -31, 49, 37, -112, 70, 106, 113}, // Matrix B row
		{-62, 93, 54, -84, 78, -62, -46, 83, -119, -87, -94, -43, 14, -68, -72, -67}, // Matrix B row
		{-65, -125, -105, -83, -37, -58, 92, 123, 52, -103, -120, -91, -77, -80, -120, 98} // Matrix B row
	}; // Matrix B row

  elem_t C[DIM][DIM];

  uint32_t A_sp_addr = 0;
  uint32_t B_sp_addr = DIM;

  // Setup end
  setup_end = read_cycles();

  // Gemmini instructions start
  gemmini_fence(); 

  // Mvout start
  start = read_cycles();

  // Clear TLB
  gemmini_flush(0);

  // Config setup
  gemmini_config_ld(DIM);

  // Main microbenchmark code
  for(int i = 0; i < iterations; i++) {
    gemmini_extended_mvin(A, A_sp_addr, DIM, DIM);
    // printf("gemmini_extended_mvin(A, %p, %u, %u);\n", A_sp_addr, DIM, DIM);
  }

  gemmini_fence();
  // Gemmini instructions end

  // Mvout end
  end = read_cycles();

  setup_cycles = setup_end - setup_start;
  benchmark_cycles = end - start;

  printf("Setup cycles taken: %u\n", setup_cycles);
  printf("Cycles taken: %u\n", benchmark_cycles);

  exit(0);
}