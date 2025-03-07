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
		{47, -26, -64, -33, 57, 61, -121, 89, -93, 119, -126, -112, 3, -82, 13, 47}, // Matrix A row
		{5, 101, -40, 55, -93, -124, 80, 64, 125, -86, 34, -35, 18, -125, 79, 56}, // Matrix A row
		{-78, 124, 5, -64, -20, 103, -40, 49, -18, -32, -40, 12, -2, 28, 75, 93}, // Matrix A row
		{-46, 18, -65, 82, 66, -73, 66, 9, 66, 113, -123, 64, 98, -69, 4, -120}, // Matrix A row
		{-9, -75, 92, -38, 105, -58, -36, -51, -121, 121, -83, 19, -15, -48, 88, -91}, // Matrix A row
		{43, 65, 69, 78, -20, -25, -123, 19, 83, 11, 3, 123, 40, 73, -119, 75}, // Matrix A row
		{52, -124, -83, 31, 59, 88, -115, -104, 21, -97, -79, 114, 56, -45, -23, 51}, // Matrix A row
		{75, -116, -97, 39, 86, -69, -90, -10, 94, 93, -68, -18, 27, 81, -9, 23}, // Matrix A row
		{100, -40, -115, -16, 87, 15, -103, -110, 117, 96, -11, 25, -54, -98, -44, -53}, // Matrix A row
		{-112, 127, -48, 8, 65, 42, 113, 106, -37, 69, -69, -39, 34, 33, -62, 17}, // Matrix A row
		{38, 82, 91, 34, 94, -80, 84, 37, 108, 124, -113, -83, -99, 51, -92, -31}, // Matrix A row
		{97, 26, 57, 50, -73, -19, 103, -107, 31, -125, 40, 116, -6, 124, 36, 74}, // Matrix A row
		{-2, -42, 4, 49, 115, -53, 83, 1, 100, 122, -96, 48, 91, -104, 112, 11}, // Matrix A row
		{-34, -41, 41, -110, -73, 100, 73, 112, -104, 92, 57, -29, 20, 63, -11, -73}, // Matrix A row
		{-102, -85, 94, 12, 32, 98, -16, -108, 100, -115, 46, 104, 103, -43, -44, 118}, // Matrix A row
		{74, -58, 98, 101, 32, -43, -103, 123, -78, 61, 126, -29, -2, 11, 127, 93} // Matrix A row
	}; // Matrix A row

  // Matrix B, elem_t, DIM, DIM Setup
	elem_t B[DIM][DIM] = { // Matrix B row
		{94, 30, -124, 42, 112, -16, 115, 75, -64, -48, -28, 104, -26, 21, 114, 35}, // Matrix B row
		{-1, -4, 57, -42, 80, -88, 29, 108, -37, -30, -8, 52, 63, -98, -43, 78}, // Matrix B row
		{92, 69, -39, 30, 41, 25, -116, -104, -31, -27, -94, -116, 88, -118, 73, -47}, // Matrix B row
		{-105, -98, -79, 109, 123, 75, 21, 11, -84, -114, 127, 23, 27, -12, -20, 13}, // Matrix B row
		{-23, -88, 42, -32, -53, -99, 32, 114, -42, -98, 22, -26, 77, -83, 67, -69}, // Matrix B row
		{-69, 79, 99, -101, 88, -34, 8, -79, -42, 38, 120, 102, 106, 34, 107, -40}, // Matrix B row
		{-54, -90, 109, -69, -17, -48, 61, 118, -87, -53, 12, -19, 125, 15, -25, 71}, // Matrix B row
		{-38, 20, 93, 81, -46, -77, -11, 119, -98, 65, 49, -45, 5, 77, 32, 33}, // Matrix B row
		{14, -22, -88, 53, -11, -119, 102, 66, -122, -125, -56, 6, 89, -9, 21, 123}, // Matrix B row
		{-60, -57, 58, 124, 17, -14, 56, 63, 20, -124, 114, 3, -95, 82, 120, 34}, // Matrix B row
		{-45, -36, -83, 116, 35, 105, 94, 67, 120, -122, -92, -98, 38, 67, -71, 101}, // Matrix B row
		{30, -47, 102, 1, -96, -13, 38, 45, 67, -94, -121, -83, 63, 94, 66, -37}, // Matrix B row
		{94, 124, -101, -128, -108, -8, 35, -125, -48, -8, -74, -34, -120, 109, 7, -78}, // Matrix B row
		{-70, -117, -52, -41, -46, 78, 7, 77, -116, 79, 34, -117, 0, -101, -70, -20}, // Matrix B row
		{24, -104, 124, -52, 49, 24, 32, 93, 42, -2, 98, 79, -50, 75, -68, 59}, // Matrix B row
		{77, -59, 55, 13, 96, 56, 107, -20, -51, -59, 52, -16, 88, 105, -106, 13} // Matrix B row
	}; // Matrix B row

  elem_t C[DIM][DIM];

  uint32_t A_sp_addr = 0;
  uint32_t B_sp_addr = DIM;
  uint32_t C_sp_addr = 1 << 31 | 0 << 30;

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
  gemmini_config_ex(WS, NO_ACTIVATION, 0);
  gemmini_config_st(DIM);

  // Move in matrices for initialization
  gemmini_extended_mvin(A, A_sp_addr, DIM, DIM);
  gemmini_extended_mvin(B, B_sp_addr, DIM, DIM);
  // printf("gemmini_extended_mvin(A, %p, %u, %u);\n", A_sp_addr, DIM, DIM);
  // printf("gemmini_extended_mvin(B, %p, %u, %u);\n", B_sp_addr, DIM, DIM);

  // Store random matrix multiplication output
  gemmini_extended_preload(B_sp_addr, C_sp_addr, 16, 16, 16, 16);
  gemmini_extended_compute_preloaded(A_sp_addr, GARBAGE_ADDR, 16, 16, 16, 16);
  // printf("gemmini_extended_preload(%p, 0x%x, 16, 16, 16, 16);\n", B_sp_addr, C_sp_addr);
  // printf("gemmini_extended_compute_preloaded(%p, 0x%x, 16, 16, 16, 16);\n", A_sp_addr, GARBAGE_ADDR);

  // Main microbenchmark code
  for(int i = 0; i < iterations; i++) {
    gemmini_extended_mvout(C, C_sp_addr, DIM, DIM);
    // printf("gemmini_extended_mvout(C, 0x%x, %u, %u);\n", C_sp_addr, DIM, DIM);
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