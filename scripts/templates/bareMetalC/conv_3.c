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

#define BATCH_SIZE      32
#define INPUT_CHANNELS  1
#define INPUT_ROW_DIM   4
#define INPUT_COL_DIM   4
#define OUTPUT_CHANNELS 1
#define KERNEL_DIM      2
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
				{-54, 23, 71, -103}, // Matrix input row
				{-51, -84, -36, -72}, // Matrix input row
				{60, 71, -100, 77}, // Matrix input row
				{-107, -52, 114, -101} // Matrix input row
			} // Matrix input row
		},  // Matrix input row
		{ // Matrix input row
			{ // Matrix input row
				{54, -108, 95, -61}, // Matrix input row
				{109, -62, -10, -56}, // Matrix input row
				{-18, -28, -115, 90}, // Matrix input row
				{96, -115, -42, -73} // Matrix input row
			} // Matrix input row
		},  // Matrix input row
		{ // Matrix input row
			{ // Matrix input row
				{-86, -36, -80, 124}, // Matrix input row
				{9, 125, -123, 45}, // Matrix input row
				{41, -77, -2, 14}, // Matrix input row
				{-3, 122, 28, -98} // Matrix input row
			} // Matrix input row
		},  // Matrix input row
		{ // Matrix input row
			{ // Matrix input row
				{52, 81, 124, -119}, // Matrix input row
				{127, 54, 93, -111}, // Matrix input row
				{1, -85, 10, 3}, // Matrix input row
				{-86, 1, -101, -128} // Matrix input row
			} // Matrix input row
		},  // Matrix input row
		{ // Matrix input row
			{ // Matrix input row
				{-63, 10, -113, -87}, // Matrix input row
				{61, -12, -54, 21}, // Matrix input row
				{-116, -22, -70, 75}, // Matrix input row
				{-27, 62, 13, -7} // Matrix input row
			} // Matrix input row
		},  // Matrix input row
		{ // Matrix input row
			{ // Matrix input row
				{-99, -47, 10, -40}, // Matrix input row
				{-63, 37, -20, -115}, // Matrix input row
				{127, -36, 113, 117}, // Matrix input row
				{-94, -22, 6, -125} // Matrix input row
			} // Matrix input row
		},  // Matrix input row
		{ // Matrix input row
			{ // Matrix input row
				{120, 21, 37, -14}, // Matrix input row
				{-58, -113, -110, 10}, // Matrix input row
				{34, -93, -78, 117}, // Matrix input row
				{101, -87, -12, -104} // Matrix input row
			} // Matrix input row
		},  // Matrix input row
		{ // Matrix input row
			{ // Matrix input row
				{19, -119, -3, 123}, // Matrix input row
				{-19, -42, -103, 62}, // Matrix input row
				{-85, -41, 57, -35}, // Matrix input row
				{-7, -13, -124, 1} // Matrix input row
			} // Matrix input row
		},  // Matrix input row
		{ // Matrix input row
			{ // Matrix input row
				{-4, 77, -114, -88}, // Matrix input row
				{-64, -121, -2, 76}, // Matrix input row
				{-123, -15, -61, -56}, // Matrix input row
				{-29, -26, -113, -15} // Matrix input row
			} // Matrix input row
		},  // Matrix input row
		{ // Matrix input row
			{ // Matrix input row
				{91, 4, -93, -56}, // Matrix input row
				{-33, -16, -60, -92}, // Matrix input row
				{6, -106, -81, 78}, // Matrix input row
				{-128, -100, -45, 31} // Matrix input row
			} // Matrix input row
		},  // Matrix input row
		{ // Matrix input row
			{ // Matrix input row
				{-118, 36, 34, -111}, // Matrix input row
				{-6, -19, -52, 20}, // Matrix input row
				{119, -88, 18, 72}, // Matrix input row
				{57, -106, 110, -8} // Matrix input row
			} // Matrix input row
		},  // Matrix input row
		{ // Matrix input row
			{ // Matrix input row
				{-12, -47, 44, -5}, // Matrix input row
				{14, -55, 6, -43}, // Matrix input row
				{90, 123, 88, 22}, // Matrix input row
				{-2, 41, 124, 18} // Matrix input row
			} // Matrix input row
		},  // Matrix input row
		{ // Matrix input row
			{ // Matrix input row
				{-31, -115, -125, 117}, // Matrix input row
				{5, -67, -56, 34}, // Matrix input row
				{-35, -128, 42, 21}, // Matrix input row
				{117, -37, 112, 3} // Matrix input row
			} // Matrix input row
		},  // Matrix input row
		{ // Matrix input row
			{ // Matrix input row
				{105, -83, 104, -65}, // Matrix input row
				{-76, 107, -93, 50}, // Matrix input row
				{84, -45, -56, 92}, // Matrix input row
				{-108, -63, -22, -107} // Matrix input row
			} // Matrix input row
		},  // Matrix input row
		{ // Matrix input row
			{ // Matrix input row
				{46, 31, 119, 81}, // Matrix input row
				{-94, -38, 20, -55}, // Matrix input row
				{-28, 79, 71, 76}, // Matrix input row
				{-62, 77, -98, 74} // Matrix input row
			} // Matrix input row
		},  // Matrix input row
		{ // Matrix input row
			{ // Matrix input row
				{-110, -44, -110, -23}, // Matrix input row
				{34, 115, 68, 90}, // Matrix input row
				{101, 23, 14, 50}, // Matrix input row
				{-24, 25, 36, 15} // Matrix input row
			} // Matrix input row
		},  // Matrix input row
		{ // Matrix input row
			{ // Matrix input row
				{7, -118, 80, -52}, // Matrix input row
				{-46, -77, 24, 56}, // Matrix input row
				{33, 97, 106, 1}, // Matrix input row
				{97, -101, 127, 96} // Matrix input row
			} // Matrix input row
		},  // Matrix input row
		{ // Matrix input row
			{ // Matrix input row
				{68, 6, -43, -50}, // Matrix input row
				{-125, -81, 41, 12}, // Matrix input row
				{71, 9, 54, -111}, // Matrix input row
				{-65, 66, -74, 125} // Matrix input row
			} // Matrix input row
		},  // Matrix input row
		{ // Matrix input row
			{ // Matrix input row
				{-57, -33, 54, 19}, // Matrix input row
				{111, 20, -22, -100}, // Matrix input row
				{-101, 127, -45, 78}, // Matrix input row
				{-19, -77, 125, 114} // Matrix input row
			} // Matrix input row
		},  // Matrix input row
		{ // Matrix input row
			{ // Matrix input row
				{-89, -76, 98, 88}, // Matrix input row
				{-121, -117, -20, 118}, // Matrix input row
				{48, -72, -78, -36}, // Matrix input row
				{-98, 101, 119, -95} // Matrix input row
			} // Matrix input row
		},  // Matrix input row
		{ // Matrix input row
			{ // Matrix input row
				{-85, -67, -98, -40}, // Matrix input row
				{97, -71, 71, -32}, // Matrix input row
				{82, -81, 18, -128}, // Matrix input row
				{111, -47, 86, 124} // Matrix input row
			} // Matrix input row
		},  // Matrix input row
		{ // Matrix input row
			{ // Matrix input row
				{-77, -107, -122, -86}, // Matrix input row
				{-101, 87, -125, -118}, // Matrix input row
				{-117, -28, -82, -118}, // Matrix input row
				{-28, 89, -59, 16} // Matrix input row
			} // Matrix input row
		},  // Matrix input row
		{ // Matrix input row
			{ // Matrix input row
				{-28, -105, -113, 114}, // Matrix input row
				{117, 80, 82, 37}, // Matrix input row
				{115, 12, -121, 28}, // Matrix input row
				{-9, -10, -110, -78} // Matrix input row
			} // Matrix input row
		},  // Matrix input row
		{ // Matrix input row
			{ // Matrix input row
				{-35, 39, 100, -11}, // Matrix input row
				{46, 53, 34, 54}, // Matrix input row
				{-76, 120, -44, 58}, // Matrix input row
				{121, -41, -73, -36} // Matrix input row
			} // Matrix input row
		},  // Matrix input row
		{ // Matrix input row
			{ // Matrix input row
				{-127, -107, -37, 62}, // Matrix input row
				{127, -40, -57, -20}, // Matrix input row
				{-75, 42, -125, -122}, // Matrix input row
				{-73, 28, -120, 17} // Matrix input row
			} // Matrix input row
		},  // Matrix input row
		{ // Matrix input row
			{ // Matrix input row
				{-102, 26, 37, -47}, // Matrix input row
				{-44, 42, 62, 1}, // Matrix input row
				{124, -5, 62, 32}, // Matrix input row
				{3, -69, -94, -4} // Matrix input row
			} // Matrix input row
		},  // Matrix input row
		{ // Matrix input row
			{ // Matrix input row
				{-10, -3, -25, -86}, // Matrix input row
				{-57, 20, 54, 6}, // Matrix input row
				{-82, -127, -86, 15}, // Matrix input row
				{25, -90, -111, 112} // Matrix input row
			} // Matrix input row
		},  // Matrix input row
		{ // Matrix input row
			{ // Matrix input row
				{-114, -124, 50, 61}, // Matrix input row
				{32, 45, 73, -78}, // Matrix input row
				{106, -109, -20, 24}, // Matrix input row
				{-49, -123, 60, 107} // Matrix input row
			} // Matrix input row
		},  // Matrix input row
		{ // Matrix input row
			{ // Matrix input row
				{72, 108, -17, -81}, // Matrix input row
				{107, -26, 64, 35}, // Matrix input row
				{-18, 37, 123, -10}, // Matrix input row
				{-73, 35, 105, 73} // Matrix input row
			} // Matrix input row
		},  // Matrix input row
		{ // Matrix input row
			{ // Matrix input row
				{-94, -15, -93, 44}, // Matrix input row
				{106, 55, -50, -9}, // Matrix input row
				{35, -38, 24, -55}, // Matrix input row
				{14, -57, -16, 57} // Matrix input row
			} // Matrix input row
		},  // Matrix input row
		{ // Matrix input row
			{ // Matrix input row
				{120, 67, -31, 108}, // Matrix input row
				{-15, 99, 81, 29}, // Matrix input row
				{-75, -70, 61, 51}, // Matrix input row
				{42, -94, 92, 57} // Matrix input row
			} // Matrix input row
		},  // Matrix input row
		{ // Matrix input row
			{ // Matrix input row
				{-122, 54, 59, -54}, // Matrix input row
				{10, -62, -34, -120}, // Matrix input row
				{-18, 109, 95, -36}, // Matrix input row
				{-106, 11, 36, 81} // Matrix input row
			} // Matrix input row
		} // Matrix input row
	}; // Matrix input row

  // Matrix weights, OUTPUT_CHANNELS, INPUT_CHANNELS, KERNEL_DIM, KERNEL_DIM Setup
	elem_t weights[OUTPUT_CHANNELS][INPUT_CHANNELS][KERNEL_DIM][KERNEL_DIM] = { // Matrix weights row
		{ // Matrix weights row
			{ // Matrix weights row
				{-119, -75}, // Matrix weights row
				{-52, 37} // Matrix weights row
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