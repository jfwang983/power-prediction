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
  gemmini_extended4_config_ld(0, MVIN_SCALE_IDENTITY, false, 256, 2);
  gemmini_extended5_config_ld(1, MVIN_SCALE_IDENTITY, false, 256, 1, 0);
  gemmini_extended4_config_ld(1, MVIN_SCALE_IDENTITY, false, 1, 1);
  gemmini_config_ex(WS, 0, 0);
  gemmini_extended_config_st(1, 0, ACC_SCALE_IDENTITY);
  gemmini_extended_mvin(input + 0x0, 0x0, 1, 16);
  gemmini_extended_mvin(input + 0x10, 0x10, 1, 16);
  gemmini_extended_mvin(input + 0x20, 0x20, 1, 16);
  gemmini_extended_mvin(input + 0x30, 0x30, 1, 16);
  gemmini_extended_mvin(input + 0x40, 0x40, 1, 16);
  gemmini_extended_mvin(input + 0x50, 0x50, 1, 16);
  gemmini_extended_mvin(input + 0x60, 0x60, 1, 16);
  gemmini_extended_mvin(input + 0x70, 0x70, 1, 16);
  gemmini_extended_mvin(input + 0x80, 0x80, 1, 16);
  gemmini_extended_mvin(input + 0x90, 0x90, 1, 16);
  gemmini_extended_mvin(input + 0xa0, 0xa0, 1, 16);
  gemmini_extended_mvin(input + 0xb0, 0xb0, 1, 16);
  gemmini_extended_mvin(input + 0xc0, 0xc0, 1, 16);
  gemmini_extended_mvin(input + 0xd0, 0xd0, 1, 16);
  gemmini_extended_mvin(input + 0xe0, 0xe0, 1, 16);
  gemmini_extended_mvin(input + 0xf0, 0xf0, 1, 16);
  gemmini_extended_mvin2(weights + 0x0, 0x3fff, 1, 1);
  gemmini_extended_preload(0x3fff, 0xc0000000, 1, 1, 1, 16);
  gemmini_extended_compute_preloaded(0x0, 0xffffffff, 1, 16, 1, 16);
  gemmini_extended_preload(0xffffffffffffffff, 0xc0000010, 1, 1, 1, 16);
  gemmini_extended_compute_accumulated(0x10, 0xffffffff, 1, 16, 1, 16);
  gemmini_extended_preload(0xffffffffffffffff, 0xc0000020, 1, 1, 1, 16);
  gemmini_extended_compute_accumulated(0x20, 0xffffffff, 1, 16, 1, 16);
  gemmini_extended_preload(0xffffffffffffffff, 0xc0000030, 1, 1, 1, 16);
  gemmini_extended_compute_accumulated(0x30, 0xffffffff, 1, 16, 1, 16);
  gemmini_extended_preload(0xffffffffffffffff, 0xc0000040, 1, 1, 1, 16);
  gemmini_extended_compute_accumulated(0x40, 0xffffffff, 1, 16, 1, 16);
  gemmini_extended_preload(0xffffffffffffffff, 0xc0000050, 1, 1, 1, 16);
  gemmini_extended_compute_accumulated(0x50, 0xffffffff, 1, 16, 1, 16);
  gemmini_extended_preload(0xffffffffffffffff, 0xc0000060, 1, 1, 1, 16);
  gemmini_extended_compute_accumulated(0x60, 0xffffffff, 1, 16, 1, 16);
  gemmini_extended_preload(0xffffffffffffffff, 0xc0000070, 1, 1, 1, 16);
  gemmini_extended_compute_accumulated(0x70, 0xffffffff, 1, 16, 1, 16);
  gemmini_extended_preload(0xffffffffffffffff, 0xc0000080, 1, 1, 1, 16);
  gemmini_extended_compute_accumulated(0x80, 0xffffffff, 1, 16, 1, 16);
  gemmini_extended_preload(0xffffffffffffffff, 0xc0000090, 1, 1, 1, 16);
  gemmini_extended_compute_accumulated(0x90, 0xffffffff, 1, 16, 1, 16);
  gemmini_extended_preload(0xffffffffffffffff, 0xc00000a0, 1, 1, 1, 16);
  gemmini_extended_compute_accumulated(0xa0, 0xffffffff, 1, 16, 1, 16);
  gemmini_extended_preload(0xffffffffffffffff, 0xc00000b0, 1, 1, 1, 16);
  gemmini_extended_compute_accumulated(0xb0, 0xffffffff, 1, 16, 1, 16);
  gemmini_extended_preload(0xffffffffffffffff, 0xc00000c0, 1, 1, 1, 16);
  gemmini_extended_compute_accumulated(0xc0, 0xffffffff, 1, 16, 1, 16);
  gemmini_extended_preload(0xffffffffffffffff, 0xc00000d0, 1, 1, 1, 16);
  gemmini_extended_compute_accumulated(0xd0, 0xffffffff, 1, 16, 1, 16);
  gemmini_extended_preload(0xffffffffffffffff, 0xc00000e0, 1, 1, 1, 16);
  gemmini_extended_compute_accumulated(0xe0, 0xffffffff, 1, 16, 1, 16);
  gemmini_extended_preload(0xffffffffffffffff, 0xc00000f0, 1, 1, 1, 16);
  gemmini_extended_compute_accumulated(0xf0, 0xffffffff, 1, 16, 1, 16);
  gemmini_extended_mvout(output + 0x0, 0xc0000000, 1, 16);
  gemmini_extended_mvout(output + 0x10, 0xc0000010, 1, 16);
  gemmini_extended_mvout(output + 0x20, 0xc0000020, 1, 16);
  gemmini_extended_mvout(output + 0x30, 0xc0000030, 1, 16);
  gemmini_extended_mvout(output + 0x40, 0xc0000040, 1, 16);
  gemmini_extended_mvout(output + 0x50, 0xc0000050, 1, 16);
  gemmini_extended_mvout(output + 0x60, 0xc0000060, 1, 16);
  gemmini_extended_mvout(output + 0x70, 0xc0000070, 1, 16);
  gemmini_extended_mvout(output + 0x80, 0xc0000080, 1, 16);
  gemmini_extended_mvout(output + 0x90, 0xc0000090, 1, 16);
  gemmini_extended_mvout(output + 0xa0, 0xc00000a0, 1, 16);
  gemmini_extended_mvout(output + 0xb0, 0xc00000b0, 1, 16);
  gemmini_extended_mvout(output + 0xc0, 0xc00000c0, 1, 16);
  gemmini_extended_mvout(output + 0xd0, 0xc00000d0, 1, 16);
  gemmini_extended_mvout(output + 0xe0, 0xc00000e0, 1, 16);
  gemmini_extended_mvout(output + 0xf0, 0xc00000f0, 1, 16);

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