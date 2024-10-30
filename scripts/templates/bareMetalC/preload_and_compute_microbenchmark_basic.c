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
		{52, -127, 2, 81, -91, -125, -71, -123, 117, 126, -15, -114, -36, 56, -18, 3}, // Matrix A row
		{77, 80, 121, 86, 55, -85, 61, -82, -29, 89, 20, 94, 77, 111, 14, 55}, // Matrix A row
		{32, -53, 49, -66, 87, 61, -89, 14, -101, 21, 34, 102, 67, -74, -93, -99}, // Matrix A row
		{-39, 88, 98, -47, -23, -26, -28, 111, 50, -31, -105, 67, -23, -93, 10, 76}, // Matrix A row
		{3, -45, 79, -65, 34, 99, -125, 9, -37, 14, -51, -38, -27, 40, 127, 105}, // Matrix A row
		{-39, -64, 9, 5, 76, 75, -34, -56, -21, 65, 43, 86, 58, 69, 86, -106}, // Matrix A row
		{121, 122, 112, 48, 39, 105, -86, 18, 124, 115, 52, 3, 112, -55, 37, 124}, // Matrix A row
		{-8, 19, 115, -95, 48, -80, -115, 42, -117, -15, 56, -16, -12, -41, -64, -90}, // Matrix A row
		{116, -114, -76, 120, -84, 86, -30, -98, -7, -85, -55, -94, 93, -57, 7, -5}, // Matrix A row
		{80, -72, 108, -43, 124, 28, -38, 33, 20, -111, 110, -42, -101, -74, -84, 26}, // Matrix A row
		{-36, -68, -124, -117, 111, -51, 102, 81, 44, -68, 0, -8, -113, 115, 21, -71}, // Matrix A row
		{-14, 48, 89, 38, -43, -66, 107, -28, -81, 23, -43, -41, -17, -106, 57, 52}, // Matrix A row
		{99, 18, -44, 71, -61, -116, -87, 111, -49, -17, 102, -74, 60, 68, -70, 96}, // Matrix A row
		{104, -5, 91, 58, 28, 105, -94, 57, -86, -81, 125, -121, 7, 112, 28, -40}, // Matrix A row
		{112, -35, -76, 112, -105, 53, -128, 79, -23, -106, 121, 72, 83, -99, -24, 74}, // Matrix A row
		{101, 108, 125, 69, -89, 98, -8, -3, -67, 44, 124, -54, -37, -115, -124, 50} // Matrix A row
	}; // Matrix A row

  // Matrix B, DIM, DIM Setup
	elem_t B[DIM][DIM] = { // Matrix B row
		{-77, -26, -13, -36, 3, 5, -14, 94, 37, -98, -18, -51, 11, -43, 127, 61}, // Matrix B row
		{97, -83, 24, 6, 22, 36, 102, -53, -54, -16, -120, 3, -124, 82, 60, 110}, // Matrix B row
		{-19, -62, -109, 35, 10, 58, -64, -22, -7, 75, 72, 55, 56, -12, 7, -42}, // Matrix B row
		{58, -98, 93, -126, 3, 57, -11, -43, -50, 70, -33, 109, 39, -2, 64, -62}, // Matrix B row
		{95, -113, -55, 45, 121, 83, 14, -78, 56, 47, -37, -8, -90, -86, -10, 48}, // Matrix B row
		{58, -90, 35, 59, -71, 10, -60, -102, -123, 108, -97, 77, -120, 79, -10, 116}, // Matrix B row
		{56, -71, 16, 63, 35, 113, -103, -29, 108, -126, -37, -56, -20, -116, -69, 60}, // Matrix B row
		{27, -7, 70, 112, 127, 95, 84, 63, -94, -106, 36, -122, 112, 37, -43, 64}, // Matrix B row
		{102, -115, 37, 37, 78, -18, -75, -91, -70, 72, 120, 93, 75, 96, 52, -3}, // Matrix B row
		{55, -93, -16, -79, -28, -24, 100, 113, -123, 116, 51, -115, 111, -73, -7, 39}, // Matrix B row
		{106, -94, 116, -12, 126, -89, 70, 6, -54, -72, -38, -13, -82, -106, 102, 85}, // Matrix B row
		{44, -74, -7, -11, -101, 87, 79, 36, -42, -62, 122, -57, 126, 114, 85, -82}, // Matrix B row
		{59, -70, -64, -59, 63, -1, 112, 15, -58, 81, -34, -65, 124, 21, -36, 70}, // Matrix B row
		{-8, -65, -11, 35, 9, 123, -107, -26, -91, -11, -86, -91, 21, -126, 17, -59}, // Matrix B row
		{-98, 20, 69, -46, 76, -37, -26, 127, -47, -27, 26, -16, -52, 103, 18, -23}, // Matrix B row
		{90, -74, 52, 12, -121, -80, 21, -105, -103, -49, -117, -92, -26, -111, 88, 43} // Matrix B row
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