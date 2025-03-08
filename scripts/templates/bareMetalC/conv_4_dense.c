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
	gemmini_extended4_config_ld(0, MVIN_SCALE_IDENTITY, false, 9, 2);
	gemmini_extended5_config_ld(32, MVIN_SCALE_IDENTITY, false, 16, 1, 0);
	gemmini_extended4_config_ld(32, MVIN_SCALE_IDENTITY, false, 128, 1);
	gemmini_config_ex(WS, 0, 0);
	gemmini_extended_config_st(1, 0, ACC_SCALE_IDENTITY);
	gemmini_extended_mvin(input + 0x0, 0x0, 32, 4);
	gemmini_extended_mvin(input + 0x80, 0x4, 32, 4);
	gemmini_extended_mvin(input + 0x100, 0x8, 32, 4);
	gemmini_extended_mvin(input + 0x180, 0xc, 32, 4);
	gemmini_extended_mvin2(weights + 0x0, 0x3f80, 1, 16);
	gemmini_extended_mvin2(weights + 0x10, 0x3f90, 1, 16);
	gemmini_extended_mvin2(weights + 0x20, 0x3fa0, 1, 16);
	gemmini_extended_mvin2(weights + 0x30, 0x3fb0, 1, 16);
	gemmini_extended_mvin2(weights + 0x40, 0x3fc0, 1, 16);
	gemmini_extended_mvin2(weights + 0x50, 0x3fd0, 1, 16);
	gemmini_extended_mvin2(weights + 0x60, 0x3fe0, 1, 16);
	gemmini_extended_mvin2(weights + 0x70, 0x3ff0, 1, 16);
	gemmini_extended_preload(0x3f80, 0xc0000000, 1, 16, 1, 3);
	gemmini_extended_compute_preloaded(0x0, 0xffffffff, 16, 3, 1, 3);
	gemmini_extended_preload(0xffffffffffffffff, 0xc0000003, 1, 16, 1, 3);
	gemmini_extended_compute_accumulated(0x4, 0xffffffff, 16, 3, 1, 3);
	gemmini_extended_preload(0xffffffffffffffff, 0xc0000006, 1, 16, 1, 3);
	gemmini_extended_compute_accumulated(0x8, 0xffffffff, 16, 3, 1, 3);
	gemmini_extended_preload(0x3f90, 0xc0000000, 1, 16, 1, 3);
	gemmini_extended_compute_preloaded(0x10, 0xffffffff, 16, 3, 1, 3);
	gemmini_extended_preload(0xffffffffffffffff, 0xc0000003, 1, 16, 1, 3);
	gemmini_extended_compute_accumulated(0x14, 0xffffffff, 16, 3, 1, 3);
	gemmini_extended_preload(0xffffffffffffffff, 0xc0000006, 1, 16, 1, 3);
	gemmini_extended_compute_accumulated(0x18, 0xffffffff, 16, 3, 1, 3);
	gemmini_extended_preload(0x3fa0, 0xc0000000, 1, 16, 1, 3);
	gemmini_extended_compute_preloaded(0x1, 0xffffffff, 16, 3, 1, 3);
	gemmini_extended_preload(0xffffffffffffffff, 0xc0000003, 1, 16, 1, 3);
	gemmini_extended_compute_accumulated(0x5, 0xffffffff, 16, 3, 1, 3);
	gemmini_extended_preload(0xffffffffffffffff, 0xc0000006, 1, 16, 1, 3);
	gemmini_extended_compute_accumulated(0x9, 0xffffffff, 16, 3, 1, 3);
	gemmini_extended_preload(0x3fb0, 0xc0000000, 1, 16, 1, 3);
	gemmini_extended_compute_preloaded(0x11, 0xffffffff, 16, 3, 1, 3);
	gemmini_extended_preload(0xffffffffffffffff, 0xc0000003, 1, 16, 1, 3);
	gemmini_extended_compute_accumulated(0x15, 0xffffffff, 16, 3, 1, 3);
	gemmini_extended_preload(0xffffffffffffffff, 0xc0000006, 1, 16, 1, 3);
	gemmini_extended_compute_accumulated(0x19, 0xffffffff, 16, 3, 1, 3);
	gemmini_extended_preload(0x3fc0, 0xc0000000, 1, 16, 1, 3);
	gemmini_extended_compute_preloaded(0x4, 0xffffffff, 16, 3, 1, 3);
	gemmini_extended_preload(0xffffffffffffffff, 0xc0000003, 1, 16, 1, 3);
	gemmini_extended_compute_accumulated(0x8, 0xffffffff, 16, 3, 1, 3);
	gemmini_extended_preload(0xffffffffffffffff, 0xc0000006, 1, 16, 1, 3);
	gemmini_extended_compute_accumulated(0xc, 0xffffffff, 16, 3, 1, 3);
	gemmini_extended_preload(0x3fd0, 0xc0000000, 1, 16, 1, 3);
	gemmini_extended_compute_preloaded(0x14, 0xffffffff, 16, 3, 1, 3);
	gemmini_extended_preload(0xffffffffffffffff, 0xc0000003, 1, 16, 1, 3);
	gemmini_extended_compute_accumulated(0x18, 0xffffffff, 16, 3, 1, 3);
	gemmini_extended_preload(0xffffffffffffffff, 0xc0000006, 1, 16, 1, 3);
	gemmini_extended_compute_accumulated(0x1c, 0xffffffff, 16, 3, 1, 3);
	gemmini_extended_preload(0x3fe0, 0xc0000000, 1, 16, 1, 3);
	gemmini_extended_compute_preloaded(0x5, 0xffffffff, 16, 3, 1, 3);
	gemmini_extended_preload(0xffffffffffffffff, 0xc0000003, 1, 16, 1, 3);
	gemmini_extended_compute_accumulated(0x9, 0xffffffff, 16, 3, 1, 3);
	gemmini_extended_preload(0xffffffffffffffff, 0xc0000006, 1, 16, 1, 3);
	gemmini_extended_compute_accumulated(0xd, 0xffffffff, 16, 3, 1, 3);
	gemmini_extended_preload(0x3ff0, 0xc0000000, 1, 16, 1, 3);
	gemmini_extended_compute_preloaded(0x15, 0xffffffff, 16, 3, 1, 3);
	gemmini_extended_preload(0xffffffffffffffff, 0xc0000003, 1, 16, 1, 3);
	gemmini_extended_compute_accumulated(0x19, 0xffffffff, 16, 3, 1, 3);
	gemmini_extended_preload(0xffffffffffffffff, 0xc0000006, 1, 16, 1, 3);
	gemmini_extended_compute_accumulated(0x1d, 0xffffffff, 16, 3, 1, 3);
	gemmini_extended_mvout(output + 0x0, 0xc0000000, 1, 3);
	gemmini_extended_mvout(output + 0x3, 0xc0000003, 1, 3);
	gemmini_extended_mvout(output + 0x6, 0xc0000006, 1, 3);

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