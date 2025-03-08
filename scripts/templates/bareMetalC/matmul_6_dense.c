// See LICENSE for license details.

#include <stdint.h>
#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>
#ifndef BAREMETAL
#include <sys/mman.h>
#endif
#include "matmul_funcs.h"

#define BATCH_SIZE  64
#define INPUT_SIZE  32
#define HIDDEN_SIZE 128

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

  // Matrix A, elem_t, BATCH_SIZE, INPUT_SIZE Setup

  // Matrix B, elem_t, INPUT_SIZE, HIDDEN_SIZE Setup	

  // Matrix C
  elem_t C[BATCH_SIZE][HIDDEN_SIZE];

  // Setup end
  setup_end = read_cycles();

  // Gemmini instructions start
  gemmini_fence();

  // Tiled matmul start
  start = read_cycles();

  // Clear TLB
  gemmini_flush(0);

  // Main benchmark code
  gemmini_extended_config_ex(WS, 0, 0, 1, 0, 0);
  gemmini_extended_config_st(128, 0, ACC_SCALE_IDENTITY);
  gemmini_extended3_config_ld(32, MVIN_SCALE_IDENTITY, 0, 0);
  gemmini_extended3_config_ld(128, MVIN_SCALE_IDENTITY, 0, 1);
  gemmini_extended3_config_ld(512, MVIN_SCALE_IDENTITY, 0, 2);
  gemmini_extended_mvin(A + 0x0, 0x0, 32, 16);
  gemmini_extended_mvin2(B + 0x0, 0x3f00, 64, 16);
  gemmini_extended_preload(0x3f00, 0x80000000, 16, 16, 16, 16);
  gemmini_extended_compute_preloaded(0x0, 0xffffffff, 16, 16, 16, 16);
  gemmini_extended_mvin(A + 0x200, 0x20, 32, 16);
  gemmini_extended_preload(0xffffffffffffffff, 0x80000080, 16, 16, 16, 16);
  gemmini_extended_compute_accumulated(0x20, 0xffffffff, 16, 16, 16, 16);
  gemmini_extended_mvin(A + 0x400, 0x40, 32, 16);
  gemmini_extended_preload(0xffffffffffffffff, 0x80000100, 16, 16, 16, 16);
  gemmini_extended_compute_accumulated(0x40, 0xffffffff, 16, 16, 16, 16);
  gemmini_extended_mvin(A + 0x600, 0x60, 32, 16);
  gemmini_extended_preload(0xffffffffffffffff, 0x80000180, 16, 16, 16, 16);
  gemmini_extended_compute_accumulated(0x60, 0xffffffff, 16, 16, 16, 16);
  gemmini_extended_preload(0x3f10, 0x80000010, 16, 16, 16, 16);
  gemmini_extended_compute_preloaded(0x0, 0xffffffff, 16, 16, 16, 16);
  gemmini_extended_preload(0xffffffffffffffff, 0x80000090, 16, 16, 16, 16);
  gemmini_extended_compute_accumulated(0x20, 0xffffffff, 16, 16, 16, 16);
  gemmini_extended_preload(0xffffffffffffffff, 0x80000110, 16, 16, 16, 16);
  gemmini_extended_compute_accumulated(0x40, 0xffffffff, 16, 16, 16, 16);
  gemmini_extended_preload(0xffffffffffffffff, 0x80000190, 16, 16, 16, 16);
  gemmini_extended_compute_accumulated(0x60, 0xffffffff, 16, 16, 16, 16);
  gemmini_extended_preload(0x3f20, 0x80000020, 16, 16, 16, 16);
  gemmini_extended_compute_preloaded(0x0, 0xffffffff, 16, 16, 16, 16);
  gemmini_extended_preload(0xffffffffffffffff, 0x800000a0, 16, 16, 16, 16);
  gemmini_extended_compute_accumulated(0x20, 0xffffffff, 16, 16, 16, 16);
  gemmini_extended_preload(0xffffffffffffffff, 0x80000120, 16, 16, 16, 16);
  gemmini_extended_compute_accumulated(0x40, 0xffffffff, 16, 16, 16, 16);
  gemmini_extended_preload(0xffffffffffffffff, 0x800001a0, 16, 16, 16, 16);
  gemmini_extended_compute_accumulated(0x60, 0xffffffff, 16, 16, 16, 16);
  gemmini_extended_preload(0x3f30, 0x80000030, 16, 16, 16, 16);
  gemmini_extended_compute_preloaded(0x0, 0xffffffff, 16, 16, 16, 16);
  gemmini_extended_preload(0xffffffffffffffff, 0x800000b0, 16, 16, 16, 16);
  gemmini_extended_compute_accumulated(0x20, 0xffffffff, 16, 16, 16, 16);
  gemmini_extended_preload(0xffffffffffffffff, 0x80000130, 16, 16, 16, 16);
  gemmini_extended_compute_accumulated(0x40, 0xffffffff, 16, 16, 16, 16);
  gemmini_extended_preload(0xffffffffffffffff, 0x800001b0, 16, 16, 16, 16);
  gemmini_extended_compute_accumulated(0x60, 0xffffffff, 16, 16, 16, 16);
  gemmini_extended_mvin2(B + 0x40, 0x3f40, 64, 16);
  gemmini_extended_preload(0x3f40, 0x80000040, 16, 16, 16, 16);
  gemmini_extended_compute_preloaded(0x0, 0xffffffff, 16, 16, 16, 16);
  gemmini_extended_preload(0xffffffffffffffff, 0x800000c0, 16, 16, 16, 16);
  gemmini_extended_compute_accumulated(0x20, 0xffffffff, 16, 16, 16, 16);
  gemmini_extended_preload(0xffffffffffffffff, 0x80000140, 16, 16, 16, 16);
  gemmini_extended_compute_accumulated(0x40, 0xffffffff, 16, 16, 16, 16);
  gemmini_extended_preload(0xffffffffffffffff, 0x800001c0, 16, 16, 16, 16);
  gemmini_extended_compute_accumulated(0x60, 0xffffffff, 16, 16, 16, 16);
  gemmini_extended_preload(0x3f50, 0x80000050, 16, 16, 16, 16);
  gemmini_extended_compute_preloaded(0x0, 0xffffffff, 16, 16, 16, 16);
  gemmini_extended_preload(0xffffffffffffffff, 0x800000d0, 16, 16, 16, 16);
  gemmini_extended_compute_accumulated(0x20, 0xffffffff, 16, 16, 16, 16);
  gemmini_extended_preload(0xffffffffffffffff, 0x80000150, 16, 16, 16, 16);
  gemmini_extended_compute_accumulated(0x40, 0xffffffff, 16, 16, 16, 16);
  gemmini_extended_preload(0xffffffffffffffff, 0x800001d0, 16, 16, 16, 16);
  gemmini_extended_compute_accumulated(0x60, 0xffffffff, 16, 16, 16, 16);
  gemmini_extended_preload(0x3f60, 0x80000060, 16, 16, 16, 16);
  gemmini_extended_compute_preloaded(0x0, 0xffffffff, 16, 16, 16, 16);
  gemmini_extended_preload(0xffffffffffffffff, 0x800000e0, 16, 16, 16, 16);
  gemmini_extended_compute_accumulated(0x20, 0xffffffff, 16, 16, 16, 16);
  gemmini_extended_preload(0xffffffffffffffff, 0x80000160, 16, 16, 16, 16);
  gemmini_extended_compute_accumulated(0x40, 0xffffffff, 16, 16, 16, 16);
  gemmini_extended_preload(0xffffffffffffffff, 0x800001e0, 16, 16, 16, 16);
  gemmini_extended_compute_accumulated(0x60, 0xffffffff, 16, 16, 16, 16);
  gemmini_extended_preload(0x3f70, 0x80000070, 16, 16, 16, 16);
  gemmini_extended_compute_preloaded(0x0, 0xffffffff, 16, 16, 16, 16);
  gemmini_extended_preload(0xffffffffffffffff, 0x800000f0, 16, 16, 16, 16);
  gemmini_extended_compute_accumulated(0x20, 0xffffffff, 16, 16, 16, 16);
  gemmini_extended_preload(0xffffffffffffffff, 0x80000170, 16, 16, 16, 16);
  gemmini_extended_compute_accumulated(0x40, 0xffffffff, 16, 16, 16, 16);
  gemmini_extended_preload(0xffffffffffffffff, 0x800001f0, 16, 16, 16, 16);
  gemmini_extended_compute_accumulated(0x60, 0xffffffff, 16, 16, 16, 16);
  gemmini_extended_mvin2(B + 0x800, 0x3f80, 64, 16);
  gemmini_extended_preload(0x3f80, 0xc0000000, 16, 16, 16, 16);
  gemmini_extended_compute_preloaded(0x10, 0xffffffff, 16, 16, 16, 16);
  gemmini_extended_preload(0xffffffffffffffff, 0xc0000080, 16, 16, 16, 16);
  gemmini_extended_compute_accumulated(0x30, 0xffffffff, 16, 16, 16, 16);
  gemmini_extended_preload(0xffffffffffffffff, 0xc0000100, 16, 16, 16, 16);
  gemmini_extended_compute_accumulated(0x50, 0xffffffff, 16, 16, 16, 16);
  gemmini_extended_preload(0xffffffffffffffff, 0xc0000180, 16, 16, 16, 16);
  gemmini_extended_compute_accumulated(0x70, 0xffffffff, 16, 16, 16, 16);
  gemmini_extended_preload(0x3f90, 0xc0000010, 16, 16, 16, 16);
  gemmini_extended_compute_preloaded(0x10, 0xffffffff, 16, 16, 16, 16);
  gemmini_extended_preload(0xffffffffffffffff, 0xc0000090, 16, 16, 16, 16);
  gemmini_extended_compute_accumulated(0x30, 0xffffffff, 16, 16, 16, 16);
  gemmini_extended_preload(0xffffffffffffffff, 0xc0000110, 16, 16, 16, 16);
  gemmini_extended_compute_accumulated(0x50, 0xffffffff, 16, 16, 16, 16);
  gemmini_extended_preload(0xffffffffffffffff, 0xc0000190, 16, 16, 16, 16);
  gemmini_extended_compute_accumulated(0x70, 0xffffffff, 16, 16, 16, 16);
  gemmini_extended_preload(0x3fa0, 0xc0000020, 16, 16, 16, 16);
  gemmini_extended_compute_preloaded(0x10, 0xffffffff, 16, 16, 16, 16);
  gemmini_extended_preload(0xffffffffffffffff, 0xc00000a0, 16, 16, 16, 16);
  gemmini_extended_compute_accumulated(0x30, 0xffffffff, 16, 16, 16, 16);
  gemmini_extended_preload(0xffffffffffffffff, 0xc0000120, 16, 16, 16, 16);
  gemmini_extended_compute_accumulated(0x50, 0xffffffff, 16, 16, 16, 16);
  gemmini_extended_preload(0xffffffffffffffff, 0xc00001a0, 16, 16, 16, 16);
  gemmini_extended_compute_accumulated(0x70, 0xffffffff, 16, 16, 16, 16);
  gemmini_extended_preload(0x3fb0, 0xc0000030, 16, 16, 16, 16);
  gemmini_extended_compute_preloaded(0x10, 0xffffffff, 16, 16, 16, 16);
  gemmini_extended_mvout(C + 0x0, 0xc0000000, 64, 16);
  gemmini_extended_preload(0xffffffffffffffff, 0xc00000b0, 16, 16, 16, 16);
  gemmini_extended_compute_accumulated(0x30, 0xffffffff, 16, 16, 16, 16);
  gemmini_extended_mvout(C + 0x800, 0xc0000080, 64, 16);
  gemmini_extended_preload(0xffffffffffffffff, 0xc0000130, 16, 16, 16, 16);
  gemmini_extended_compute_accumulated(0x50, 0xffffffff, 16, 16, 16, 16);
  gemmini_extended_mvout(C + 0x1000, 0xc0000100, 64, 16);
  gemmini_extended_preload(0xffffffffffffffff, 0xc00001b0, 16, 16, 16, 16);
  gemmini_extended_compute_accumulated(0x70, 0xffffffff, 16, 16, 16, 16);
  gemmini_extended_mvout(C + 0x1800, 0xc0000180, 64, 16);
  gemmini_extended_mvin2(B + 0x840, 0x3fc0, 64, 16);
  gemmini_extended_preload(0x3fc0, 0xc0000040, 16, 16, 16, 16);
  gemmini_extended_compute_preloaded(0x10, 0xffffffff, 16, 16, 16, 16);
  gemmini_extended_preload(0xffffffffffffffff, 0xc00000c0, 16, 16, 16, 16);
  gemmini_extended_compute_accumulated(0x30, 0xffffffff, 16, 16, 16, 16);
  gemmini_extended_preload(0xffffffffffffffff, 0xc0000140, 16, 16, 16, 16);
  gemmini_extended_compute_accumulated(0x50, 0xffffffff, 16, 16, 16, 16);
  gemmini_extended_preload(0xffffffffffffffff, 0xc00001c0, 16, 16, 16, 16);
  gemmini_extended_compute_accumulated(0x70, 0xffffffff, 16, 16, 16, 16);
  gemmini_extended_preload(0x3fd0, 0xc0000050, 16, 16, 16, 16);
  gemmini_extended_compute_preloaded(0x10, 0xffffffff, 16, 16, 16, 16);
  gemmini_extended_preload(0xffffffffffffffff, 0xc00000d0, 16, 16, 16, 16);
  gemmini_extended_compute_accumulated(0x30, 0xffffffff, 16, 16, 16, 16);
  gemmini_extended_preload(0xffffffffffffffff, 0xc0000150, 16, 16, 16, 16);
  gemmini_extended_compute_accumulated(0x50, 0xffffffff, 16, 16, 16, 16);
  gemmini_extended_preload(0xffffffffffffffff, 0xc00001d0, 16, 16, 16, 16);
  gemmini_extended_compute_accumulated(0x70, 0xffffffff, 16, 16, 16, 16);
  gemmini_extended_preload(0x3fe0, 0xc0000060, 16, 16, 16, 16);
  gemmini_extended_compute_preloaded(0x10, 0xffffffff, 16, 16, 16, 16);
  gemmini_extended_preload(0xffffffffffffffff, 0xc00000e0, 16, 16, 16, 16);
  gemmini_extended_compute_accumulated(0x30, 0xffffffff, 16, 16, 16, 16);
  gemmini_extended_preload(0xffffffffffffffff, 0xc0000160, 16, 16, 16, 16);
  gemmini_extended_compute_accumulated(0x50, 0xffffffff, 16, 16, 16, 16);
  gemmini_extended_preload(0xffffffffffffffff, 0xc00001e0, 16, 16, 16, 16);
  gemmini_extended_compute_accumulated(0x70, 0xffffffff, 16, 16, 16, 16);
  gemmini_extended_preload(0x3ff0, 0xc0000070, 16, 16, 16, 16);
  gemmini_extended_compute_preloaded(0x10, 0xffffffff, 16, 16, 16, 16);
  gemmini_extended_mvout(C + 0x40, 0xc0000040, 64, 16);
  gemmini_extended_preload(0xffffffffffffffff, 0xc00000f0, 16, 16, 16, 16);
  gemmini_extended_compute_accumulated(0x30, 0xffffffff, 16, 16, 16, 16);
  gemmini_extended_mvout(C + 0x840, 0xc00000c0, 64, 16);
  gemmini_extended_preload(0xffffffffffffffff, 0xc0000170, 16, 16, 16, 16);
  gemmini_extended_compute_accumulated(0x50, 0xffffffff, 16, 16, 16, 16);
  gemmini_extended_mvout(C + 0x1040, 0xc0000140, 64, 16);
  gemmini_extended_preload(0xffffffffffffffff, 0xc00001f0, 16, 16, 16, 16);
  gemmini_extended_compute_accumulated(0x70, 0xffffffff, 16, 16, 16, 16);
  gemmini_extended_mvout(C + 0x1840, 0xc00001c0, 64, 16);

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