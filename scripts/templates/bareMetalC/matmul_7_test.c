// See LICENSE for license details.

#include <stdint.h>
#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>
#ifndef BAREMETAL
#include <sys/mman.h>
#endif
#include "matmul_funcs.h"

#define BATCH_SIZE  256
#define INPUT_SIZE  128
#define HIDDEN_SIZE 16

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
  gemmini_extended_config_st(16, 0, ACC_SCALE_IDENTITY);
  gemmini_extended3_config_ld(128, MVIN_SCALE_IDENTITY, 0, 0);
  gemmini_extended3_config_ld(16, MVIN_SCALE_IDENTITY, 0, 1);
  gemmini_extended3_config_ld(64, MVIN_SCALE_IDENTITY, 0, 2);
  gemmini_extended_mvin(A + 0x0, 0x0, 64, 16);
  gemmini_extended_mvin2(B + 0x0, 0x3f80, 16, 16);
  gemmini_extended_preload(0x3f80, 0x80000000, 16, 16, 16, 16);
  gemmini_extended_compute_preloaded(0x0, 0xffffffff, 16, 16, 16, 16);
  gemmini_extended_mvin(A + 0x800, 0x80, 64, 16);
  gemmini_extended_preload(0xffffffffffffffff, 0x80000010, 16, 16, 16, 16);
  gemmini_extended_compute_accumulated(0x80, 0xffffffff, 16, 16, 16, 16);
  gemmini_extended_mvin(A + 0x1000, 0x100, 64, 16);
  gemmini_extended_preload(0xffffffffffffffff, 0x80000020, 16, 16, 16, 16);
  gemmini_extended_compute_accumulated(0x100, 0xffffffff, 16, 16, 16, 16);
  gemmini_extended_mvin(A + 0x1800, 0x180, 64, 16);
  gemmini_extended_preload(0xffffffffffffffff, 0x80000030, 16, 16, 16, 16);
  gemmini_extended_compute_accumulated(0x180, 0xffffffff, 16, 16, 16, 16);
  gemmini_extended_mvin(A + 0x2000, 0x200, 64, 16);
  gemmini_extended_preload(0xffffffffffffffff, 0x80000040, 16, 16, 16, 16);
  gemmini_extended_compute_accumulated(0x200, 0xffffffff, 16, 16, 16, 16);
  gemmini_extended_mvin(A + 0x2800, 0x280, 64, 16);
  gemmini_extended_preload(0xffffffffffffffff, 0x80000050, 16, 16, 16, 16);
  gemmini_extended_compute_accumulated(0x280, 0xffffffff, 16, 16, 16, 16);
  gemmini_extended_mvin(A + 0x3000, 0x300, 64, 16);
  gemmini_extended_preload(0xffffffffffffffff, 0x80000060, 16, 16, 16, 16);
  gemmini_extended_compute_accumulated(0x300, 0xffffffff, 16, 16, 16, 16);
  gemmini_extended_mvin(A + 0x3800, 0x380, 64, 16);
  gemmini_extended_preload(0xffffffffffffffff, 0x80000070, 16, 16, 16, 16);
  gemmini_extended_compute_accumulated(0x380, 0xffffffff, 16, 16, 16, 16);
  gemmini_extended_mvin(A + 0x4000, 0x400, 64, 16);
  gemmini_extended_preload(0xffffffffffffffff, 0x80000080, 16, 16, 16, 16);
  gemmini_extended_compute_accumulated(0x400, 0xffffffff, 16, 16, 16, 16);
  gemmini_extended_mvin(A + 0x4800, 0x480, 64, 16);
  gemmini_extended_preload(0xffffffffffffffff, 0x80000090, 16, 16, 16, 16);
  gemmini_extended_compute_accumulated(0x480, 0xffffffff, 16, 16, 16, 16);
  gemmini_extended_mvin(A + 0x5000, 0x500, 64, 16);
  gemmini_extended_preload(0xffffffffffffffff, 0x800000a0, 16, 16, 16, 16);
  gemmini_extended_compute_accumulated(0x500, 0xffffffff, 16, 16, 16, 16);
  gemmini_extended_mvin(A + 0x5800, 0x580, 64, 16);
  gemmini_extended_preload(0xffffffffffffffff, 0x800000b0, 16, 16, 16, 16);
  gemmini_extended_compute_accumulated(0x580, 0xffffffff, 16, 16, 16, 16);
  gemmini_extended_mvin(A + 0x6000, 0x600, 64, 16);
  gemmini_extended_preload(0xffffffffffffffff, 0x800000c0, 16, 16, 16, 16);
  gemmini_extended_compute_accumulated(0x600, 0xffffffff, 16, 16, 16, 16);
  gemmini_extended_mvin(A + 0x6800, 0x680, 64, 16);
  gemmini_extended_preload(0xffffffffffffffff, 0x800000d0, 16, 16, 16, 16);
  gemmini_extended_compute_accumulated(0x680, 0xffffffff, 16, 16, 16, 16);
  gemmini_extended_mvin(A + 0x7000, 0x700, 64, 16);
  gemmini_extended_preload(0xffffffffffffffff, 0x800000e0, 16, 16, 16, 16);
  gemmini_extended_compute_accumulated(0x700, 0xffffffff, 16, 16, 16, 16);
  gemmini_extended_mvin(A + 0x7800, 0x780, 64, 16);
  gemmini_extended_preload(0xffffffffffffffff, 0x800000f0, 16, 16, 16, 16);
  gemmini_extended_compute_accumulated(0x780, 0xffffffff, 16, 16, 16, 16);
  gemmini_extended_mvin2(B + 0x100, 0x3f90, 16, 16);
  gemmini_extended_preload(0x3f90, 0xc0000000, 16, 16, 16, 16);
  gemmini_extended_compute_preloaded(0x10, 0xffffffff, 16, 16, 16, 16);
  gemmini_extended_preload(0xffffffffffffffff, 0xc0000010, 16, 16, 16, 16);
  gemmini_extended_compute_accumulated(0x90, 0xffffffff, 16, 16, 16, 16);
  gemmini_extended_preload(0xffffffffffffffff, 0xc0000020, 16, 16, 16, 16);
  gemmini_extended_compute_accumulated(0x110, 0xffffffff, 16, 16, 16, 16);
  gemmini_extended_preload(0xffffffffffffffff, 0xc0000030, 16, 16, 16, 16);
  gemmini_extended_compute_accumulated(0x190, 0xffffffff, 16, 16, 16, 16);
  gemmini_extended_preload(0xffffffffffffffff, 0xc0000040, 16, 16, 16, 16);
  gemmini_extended_compute_accumulated(0x210, 0xffffffff, 16, 16, 16, 16);
  gemmini_extended_preload(0xffffffffffffffff, 0xc0000050, 16, 16, 16, 16);
  gemmini_extended_compute_accumulated(0x290, 0xffffffff, 16, 16, 16, 16);
  gemmini_extended_preload(0xffffffffffffffff, 0xc0000060, 16, 16, 16, 16);
  gemmini_extended_compute_accumulated(0x310, 0xffffffff, 16, 16, 16, 16);
  gemmini_extended_preload(0xffffffffffffffff, 0xc0000070, 16, 16, 16, 16);
  gemmini_extended_compute_accumulated(0x390, 0xffffffff, 16, 16, 16, 16);
  gemmini_extended_preload(0xffffffffffffffff, 0xc0000080, 16, 16, 16, 16);
  gemmini_extended_compute_accumulated(0x410, 0xffffffff, 16, 16, 16, 16);
  gemmini_extended_preload(0xffffffffffffffff, 0xc0000090, 16, 16, 16, 16);
  gemmini_extended_compute_accumulated(0x490, 0xffffffff, 16, 16, 16, 16);
  gemmini_extended_preload(0xffffffffffffffff, 0xc00000a0, 16, 16, 16, 16);
  gemmini_extended_compute_accumulated(0x510, 0xffffffff, 16, 16, 16, 16);
  gemmini_extended_preload(0xffffffffffffffff, 0xc00000b0, 16, 16, 16, 16);
  gemmini_extended_compute_accumulated(0x590, 0xffffffff, 16, 16, 16, 16);
  gemmini_extended_preload(0xffffffffffffffff, 0xc00000c0, 16, 16, 16, 16);
  gemmini_extended_compute_accumulated(0x610, 0xffffffff, 16, 16, 16, 16);
  gemmini_extended_preload(0xffffffffffffffff, 0xc00000d0, 16, 16, 16, 16);
  gemmini_extended_compute_accumulated(0x690, 0xffffffff, 16, 16, 16, 16);
  gemmini_extended_preload(0xffffffffffffffff, 0xc00000e0, 16, 16, 16, 16);
  gemmini_extended_compute_accumulated(0x710, 0xffffffff, 16, 16, 16, 16);
  gemmini_extended_preload(0xffffffffffffffff, 0xc00000f0, 16, 16, 16, 16);
  gemmini_extended_compute_accumulated(0x790, 0xffffffff, 16, 16, 16, 16);
  gemmini_extended_mvin2(B + 0x200, 0x3fa0, 16, 16);
  gemmini_extended_preload(0x3fa0, 0xc0000000, 16, 16, 16, 16);
  gemmini_extended_compute_preloaded(0x20, 0xffffffff, 16, 16, 16, 16);
  gemmini_extended_preload(0xffffffffffffffff, 0xc0000010, 16, 16, 16, 16);
  gemmini_extended_compute_accumulated(0xa0, 0xffffffff, 16, 16, 16, 16);
  gemmini_extended_preload(0xffffffffffffffff, 0xc0000020, 16, 16, 16, 16);
  gemmini_extended_compute_accumulated(0x120, 0xffffffff, 16, 16, 16, 16);
  gemmini_extended_preload(0xffffffffffffffff, 0xc0000030, 16, 16, 16, 16);
  gemmini_extended_compute_accumulated(0x1a0, 0xffffffff, 16, 16, 16, 16);
  gemmini_extended_preload(0xffffffffffffffff, 0xc0000040, 16, 16, 16, 16);
  gemmini_extended_compute_accumulated(0x220, 0xffffffff, 16, 16, 16, 16);
  gemmini_extended_preload(0xffffffffffffffff, 0xc0000050, 16, 16, 16, 16);
  gemmini_extended_compute_accumulated(0x2a0, 0xffffffff, 16, 16, 16, 16);
  gemmini_extended_preload(0xffffffffffffffff, 0xc0000060, 16, 16, 16, 16);
  gemmini_extended_compute_accumulated(0x320, 0xffffffff, 16, 16, 16, 16);
  gemmini_extended_preload(0xffffffffffffffff, 0xc0000070, 16, 16, 16, 16);
  gemmini_extended_compute_accumulated(0x3a0, 0xffffffff, 16, 16, 16, 16);
  gemmini_extended_preload(0xffffffffffffffff, 0xc0000080, 16, 16, 16, 16);
  gemmini_extended_compute_accumulated(0x420, 0xffffffff, 16, 16, 16, 16);
  gemmini_extended_preload(0xffffffffffffffff, 0xc0000090, 16, 16, 16, 16);
  gemmini_extended_compute_accumulated(0x4a0, 0xffffffff, 16, 16, 16, 16);
  gemmini_extended_preload(0xffffffffffffffff, 0xc00000a0, 16, 16, 16, 16);
  gemmini_extended_compute_accumulated(0x520, 0xffffffff, 16, 16, 16, 16);
  gemmini_extended_preload(0xffffffffffffffff, 0xc00000b0, 16, 16, 16, 16);
  gemmini_extended_compute_accumulated(0x5a0, 0xffffffff, 16, 16, 16, 16);
  gemmini_extended_preload(0xffffffffffffffff, 0xc00000c0, 16, 16, 16, 16);
  gemmini_extended_compute_accumulated(0x620, 0xffffffff, 16, 16, 16, 16);
  gemmini_extended_preload(0xffffffffffffffff, 0xc00000d0, 16, 16, 16, 16);
  gemmini_extended_compute_accumulated(0x6a0, 0xffffffff, 16, 16, 16, 16);
  gemmini_extended_preload(0xffffffffffffffff, 0xc00000e0, 16, 16, 16, 16);
  gemmini_extended_compute_accumulated(0x720, 0xffffffff, 16, 16, 16, 16);
  gemmini_extended_preload(0xffffffffffffffff, 0xc00000f0, 16, 16, 16, 16);
  gemmini_extended_compute_accumulated(0x7a0, 0xffffffff, 16, 16, 16, 16);
  gemmini_extended_mvin2(B + 0x300, 0x3fb0, 16, 16);
  gemmini_extended_preload(0x3fb0, 0xc0000000, 16, 16, 16, 16);
  gemmini_extended_compute_preloaded(0x30, 0xffffffff, 16, 16, 16, 16);
  gemmini_extended_preload(0xffffffffffffffff, 0xc0000010, 16, 16, 16, 16);
  gemmini_extended_compute_accumulated(0xb0, 0xffffffff, 16, 16, 16, 16);
  gemmini_extended_preload(0xffffffffffffffff, 0xc0000020, 16, 16, 16, 16);
  gemmini_extended_compute_accumulated(0x130, 0xffffffff, 16, 16, 16, 16);
  gemmini_extended_preload(0xffffffffffffffff, 0xc0000030, 16, 16, 16, 16);
  gemmini_extended_compute_accumulated(0x1b0, 0xffffffff, 16, 16, 16, 16);
  gemmini_extended_preload(0xffffffffffffffff, 0xc0000040, 16, 16, 16, 16);
  gemmini_extended_compute_accumulated(0x230, 0xffffffff, 16, 16, 16, 16);
  gemmini_extended_preload(0xffffffffffffffff, 0xc0000050, 16, 16, 16, 16);
  gemmini_extended_compute_accumulated(0x2b0, 0xffffffff, 16, 16, 16, 16);
  gemmini_extended_preload(0xffffffffffffffff, 0xc0000060, 16, 16, 16, 16);
  gemmini_extended_compute_accumulated(0x330, 0xffffffff, 16, 16, 16, 16);
  gemmini_extended_preload(0xffffffffffffffff, 0xc0000070, 16, 16, 16, 16);
  gemmini_extended_compute_accumulated(0x3b0, 0xffffffff, 16, 16, 16, 16);
  gemmini_extended_preload(0xffffffffffffffff, 0xc0000080, 16, 16, 16, 16);
  gemmini_extended_compute_accumulated(0x430, 0xffffffff, 16, 16, 16, 16);
  gemmini_extended_preload(0xffffffffffffffff, 0xc0000090, 16, 16, 16, 16);
  gemmini_extended_compute_accumulated(0x4b0, 0xffffffff, 16, 16, 16, 16);
  gemmini_extended_preload(0xffffffffffffffff, 0xc00000a0, 16, 16, 16, 16);
  gemmini_extended_compute_accumulated(0x530, 0xffffffff, 16, 16, 16, 16);
  gemmini_extended_preload(0xffffffffffffffff, 0xc00000b0, 16, 16, 16, 16);
  gemmini_extended_compute_accumulated(0x5b0, 0xffffffff, 16, 16, 16, 16);
  gemmini_extended_preload(0xffffffffffffffff, 0xc00000c0, 16, 16, 16, 16);
  gemmini_extended_compute_accumulated(0x630, 0xffffffff, 16, 16, 16, 16);
  gemmini_extended_preload(0xffffffffffffffff, 0xc00000d0, 16, 16, 16, 16);
  gemmini_extended_compute_accumulated(0x6b0, 0xffffffff, 16, 16, 16, 16);
  gemmini_extended_preload(0xffffffffffffffff, 0xc00000e0, 16, 16, 16, 16);
  gemmini_extended_compute_accumulated(0x730, 0xffffffff, 16, 16, 16, 16);
  gemmini_extended_preload(0xffffffffffffffff, 0xc00000f0, 16, 16, 16, 16);
  gemmini_extended_compute_accumulated(0x7b0, 0xffffffff, 16, 16, 16, 16);
  gemmini_extended_mvin(A + 0x40, 0x40, 64, 16);
  gemmini_extended_mvin2(B + 0x400, 0x3fc0, 16, 16);
  gemmini_extended_preload(0x3fc0, 0xc0000000, 16, 16, 16, 16);
  gemmini_extended_compute_preloaded(0x40, 0xffffffff, 16, 16, 16, 16);
  gemmini_extended_mvin(A + 0x840, 0xc0, 64, 16);
  gemmini_extended_preload(0xffffffffffffffff, 0xc0000010, 16, 16, 16, 16);
  gemmini_extended_compute_accumulated(0xc0, 0xffffffff, 16, 16, 16, 16);
  gemmini_extended_mvin(A + 0x1040, 0x140, 64, 16);
  gemmini_extended_preload(0xffffffffffffffff, 0xc0000020, 16, 16, 16, 16);
  gemmini_extended_compute_accumulated(0x140, 0xffffffff, 16, 16, 16, 16);
  gemmini_extended_mvin(A + 0x1840, 0x1c0, 64, 16);
  gemmini_extended_preload(0xffffffffffffffff, 0xc0000030, 16, 16, 16, 16);
  gemmini_extended_compute_accumulated(0x1c0, 0xffffffff, 16, 16, 16, 16);
  gemmini_extended_mvin(A + 0x2040, 0x240, 64, 16);
  gemmini_extended_preload(0xffffffffffffffff, 0xc0000040, 16, 16, 16, 16);
  gemmini_extended_compute_accumulated(0x240, 0xffffffff, 16, 16, 16, 16);
  gemmini_extended_mvin(A + 0x2840, 0x2c0, 64, 16);
  gemmini_extended_preload(0xffffffffffffffff, 0xc0000050, 16, 16, 16, 16);
  gemmini_extended_compute_accumulated(0x2c0, 0xffffffff, 16, 16, 16, 16);
  gemmini_extended_mvin(A + 0x3040, 0x340, 64, 16);
  gemmini_extended_preload(0xffffffffffffffff, 0xc0000060, 16, 16, 16, 16);
  gemmini_extended_compute_accumulated(0x340, 0xffffffff, 16, 16, 16, 16);
  gemmini_extended_mvin(A + 0x3840, 0x3c0, 64, 16);
  gemmini_extended_preload(0xffffffffffffffff, 0xc0000070, 16, 16, 16, 16);
  gemmini_extended_compute_accumulated(0x3c0, 0xffffffff, 16, 16, 16, 16);
  gemmini_extended_mvin(A + 0x4040, 0x440, 64, 16);
  gemmini_extended_preload(0xffffffffffffffff, 0xc0000080, 16, 16, 16, 16);
  gemmini_extended_compute_accumulated(0x440, 0xffffffff, 16, 16, 16, 16);
  gemmini_extended_mvin(A + 0x4840, 0x4c0, 64, 16);
  gemmini_extended_preload(0xffffffffffffffff, 0xc0000090, 16, 16, 16, 16);
  gemmini_extended_compute_accumulated(0x4c0, 0xffffffff, 16, 16, 16, 16);
  gemmini_extended_mvin(A + 0x5040, 0x540, 64, 16);
  gemmini_extended_preload(0xffffffffffffffff, 0xc00000a0, 16, 16, 16, 16);
  gemmini_extended_compute_accumulated(0x540, 0xffffffff, 16, 16, 16, 16);
  gemmini_extended_mvin(A + 0x5840, 0x5c0, 64, 16);
  gemmini_extended_preload(0xffffffffffffffff, 0xc00000b0, 16, 16, 16, 16);
  gemmini_extended_compute_accumulated(0x5c0, 0xffffffff, 16, 16, 16, 16);
  gemmini_extended_mvin(A + 0x6040, 0x640, 64, 16);
  gemmini_extended_preload(0xffffffffffffffff, 0xc00000c0, 16, 16, 16, 16);
  gemmini_extended_compute_accumulated(0x640, 0xffffffff, 16, 16, 16, 16);
  gemmini_extended_mvin(A + 0x6840, 0x6c0, 64, 16);
  gemmini_extended_preload(0xffffffffffffffff, 0xc00000d0, 16, 16, 16, 16);
  gemmini_extended_compute_accumulated(0x6c0, 0xffffffff, 16, 16, 16, 16);
  gemmini_extended_mvin(A + 0x7040, 0x740, 64, 16);
  gemmini_extended_preload(0xffffffffffffffff, 0xc00000e0, 16, 16, 16, 16);
  gemmini_extended_compute_accumulated(0x740, 0xffffffff, 16, 16, 16, 16);
  gemmini_extended_mvin(A + 0x7840, 0x7c0, 64, 16);
  gemmini_extended_preload(0xffffffffffffffff, 0xc00000f0, 16, 16, 16, 16);
  gemmini_extended_compute_accumulated(0x7c0, 0xffffffff, 16, 16, 16, 16);
  gemmini_extended_mvin2(B + 0x500, 0x3fd0, 16, 16);
  gemmini_extended_preload(0x3fd0, 0xc0000000, 16, 16, 16, 16);
  gemmini_extended_compute_preloaded(0x50, 0xffffffff, 16, 16, 16, 16);
  gemmini_extended_preload(0xffffffffffffffff, 0xc0000010, 16, 16, 16, 16);
  gemmini_extended_compute_accumulated(0xd0, 0xffffffff, 16, 16, 16, 16);
  gemmini_extended_preload(0xffffffffffffffff, 0xc0000020, 16, 16, 16, 16);
  gemmini_extended_compute_accumulated(0x150, 0xffffffff, 16, 16, 16, 16);
  gemmini_extended_preload(0xffffffffffffffff, 0xc0000030, 16, 16, 16, 16);
  gemmini_extended_compute_accumulated(0x1d0, 0xffffffff, 16, 16, 16, 16);
  gemmini_extended_preload(0xffffffffffffffff, 0xc0000040, 16, 16, 16, 16);
  gemmini_extended_compute_accumulated(0x250, 0xffffffff, 16, 16, 16, 16);
  gemmini_extended_preload(0xffffffffffffffff, 0xc0000050, 16, 16, 16, 16);
  gemmini_extended_compute_accumulated(0x2d0, 0xffffffff, 16, 16, 16, 16);
  gemmini_extended_preload(0xffffffffffffffff, 0xc0000060, 16, 16, 16, 16);
  gemmini_extended_compute_accumulated(0x350, 0xffffffff, 16, 16, 16, 16);
  gemmini_extended_preload(0xffffffffffffffff, 0xc0000070, 16, 16, 16, 16);
  gemmini_extended_compute_accumulated(0x3d0, 0xffffffff, 16, 16, 16, 16);
  gemmini_extended_preload(0xffffffffffffffff, 0xc0000080, 16, 16, 16, 16);
  gemmini_extended_compute_accumulated(0x450, 0xffffffff, 16, 16, 16, 16);
  gemmini_extended_preload(0xffffffffffffffff, 0xc0000090, 16, 16, 16, 16);
  gemmini_extended_compute_accumulated(0x4d0, 0xffffffff, 16, 16, 16, 16);
  gemmini_extended_preload(0xffffffffffffffff, 0xc00000a0, 16, 16, 16, 16);
  gemmini_extended_compute_accumulated(0x550, 0xffffffff, 16, 16, 16, 16);
  gemmini_extended_preload(0xffffffffffffffff, 0xc00000b0, 16, 16, 16, 16);
  gemmini_extended_compute_accumulated(0x5d0, 0xffffffff, 16, 16, 16, 16);
  gemmini_extended_preload(0xffffffffffffffff, 0xc00000c0, 16, 16, 16, 16);
  gemmini_extended_compute_accumulated(0x650, 0xffffffff, 16, 16, 16, 16);
  gemmini_extended_preload(0xffffffffffffffff, 0xc00000d0, 16, 16, 16, 16);
  gemmini_extended_compute_accumulated(0x6d0, 0xffffffff, 16, 16, 16, 16);
  gemmini_extended_preload(0xffffffffffffffff, 0xc00000e0, 16, 16, 16, 16);
  gemmini_extended_compute_accumulated(0x750, 0xffffffff, 16, 16, 16, 16);
  gemmini_extended_preload(0xffffffffffffffff, 0xc00000f0, 16, 16, 16, 16);
  gemmini_extended_compute_accumulated(0x7d0, 0xffffffff, 16, 16, 16, 16);
  gemmini_extended_mvin2(B + 0x600, 0x3fe0, 16, 16);
  gemmini_extended_preload(0x3fe0, 0xc0000000, 16, 16, 16, 16);
  gemmini_extended_compute_preloaded(0x60, 0xffffffff, 16, 16, 16, 16);
  gemmini_extended_preload(0xffffffffffffffff, 0xc0000010, 16, 16, 16, 16);
  gemmini_extended_compute_accumulated(0xe0, 0xffffffff, 16, 16, 16, 16);
  gemmini_extended_preload(0xffffffffffffffff, 0xc0000020, 16, 16, 16, 16);
  gemmini_extended_compute_accumulated(0x160, 0xffffffff, 16, 16, 16, 16);
  gemmini_extended_preload(0xffffffffffffffff, 0xc0000030, 16, 16, 16, 16);
  gemmini_extended_compute_accumulated(0x1e0, 0xffffffff, 16, 16, 16, 16);
  gemmini_extended_preload(0xffffffffffffffff, 0xc0000040, 16, 16, 16, 16);
  gemmini_extended_compute_accumulated(0x260, 0xffffffff, 16, 16, 16, 16);
  gemmini_extended_preload(0xffffffffffffffff, 0xc0000050, 16, 16, 16, 16);
  gemmini_extended_compute_accumulated(0x2e0, 0xffffffff, 16, 16, 16, 16);
  gemmini_extended_preload(0xffffffffffffffff, 0xc0000060, 16, 16, 16, 16);
  gemmini_extended_compute_accumulated(0x360, 0xffffffff, 16, 16, 16, 16);
  gemmini_extended_preload(0xffffffffffffffff, 0xc0000070, 16, 16, 16, 16);
  gemmini_extended_compute_accumulated(0x3e0, 0xffffffff, 16, 16, 16, 16);
  gemmini_extended_preload(0xffffffffffffffff, 0xc0000080, 16, 16, 16, 16);
  gemmini_extended_compute_accumulated(0x460, 0xffffffff, 16, 16, 16, 16);
  gemmini_extended_preload(0xffffffffffffffff, 0xc0000090, 16, 16, 16, 16);
  gemmini_extended_compute_accumulated(0x4e0, 0xffffffff, 16, 16, 16, 16);
  gemmini_extended_preload(0xffffffffffffffff, 0xc00000a0, 16, 16, 16, 16);
  gemmini_extended_compute_accumulated(0x560, 0xffffffff, 16, 16, 16, 16);
  gemmini_extended_preload(0xffffffffffffffff, 0xc00000b0, 16, 16, 16, 16);
  gemmini_extended_compute_accumulated(0x5e0, 0xffffffff, 16, 16, 16, 16);
  gemmini_extended_preload(0xffffffffffffffff, 0xc00000c0, 16, 16, 16, 16);
  gemmini_extended_compute_accumulated(0x660, 0xffffffff, 16, 16, 16, 16);
  gemmini_extended_preload(0xffffffffffffffff, 0xc00000d0, 16, 16, 16, 16);
  gemmini_extended_compute_accumulated(0x6e0, 0xffffffff, 16, 16, 16, 16);
  gemmini_extended_preload(0xffffffffffffffff, 0xc00000e0, 16, 16, 16, 16);
  gemmini_extended_compute_accumulated(0x760, 0xffffffff, 16, 16, 16, 16);
  gemmini_extended_preload(0xffffffffffffffff, 0xc00000f0, 16, 16, 16, 16);
  gemmini_extended_compute_accumulated(0x7e0, 0xffffffff, 16, 16, 16, 16);
  gemmini_extended_mvin2(B + 0x700, 0x3ff0, 16, 16);
  gemmini_extended_preload(0x3ff0, 0xc0000000, 16, 16, 16, 16);
  gemmini_extended_compute_preloaded(0x70, 0xffffffff, 16, 16, 16, 16);
  gemmini_extended_mvout(C + 0x0, 0xc0000000, 16, 16);
  gemmini_extended_preload(0xffffffffffffffff, 0xc0000010, 16, 16, 16, 16);
  gemmini_extended_compute_accumulated(0xf0, 0xffffffff, 16, 16, 16, 16);
  gemmini_extended_mvout(C + 0x100, 0xc0000010, 16, 16);
  gemmini_extended_preload(0xffffffffffffffff, 0xc0000020, 16, 16, 16, 16);
  gemmini_extended_compute_accumulated(0x170, 0xffffffff, 16, 16, 16, 16);
  gemmini_extended_mvout(C + 0x200, 0xc0000020, 16, 16);
  gemmini_extended_preload(0xffffffffffffffff, 0xc0000030, 16, 16, 16, 16);
  gemmini_extended_compute_accumulated(0x1f0, 0xffffffff, 16, 16, 16, 16);
  gemmini_extended_mvout(C + 0x300, 0xc0000030, 16, 16);
  gemmini_extended_preload(0xffffffffffffffff, 0xc0000040, 16, 16, 16, 16);
  gemmini_extended_compute_accumulated(0x270, 0xffffffff, 16, 16, 16, 16);
  gemmini_extended_mvout(C + 0x400, 0xc0000040, 16, 16);
  gemmini_extended_preload(0xffffffffffffffff, 0xc0000050, 16, 16, 16, 16);
  gemmini_extended_compute_accumulated(0x2f0, 0xffffffff, 16, 16, 16, 16);
  gemmini_extended_mvout(C + 0x500, 0xc0000050, 16, 16);
  gemmini_extended_preload(0xffffffffffffffff, 0xc0000060, 16, 16, 16, 16);
  gemmini_extended_compute_accumulated(0x370, 0xffffffff, 16, 16, 16, 16);
  gemmini_extended_mvout(C + 0x600, 0xc0000060, 16, 16);
  gemmini_extended_preload(0xffffffffffffffff, 0xc0000070, 16, 16, 16, 16);
  gemmini_extended_compute_accumulated(0x3f0, 0xffffffff, 16, 16, 16, 16);
  gemmini_extended_mvout(C + 0x700, 0xc0000070, 16, 16);
  gemmini_extended_preload(0xffffffffffffffff, 0xc0000080, 16, 16, 16, 16);
  gemmini_extended_compute_accumulated(0x470, 0xffffffff, 16, 16, 16, 16);
  gemmini_extended_mvout(C + 0x800, 0xc0000080, 16, 16);
  gemmini_extended_preload(0xffffffffffffffff, 0xc0000090, 16, 16, 16, 16);
  gemmini_extended_compute_accumulated(0x4f0, 0xffffffff, 16, 16, 16, 16);
  gemmini_extended_mvout(C + 0x900, 0xc0000090, 16, 16);
  gemmini_extended_preload(0xffffffffffffffff, 0xc00000a0, 16, 16, 16, 16);
  gemmini_extended_compute_accumulated(0x570, 0xffffffff, 16, 16, 16, 16);
  gemmini_extended_mvout(C + 0xa00, 0xc00000a0, 16, 16);
  gemmini_extended_preload(0xffffffffffffffff, 0xc00000b0, 16, 16, 16, 16);
  gemmini_extended_compute_accumulated(0x5f0, 0xffffffff, 16, 16, 16, 16);
  gemmini_extended_mvout(C + 0xb00, 0xc00000b0, 16, 16);
  gemmini_extended_preload(0xffffffffffffffff, 0xc00000c0, 16, 16, 16, 16);
  gemmini_extended_compute_accumulated(0x670, 0xffffffff, 16, 16, 16, 16);
  gemmini_extended_mvout(C + 0xc00, 0xc00000c0, 16, 16);
  gemmini_extended_preload(0xffffffffffffffff, 0xc00000d0, 16, 16, 16, 16);
  gemmini_extended_compute_accumulated(0x6f0, 0xffffffff, 16, 16, 16, 16);
  gemmini_extended_mvout(C + 0xd00, 0xc00000d0, 16, 16);
  gemmini_extended_preload(0xffffffffffffffff, 0xc00000e0, 16, 16, 16, 16);
  gemmini_extended_compute_accumulated(0x770, 0xffffffff, 16, 16, 16, 16);
  gemmini_extended_mvout(C + 0xe00, 0xc00000e0, 16, 16);
  gemmini_extended_preload(0xffffffffffffffff, 0xc00000f0, 16, 16, 16, 16);
  gemmini_extended_compute_accumulated(0x7f0, 0xffffffff, 16, 16, 16, 16);
  gemmini_extended_mvout(C + 0xf00, 0xc00000f0, 16, 16);

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