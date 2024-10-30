// See LICENSE for license details.

#include <stdint.h>
#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>
#ifndef BAREMETAL
#include <sys/mman.h>
#endif
#include "matmul_funcs.h"

#define PADDING         0
#define STRIDE          1

#define BATCH_SIZE      1
#define INPUT_CHANNELS  1
#define INPUT_ROW_DIM   16
#define INPUT_COL_DIM   16
#define OUTPUT_CHANNELS 1
#define KERNEL_DIM      1
#define OUTPUT_ROW_DIM  (INPUT_ROW_DIM + 2 * PADDING - KERNEL_DIM)/STRIDE + 1
#define OUTPUT_COL_DIM  (INPUT_COL_DIM + 2 * PADDING - KERNEL_DIM)/STRIDE + 1

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

  // Matrix input, BATCH_SIZE, INPUT_CHANNELS, INPUT_ROW_DIM, INPUT_COL_DIM Setup
	elem_t input[BATCH_SIZE][INPUT_CHANNELS][INPUT_ROW_DIM][INPUT_COL_DIM] = { // Matrix input row
		{ // Matrix input row
			{ // Matrix input row
				{-34, 7, 1, -73, 38, -91, -76, -115, -94, 34, 51, 55, -31, 24, 52, 23}, // Matrix input row
				{-122, -103, 102, 0, -110, 76, -128, -48, -32, -48, 90, -128, 99, 49, -109, -3}, // Matrix input row
				{125, -13, -31, 7, -62, -75, 91, -34, 101, 62, 73, -74, 110, 102, -36, -99}, // Matrix input row
				{-13, 11, -46, 99, 2, 62, 39, -56, 31, -96, -30, 21, 65, -8, 11, 76}, // Matrix input row
				{-118, 35, -115, -73, 118, 20, -121, -43, -15, 93, 119, -107, 38, -121, 126, -110}, // Matrix input row
				{-87, 78, 42, -66, -126, 105, -28, 123, 18, 106, -81, 69, 94, 48, 29, 121}, // Matrix input row
				{48, -30, -48, -68, 95, 90, -106, 103, 120, 114, 75, -6, 89, 21, 21, -107}, // Matrix input row
				{-97, -128, -40, 110, -113, -85, 36, -37, -36, 84, 13, 97, 84, -4, -85, 13}, // Matrix input row
				{-27, 13, 117, 66, -112, 2, 98, 7, 71, 4, 70, -53, -31, -114, -1, 69}, // Matrix input row
				{99, -2, 53, 22, -88, 79, -13, -115, 78, 97, 73, 107, 79, -99, 19, 91}, // Matrix input row
				{-6, -63, -38, 57, -29, 82, 21, -111, 24, -14, -57, 114, -96, -6, 32, -23}, // Matrix input row
				{-75, 84, 120, -74, 38, -67, -97, -124, 105, 90, -108, 15, 33, 86, -48, 55}, // Matrix input row
				{-14, -2, 4, 31, 6, -65, 9, 6, 4, 20, -123, 65, 77, -104, -76, 104}, // Matrix input row
				{-58, 34, -37, -40, -51, -29, -112, -22, 38, -117, 119, 39, -127, 30, 65, -109}, // Matrix input row
				{7, -115, -101, 10, 74, -98, -48, 30, -42, -2, 97, 65, 65, 109, -100, -57}, // Matrix input row
				{122, -38, -88, -94, 106, 104, 64, -66, 86, -105, -102, 74, 73, -27, 95, -24} // Matrix input row
			} // Matrix input row
		} // Matrix input row
	}; // Matrix input row

  // Matrix weights, OUTPUT_CHANNELS, INPUT_CHANNELS, KERNEL_DIM, KERNEL_DIM Setup
	elem_t weights[OUTPUT_CHANNELS][INPUT_CHANNELS][KERNEL_DIM][KERNEL_DIM] = { // Matrix weights row
		{ // Matrix weights row
			{ // Matrix weights row
				{-126} // Matrix weights row
			} // Matrix weights row
		} // Matrix weights row
	}; // Matrix weights row

  elem_t output[BATCH_SIZE][OUTPUT_CHANNELS][OUTPUT_ROW_DIM][OUTPUT_COL_DIM];

  // // Runtime input + weights Setup
  // for(int i = 0; i < INPUT_ROW_DIM; i++) {
  //   for(int j = 0; j < INPUT_COL_DIM; j++) {
  //     input[0][0][i][j] = rand() % 256 - 128;
  //   }
  // }
  // for(int i = 0; i < KERNEL_DIM; i++) {
  //   for(int j = 0; j < KERNEL_DIM; j++) {
  //     weights[0][0][i][j] = rand() % 256 - 128;
  //   } 
  // }

  // Setup end
  setup_end = read_cycles();

  // Gemmini instructions start
  gemmini_fence();

  // Tiled matmul start
  start = read_cycles();

  // Clear TLB
  gemmini_flush(0);

  // Main benchmark code
  sp_tiled_conv_auto(BATCH_SIZE, INPUT_ROW_DIM, INPUT_COL_DIM, INPUT_CHANNELS,
    OUTPUT_CHANNELS, OUTPUT_ROW_DIM, OUTPUT_COL_DIM, STRIDE, PADDING, KERNEL_DIM,
    input, weights, output, NULL, NO_ACTIVATION);

  gemmini_fence();
  // Gemmini instructions end

  // Tiled matmul end
  end = read_cycles();
  
  setup_cycles = setup_end - setup_start;
  benchmark_cycles = end - start;
  printf("Setup cycles taken: %u\n", setup_cycles);
  printf("Cycles taken: %u\n", benchmark_cycles);

  exit(0);
}