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
		{-38, 110, 93, -38, -102, -45, 109, 83, 106, 119, -111, -96, 61, -98, -107, -14}, // Matrix A row
		{-110, -46, 24, -26, -22, -38, 19, 120, 70, -116, -18, -15, -119, 84, 101, 47}, // Matrix A row
		{1, -23, 13, 108, 93, -118, -54, -106, -22, -35, 41, 95, 90, 20, -93, 0}, // Matrix A row
		{44, -107, -82, 40, 92, 85, 98, -2, 104, -28, -89, -36, -113, 1, 92, 25}, // Matrix A row
		{-26, 20, 85, 52, -118, -60, 90, -124, 101, 125, 39, 113, 100, 3, 48, -102}, // Matrix A row
		{-83, 35, -92, -97, 126, 29, 52, 13, 7, -116, -116, 90, -4, 108, -48, -2}, // Matrix A row
		{114, -26, -53, 28, 65, -91, 72, -95, 55, -78, -14, -60, 55, -99, 30, 60}, // Matrix A row
		{78, 123, -27, -5, 105, 98, 106, -124, -39, 107, -80, 12, -83, 61, -85, 50}, // Matrix A row
		{-70, 68, -85, -79, 12, -6, 39, 68, -128, 14, 81, 109, 115, -111, 73, -109}, // Matrix A row
		{-18, -56, 51, -80, 60, -118, -63, 49, 1, 42, 65, -46, 125, -46, 81, 40}, // Matrix A row
		{-91, 39, -101, -126, -56, -47, 52, 68, 78, 17, 114, 22, -101, -58, 115, 35}, // Matrix A row
		{-101, 112, -7, -35, -6, 34, -40, -100, 107, -42, 17, 123, -103, -97, 60, -112}, // Matrix A row
		{-11, 88, -63, 66, -22, 64, 15, 21, -70, 76, -50, -32, 42, -89, -61, 125}, // Matrix A row
		{-20, -14, 28, -7, 46, -33, 33, -61, 67, -60, 16, -61, -58, 36, -99, 7}, // Matrix A row
		{-80, 27, 36, -23, 35, -107, -125, -115, -8, 101, -48, -74, 114, -18, 73, -81}, // Matrix A row
		{-62, 88, -116, -91, 113, 89, -126, 14, 24, 71, 85, -88, 80, 31, 9, 31} // Matrix A row
	}; // Matrix A row

  // Matrix B, DIM, DIM Setup
	elem_t B[DIM][DIM] = { // Matrix B row
		{-26, -30, 90, 118, -26, -120, -61, -20, -15, -113, -53, -95, 70, -103, -71, 10}, // Matrix B row
		{13, -90, 52, -5, -110, -72, -83, -9, 83, -5, -20, 109, -113, 110, 63, -109}, // Matrix B row
		{28, -53, 83, 4, -119, 17, -123, -55, 111, -76, -48, 47, 121, -47, -16, 15}, // Matrix B row
		{-63, 1, 60, -118, -103, 110, 9, 67, 30, 126, -6, 70, -124, 99, 84, 52}, // Matrix B row
		{-86, -81, 58, -88, -60, -90, 57, 1, 61, -97, 56, 66, 71, 86, -104, -74}, // Matrix B row
		{95, -16, -126, 35, 52, 7, -8, -93, 120, 121, 78, -44, 22, 27, -13, -62}, // Matrix B row
		{-96, 86, 103, 19, 122, 39, 85, 42, 106, -38, -23, -30, -45, 23, -74, 17}, // Matrix B row
		{-65, -118, 40, -44, 52, -37, 8, -4, 20, -43, 83, 55, -28, -26, 62, -101}, // Matrix B row
		{-76, 29, 84, -108, 126, -126, 91, -60, 4, 28, 27, -49, -16, -72, 68, 67}, // Matrix B row
		{-112, 70, -61, 20, -104, 66, 85, -71, -24, 92, -73, -5, -64, -85, 78, -89}, // Matrix B row
		{74, -115, -46, -26, -32, -94, 18, 87, 72, 53, -100, -66, -57, 100, -100, 54}, // Matrix B row
		{-114, 111, -29, 17, 127, -62, 92, -119, -14, -45, 47, 56, -97, 120, -1, -103}, // Matrix B row
		{41, -107, 52, 115, 77, -95, 125, 72, -13, -53, 66, -63, -3, 58, 75, 7}, // Matrix B row
		{75, 21, 62, -21, -86, -66, 93, -21, -105, 13, -35, 55, -43, 5, 62, -52}, // Matrix B row
		{14, -121, 45, -96, -5, 18, 95, 41, 66, 24, 60, 57, 36, -21, 33, -67}, // Matrix B row
		{-1, 71, 47, -56, 32, -60, -74, 74, 52, 33, -65, 116, -57, -75, -76, 54} // Matrix B row
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