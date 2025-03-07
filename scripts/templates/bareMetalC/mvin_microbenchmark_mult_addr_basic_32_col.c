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
#define SPAD_ROWS 16384
#define A_rows 16
#define A_cols 32

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
		{27, 85, 10, 33, 90, 122, 7, 17, -39, 117, 25, -58, -120, -69, 99, 24, 46, 29, 101, 2, 66, 71, 116, -25, -101, -43, -22, -89, -125, -29, -1, 84}, // Matrix A row
		{85, -43, -68, 87, -53, 34, 5, -50, 54, 14, 80, -65, -46, 117, -15, -75, 121, -126, -19, -56, 57, 98, -108, -9, -55, -82, -116, -63, -23, -93, 55, -8}, // Matrix A row
		{6, 54, -96, 118, 50, -77, -125, 30, 24, -62, 27, 117, -63, -88, -88, 85, 71, 43, 98, -83, 35, -22, -52, -89, -31, 35, -109, -37, 98, -73, -42, -55}, // Matrix A row
		{-117, -18, 33, 29, -30, -89, -16, 1, 38, -56, -31, -54, 71, -73, 78, 3, 9, -113, -27, 39, 70, 66, 85, -72, -114, -64, 35, -93, 40, 74, -21, -51}, // Matrix A row
		{-82, -76, -106, 10, -128, 117, -76, -50, -21, 70, -86, -117, -11, -53, 38, -59, 72, -117, 9, 58, -123, -101, -74, -10, 104, 113, -7, 22, -109, -36, 120, -79}, // Matrix A row
		{-4, -102, 6, -86, 52, 27, -124, -106, 85, -87, 49, -9, 109, 77, -3, -19, -37, -103, -107, 53, 107, 35, -71, 120, 35, -16, 81, -110, 41, -8, 54, -15}, // Matrix A row
		{-9, 58, -1, 79, 127, -41, 29, -48, 23, 3, 24, 92, -12, -128, 11, 43, -58, -46, -116, -85, -95, -10, -10, 67, -50, -118, 112, 104, -7, -14, 26, 88}, // Matrix A row
		{17, -8, 4, -30, -86, 109, -20, 110, -7, 115, 86, 118, 20, -127, -105, 47, -115, -120, 43, -106, 110, -90, -62, -64, 88, 85, 69, 47, -14, -79, -77, -21}, // Matrix A row
		{7, 127, 17, -52, 33, 51, 0, -111, 120, -22, -66, -1, 105, -37, 49, -26, 10, -44, 76, -104, 10, -113, -57, -93, -112, -9, -76, 1, 21, -76, -77, 30}, // Matrix A row
		{-36, 106, -72, -114, 24, -15, -75, -92, -64, -51, 4, -47, 68, -4, -99, -59, 93, 107, -101, 102, -106, 84, -56, -14, 75, -46, -57, 98, 55, 90, 67, -64}, // Matrix A row
		{-47, 63, 120, -52, -67, -81, 69, 81, 64, 125, 125, -54, 46, -97, -90, -88, -68, -123, 62, 82, -16, 2, -108, 41, -85, -47, 61, -18, 110, 14, 83, -7}, // Matrix A row
		{98, 50, 15, -21, 96, 60, 87, -77, 72, 102, -35, 45, -39, -92, 18, -127, 64, -111, -67, -75, 104, -111, -68, 96, 3, 115, 113, 105, 31, 57, -80, -54}, // Matrix A row
		{111, 105, -81, -53, 119, -15, 65, -42, -78, 28, 91, -17, -61, 86, 7, 81, -29, 77, -41, -94, 25, -25, 28, 18, -40, -79, -99, 30, -18, 34, -62, -113}, // Matrix A row
		{-43, -35, -63, -13, 46, -67, 50, 102, 92, 28, 91, 5, 106, -25, -70, -39, 89, 73, -80, -91, -128, 78, -97, -63, -67, 84, 17, 56, -48, -79, -66, 12}, // Matrix A row
		{-96, -74, -101, 119, -121, 76, 74, 12, 114, -28, -10, 83, -73, 23, -52, -60, -77, -69, -77, -2, 72, 22, -114, -107, -83, -73, 31, 89, 27, -100, 51, -29}, // Matrix A row
		{9, -6, 84, 8, -120, -11, -12, 26, -35, 32, 94, 56, -58, -55, -70, -38, 32, 27, -52, 25, 20, -19, -67, 90, 7, 87, -32, 96, -76, 52, -102, -120} // Matrix A row
	}; // Matrix A row

  uint32_t A_sp_addr = 0;
  uint32_t next_A_sp_addr;

  // Setup end
  setup_end = read_cycles();

  // Gemmini instructions start
  gemmini_fence(); 

  // Mvout start
  start = read_cycles();

  // Clear TLB
  gemmini_flush(0);

  // Config setup
  gemmini_extended3_config_ld(A_cols, MVIN_SCALE_IDENTITY, false, 0); // A Matrix
  // printf("gemmini_extended3_config_ld(%u, MVIN_SCALE_IDENTITY, false, 0);\n", A_cols);

  // Main microbenchmark code
  for(int i = 0; i < iterations; i++) {
    gemmini_extended_mvin(A, A_sp_addr, A_cols, A_rows);
    // printf("gemmini_extended_mvin(A, %p, %u, %u);\n", A_sp_addr, A_cols, A_rows);
    next_A_sp_addr = A_sp_addr + A_cols;
    A_sp_addr = (next_A_sp_addr >= SPAD_ROWS) ? 0 : next_A_sp_addr;
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