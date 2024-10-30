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
#define INPUT_CHANNELS  32
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
				{81, 97, -106, 105}, // Matrix input row
				{-50, 18, 75, 119}, // Matrix input row
				{98, 12, -22, -16}, // Matrix input row
				{-78, 48, 2, 83} // Matrix input row
			},  // Matrix input row
			{ // Matrix input row
				{-91, 18, -12, 22}, // Matrix input row
				{-16, 76, -40, -106}, // Matrix input row
				{-33, -19, 126, -66}, // Matrix input row
				{27, 93, -128, -106} // Matrix input row
			},  // Matrix input row
			{ // Matrix input row
				{99, -91, -67, 118}, // Matrix input row
				{-122, -10, 19, 120}, // Matrix input row
				{83, 91, 110, -47}, // Matrix input row
				{-64, 70, -89, 125} // Matrix input row
			},  // Matrix input row
			{ // Matrix input row
				{-90, 70, 2, -59}, // Matrix input row
				{52, -61, -49, 91}, // Matrix input row
				{95, 12, 76, -8}, // Matrix input row
				{-121, -93, -86, -118} // Matrix input row
			},  // Matrix input row
			{ // Matrix input row
				{-31, -119, 89, 67}, // Matrix input row
				{-57, -5, 112, 103}, // Matrix input row
				{-111, -100, -105, 16}, // Matrix input row
				{83, 125, -71, -38} // Matrix input row
			},  // Matrix input row
			{ // Matrix input row
				{86, 126, 120, -122}, // Matrix input row
				{24, 1, -127, -103}, // Matrix input row
				{-23, -51, -54, 26}, // Matrix input row
				{-55, -109, -9, -69} // Matrix input row
			},  // Matrix input row
			{ // Matrix input row
				{52, -17, 125, -69}, // Matrix input row
				{51, -39, -126, 36}, // Matrix input row
				{-99, -95, 20, -88}, // Matrix input row
				{28, 74, 55, -28} // Matrix input row
			},  // Matrix input row
			{ // Matrix input row
				{-31, -116, 60, 77}, // Matrix input row
				{-123, 108, 101, 56}, // Matrix input row
				{55, 111, 107, -101}, // Matrix input row
				{65, 25, -61, 31} // Matrix input row
			},  // Matrix input row
			{ // Matrix input row
				{26, 103, 101, -59}, // Matrix input row
				{-6, 21, 68, -116}, // Matrix input row
				{106, -6, 18, 52}, // Matrix input row
				{-84, -116, -33, 113} // Matrix input row
			},  // Matrix input row
			{ // Matrix input row
				{-99, -65, 60, -21}, // Matrix input row
				{-25, -119, 81, -26}, // Matrix input row
				{-33, 126, -81, -111}, // Matrix input row
				{113, -38, 81, -1} // Matrix input row
			},  // Matrix input row
			{ // Matrix input row
				{-32, 123, 97, -41}, // Matrix input row
				{63, -24, -103, 19}, // Matrix input row
				{-75, -82, -32, 126}, // Matrix input row
				{29, -33, -56, -42} // Matrix input row
			},  // Matrix input row
			{ // Matrix input row
				{-30, 61, -49, -61}, // Matrix input row
				{80, 105, 107, -126}, // Matrix input row
				{-61, -76, -14, -87}, // Matrix input row
				{66, 32, 96, 113} // Matrix input row
			},  // Matrix input row
			{ // Matrix input row
				{2, -58, 42, -48}, // Matrix input row
				{2, -47, 38, 18}, // Matrix input row
				{18, 63, 98, 86}, // Matrix input row
				{-97, -14, 46, 4} // Matrix input row
			},  // Matrix input row
			{ // Matrix input row
				{102, -51, -109, 123}, // Matrix input row
				{-93, -68, -96, 88}, // Matrix input row
				{-8, 9, -73, 124}, // Matrix input row
				{-79, 90, -33, 59} // Matrix input row
			},  // Matrix input row
			{ // Matrix input row
				{-19, 15, -16, -92}, // Matrix input row
				{39, 92, 125, 77}, // Matrix input row
				{28, -108, -125, 64}, // Matrix input row
				{11, 27, -93, 98} // Matrix input row
			},  // Matrix input row
			{ // Matrix input row
				{-76, -78, 16, -17}, // Matrix input row
				{31, 80, 78, 84}, // Matrix input row
				{-107, 32, -97, 115}, // Matrix input row
				{-114, 52, 60, -39} // Matrix input row
			},  // Matrix input row
			{ // Matrix input row
				{-103, 26, 87, -75}, // Matrix input row
				{80, 100, 1, -106}, // Matrix input row
				{27, 81, 44, -16}, // Matrix input row
				{-27, -43, 21, 69} // Matrix input row
			},  // Matrix input row
			{ // Matrix input row
				{-45, 46, 38, 69}, // Matrix input row
				{-5, -33, 60, 93}, // Matrix input row
				{113, -111, 73, -49}, // Matrix input row
				{2, 98, 53, 3} // Matrix input row
			},  // Matrix input row
			{ // Matrix input row
				{-6, -21, 11, 44}, // Matrix input row
				{39, -11, -2, -80}, // Matrix input row
				{72, 28, -70, 74}, // Matrix input row
				{-124, 49, -81, 62} // Matrix input row
			},  // Matrix input row
			{ // Matrix input row
				{13, 78, -2, 29}, // Matrix input row
				{-47, 62, 3, -84}, // Matrix input row
				{-78, -56, -67, -76}, // Matrix input row
				{120, 72, 91, -8} // Matrix input row
			},  // Matrix input row
			{ // Matrix input row
				{6, 86, -80, 22}, // Matrix input row
				{-79, -86, 103, -45}, // Matrix input row
				{-127, -118, -103, 111}, // Matrix input row
				{-84, 28, 71, -64} // Matrix input row
			},  // Matrix input row
			{ // Matrix input row
				{58, 77, -57, 46}, // Matrix input row
				{72, 99, -90, 118}, // Matrix input row
				{-52, 4, -128, 79}, // Matrix input row
				{-43, 119, 4, -6} // Matrix input row
			},  // Matrix input row
			{ // Matrix input row
				{-22, -15, -23, -4}, // Matrix input row
				{127, -66, -52, 16}, // Matrix input row
				{-73, 68, 18, -50}, // Matrix input row
				{-116, 20, 99, 85} // Matrix input row
			},  // Matrix input row
			{ // Matrix input row
				{-49, -109, 87, -73}, // Matrix input row
				{106, -78, 103, -122}, // Matrix input row
				{29, -11, -51, -6}, // Matrix input row
				{-80, 122, 108, 82} // Matrix input row
			},  // Matrix input row
			{ // Matrix input row
				{-82, -45, -55, -8}, // Matrix input row
				{68, 87, 24, -26}, // Matrix input row
				{98, -105, 16, -103}, // Matrix input row
				{116, -39, 53, 62} // Matrix input row
			},  // Matrix input row
			{ // Matrix input row
				{119, 36, 35, -96}, // Matrix input row
				{39, 90, -124, 38}, // Matrix input row
				{-104, 11, 53, -63}, // Matrix input row
				{102, 107, -105, -46} // Matrix input row
			},  // Matrix input row
			{ // Matrix input row
				{49, -89, -25, -10}, // Matrix input row
				{41, 49, 120, -26}, // Matrix input row
				{-16, 123, 28, -54}, // Matrix input row
				{17, -84, 5, 93} // Matrix input row
			},  // Matrix input row
			{ // Matrix input row
				{67, 60, -88, 86}, // Matrix input row
				{33, 36, 114, -83}, // Matrix input row
				{113, -39, 11, -119}, // Matrix input row
				{31, -73, -119, -63} // Matrix input row
			},  // Matrix input row
			{ // Matrix input row
				{75, 110, -17, -91}, // Matrix input row
				{110, -89, -17, -107}, // Matrix input row
				{2, 47, -64, 122}, // Matrix input row
				{83, 84, -24, 48} // Matrix input row
			},  // Matrix input row
			{ // Matrix input row
				{14, -29, -19, 6}, // Matrix input row
				{-38, 83, -100, 45}, // Matrix input row
				{-127, -84, 77, -45}, // Matrix input row
				{0, -57, 125, 80} // Matrix input row
			},  // Matrix input row
			{ // Matrix input row
				{36, -40, 92, -35}, // Matrix input row
				{-80, 39, 113, -8}, // Matrix input row
				{61, -88, 122, 52}, // Matrix input row
				{-61, -42, -19, 111} // Matrix input row
			},  // Matrix input row
			{ // Matrix input row
				{79, 116, -26, -38}, // Matrix input row
				{127, -118, 106, -8}, // Matrix input row
				{78, -102, 33, 9}, // Matrix input row
				{-25, 23, -30, -102} // Matrix input row
			} // Matrix input row
		} // Matrix input row
	}; // Matrix input row

  // Matrix weights, OUTPUT_CHANNELS, INPUT_CHANNELS, KERNEL_DIM, KERNEL_DIM Setup
	elem_t weights[OUTPUT_CHANNELS][INPUT_CHANNELS][KERNEL_DIM][KERNEL_DIM] = { // Matrix weights row
		{ // Matrix weights row
			{ // Matrix weights row
				{-68, -89}, // Matrix weights row
				{-99, -122} // Matrix weights row
			},  // Matrix weights row
			{ // Matrix weights row
				{-49, -83}, // Matrix weights row
				{120, -124} // Matrix weights row
			},  // Matrix weights row
			{ // Matrix weights row
				{-102, -98}, // Matrix weights row
				{-87, -75} // Matrix weights row
			},  // Matrix weights row
			{ // Matrix weights row
				{-110, 121}, // Matrix weights row
				{-77, 119} // Matrix weights row
			},  // Matrix weights row
			{ // Matrix weights row
				{19, -26}, // Matrix weights row
				{-33, -61} // Matrix weights row
			},  // Matrix weights row
			{ // Matrix weights row
				{-40, 91}, // Matrix weights row
				{14, 32} // Matrix weights row
			},  // Matrix weights row
			{ // Matrix weights row
				{-31, -86}, // Matrix weights row
				{4, -93} // Matrix weights row
			},  // Matrix weights row
			{ // Matrix weights row
				{-22, 42}, // Matrix weights row
				{46, 76} // Matrix weights row
			},  // Matrix weights row
			{ // Matrix weights row
				{-14, 85}, // Matrix weights row
				{51, 61} // Matrix weights row
			},  // Matrix weights row
			{ // Matrix weights row
				{-48, -5}, // Matrix weights row
				{9, 126} // Matrix weights row
			},  // Matrix weights row
			{ // Matrix weights row
				{-33, -75}, // Matrix weights row
				{114, -111} // Matrix weights row
			},  // Matrix weights row
			{ // Matrix weights row
				{43, -91}, // Matrix weights row
				{-34, 13} // Matrix weights row
			},  // Matrix weights row
			{ // Matrix weights row
				{-89, 24}, // Matrix weights row
				{26, 10} // Matrix weights row
			},  // Matrix weights row
			{ // Matrix weights row
				{37, 36}, // Matrix weights row
				{23, 32} // Matrix weights row
			},  // Matrix weights row
			{ // Matrix weights row
				{51, -110}, // Matrix weights row
				{10, 28} // Matrix weights row
			},  // Matrix weights row
			{ // Matrix weights row
				{100, 115}, // Matrix weights row
				{-100, -11} // Matrix weights row
			},  // Matrix weights row
			{ // Matrix weights row
				{-80, -58}, // Matrix weights row
				{52, -62} // Matrix weights row
			},  // Matrix weights row
			{ // Matrix weights row
				{-73, 71}, // Matrix weights row
				{-22, -5} // Matrix weights row
			},  // Matrix weights row
			{ // Matrix weights row
				{0, -68}, // Matrix weights row
				{74, 38} // Matrix weights row
			},  // Matrix weights row
			{ // Matrix weights row
				{59, 48}, // Matrix weights row
				{64, 116} // Matrix weights row
			},  // Matrix weights row
			{ // Matrix weights row
				{116, -112}, // Matrix weights row
				{16, -123} // Matrix weights row
			},  // Matrix weights row
			{ // Matrix weights row
				{-82, 102}, // Matrix weights row
				{-117, -29} // Matrix weights row
			},  // Matrix weights row
			{ // Matrix weights row
				{-120, -63}, // Matrix weights row
				{16, -52} // Matrix weights row
			},  // Matrix weights row
			{ // Matrix weights row
				{-5, -7}, // Matrix weights row
				{79, -91} // Matrix weights row
			},  // Matrix weights row
			{ // Matrix weights row
				{61, -25}, // Matrix weights row
				{-78, 53} // Matrix weights row
			},  // Matrix weights row
			{ // Matrix weights row
				{-22, -84}, // Matrix weights row
				{56, -95} // Matrix weights row
			},  // Matrix weights row
			{ // Matrix weights row
				{90, 39}, // Matrix weights row
				{-55, 50} // Matrix weights row
			},  // Matrix weights row
			{ // Matrix weights row
				{52, -85}, // Matrix weights row
				{76, -92} // Matrix weights row
			},  // Matrix weights row
			{ // Matrix weights row
				{-114, -122}, // Matrix weights row
				{12, 96} // Matrix weights row
			},  // Matrix weights row
			{ // Matrix weights row
				{35, -124}, // Matrix weights row
				{48, -37} // Matrix weights row
			},  // Matrix weights row
			{ // Matrix weights row
				{-33, -121}, // Matrix weights row
				{114, -74} // Matrix weights row
			},  // Matrix weights row
			{ // Matrix weights row
				{20, 24}, // Matrix weights row
				{-56, -15} // Matrix weights row
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