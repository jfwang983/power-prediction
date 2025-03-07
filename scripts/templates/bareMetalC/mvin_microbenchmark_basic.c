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
		{-65, 15, 10, -41, 10, -53, -46, 93, 73, -108, -57, 78, 76, -112, -117, 62}, // Matrix A row
		{-55, -86, -13, -121, -3, -84, -49, 18, 75, 86, -49, 29, -117, 27, 46, -99}, // Matrix A row
		{45, 34, 98, 80, -32, -31, 110, -52, -46, -21, 93, 20, 115, 48, -39, 8}, // Matrix A row
		{55, -15, 119, -70, -44, -49, 7, 10, -13, 68, -61, -61, -12, 61, -53, -25}, // Matrix A row
		{-101, 7, -69, -76, -11, -27, 71, -43, -15, 93, -117, -43, 22, 69, -34, -112}, // Matrix A row
		{-49, -46, 77, -51, -116, 14, -104, -16, 81, -47, -32, 38, -108, 40, -96, 127}, // Matrix A row
		{5, 39, 89, 1, 49, -41, 88, -112, 53, 83, -51, -61, -26, 79, 21, -122}, // Matrix A row
		{-74, 111, 70, -82, 95, 108, 124, 46, -77, -76, -1, 21, 119, -63, 30, 15}, // Matrix A row
		{-32, -115, 72, -27, 66, 89, 44, 76, 118, 81, -127, 111, 72, 34, -125, 98}, // Matrix A row
		{27, 69, 119, -76, 107, -51, -95, -9, 15, -21, -92, 68, -41, 98, -77, -126}, // Matrix A row
		{108, -104, 52, -106, -42, 78, 95, 30, -63, 15, -13, 16, -66, -26, -10, 12}, // Matrix A row
		{76, -21, -48, -37, 56, 5, 56, -5, -126, -42, -82, 69, -25, -60, -47, -8}, // Matrix A row
		{-43, 23, 10, -53, -77, -4, 103, -35, 50, 13, -64, 34, 64, 12, -27, -123}, // Matrix A row
		{-65, -96, -90, 60, -62, 124, -104, -111, 57, -64, -49, -113, 6, -124, -9, -42}, // Matrix A row
		{96, 51, 67, 123, -87, 87, -58, 59, -111, 23, -112, 84, -103, 9, 80, 18}, // Matrix A row
		{106, 123, 21, 40, 77, 64, 43, 44, 24, 89, -112, -69, 37, -107, 10, -44} // Matrix A row
	}; // Matrix A row

  // Matrix B, elem_t, DIM, DIM Setup
	elem_t B[DIM][DIM] = { // Matrix B row
		{114, 44, 34, -12, 122, -128, 91, -95, 119, 28, -84, -98, -7, 41, 87, 116}, // Matrix B row
		{6, -9, 49, -91, 11, 94, -36, 84, 99, 42, -48, 59, 56, -36, -6, -107}, // Matrix B row
		{-120, 41, -69, -113, 107, 48, -83, -76, -105, 101, -40, -110, -47, 56, 15, -26}, // Matrix B row
		{103, 125, 17, -70, -109, 2, -32, -33, 70, 53, -112, 92, 18, -78, -58, -101}, // Matrix B row
		{-95, 71, 99, 117, -1, 111, -57, 33, -54, -36, 57, -7, -119, -41, -107, -77}, // Matrix B row
		{-16, -97, 121, -21, 60, 97, 20, 84, 70, -76, -25, -63, 115, 85, -35, -29}, // Matrix B row
		{18, 20, -50, 108, 2, -123, -119, 47, 91, -24, -83, -26, -71, 39, 0, -42}, // Matrix B row
		{21, -119, 112, 124, 63, -29, 7, -89, -90, -46, 107, 86, -110, -123, 17, 125}, // Matrix B row
		{79, -110, 90, -6, -108, 108, 39, -83, -82, 82, -91, 33, -73, 11, -17, -59}, // Matrix B row
		{-74, 119, 29, -11, -39, -55, -100, -48, 5, -101, -123, 113, -91, -50, -30, -24}, // Matrix B row
		{-127, 36, 33, -115, 35, -51, -60, 19, 31, -88, 114, -40, -25, -41, -59, 81}, // Matrix B row
		{-4, -3, 91, -6, 50, -61, 1, -34, 110, -50, 109, 30, -38, -78, 34, -118}, // Matrix B row
		{-56, -43, -12, -91, -78, 33, 58, -22, -105, 50, -82, -119, 80, 102, -41, -26}, // Matrix B row
		{-98, 76, -125, 72, 82, -88, -100, 98, -69, -50, 6, -53, -119, -9, 47, -99}, // Matrix B row
		{117, 105, -65, -67, 21, 27, -33, 12, -10, -89, 3, 102, 94, 67, -88, -63}, // Matrix B row
		{-66, -46, 63, 84, 91, 102, 102, 43, -48, -84, -48, 84, -112, 125, -94, -128} // Matrix B row
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