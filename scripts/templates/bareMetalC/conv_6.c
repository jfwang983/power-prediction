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
#define INPUT_CHANNELS  16
#define INPUT_ROW_DIM   32
#define INPUT_COL_DIM   32
#define OUTPUT_CHANNELS 1
#define KERNEL_DIM      3
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

  // Matrix input, elem_t, BATCH_SIZE, INPUT_CHANNELS, INPUT_ROW_DIM, INPUT_COL_DIM Setup

  // Matrix weights, elem_t, OUTPUT_CHANNELS, INPUT_CHANNELS, KERNEL_DIM, KERNEL_DIM Setup

  elem_t output[BATCH_SIZE][OUTPUT_CHANNELS][OUTPUT_ROW_DIM][OUTPUT_COL_DIM];

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