// See LICENSE for license details.

#include <stdint.h>
#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>
#ifndef BAREMETAL
#include <sys/mman.h>
#endif
#include "matmul_funcs.h"

#define BATCH_SIZE  16
#define INPUT_SIZE  32
#define HIDDEN_SIZE 1024

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
  gemmini_extended_config_st(1024, 0, ACC_SCALE_IDENTITY);
  gemmini_extended3_config_ld(32, MVIN_SCALE_IDENTITY, 0, 0);
  gemmini_extended3_config_ld(1024, MVIN_SCALE_IDENTITY, 0, 1);
  gemmini_extended3_config_ld(4096, MVIN_SCALE_IDENTITY, 0, 2);
  gemmini_extended_mvin(A + 0x0, 0x0, 32, 16);
  gemmini_extended_mvin2(B + 0x0, 0x3800, 64, 16);
  gemmini_extended_preload(0x3800, 0x80000000, 16, 16, 16, 16);
  gemmini_extended_compute_preloaded(0x0, 0xffffffff, 16, 16, 16, 16);
  gemmini_extended_preload(0x3810, 0x80000010, 16, 16, 16, 16);
  gemmini_extended_compute_preloaded(0x0, 0xffffffff, 16, 16, 16, 16);
  gemmini_extended_preload(0x3820, 0x80000020, 16, 16, 16, 16);
  gemmini_extended_compute_preloaded(0x0, 0xffffffff, 16, 16, 16, 16);
  gemmini_extended_preload(0x3830, 0x80000030, 16, 16, 16, 16);
  gemmini_extended_compute_preloaded(0x0, 0xffffffff, 16, 16, 16, 16);
  gemmini_extended_mvin2(B + 0x40, 0x3840, 64, 16);
  gemmini_extended_preload(0x3840, 0x80000040, 16, 16, 16, 16);
  gemmini_extended_compute_preloaded(0x0, 0xffffffff, 16, 16, 16, 16);
  gemmini_extended_preload(0x3850, 0x80000050, 16, 16, 16, 16);
  gemmini_extended_compute_preloaded(0x0, 0xffffffff, 16, 16, 16, 16);
  gemmini_extended_preload(0x3860, 0x80000060, 16, 16, 16, 16);
  gemmini_extended_compute_preloaded(0x0, 0xffffffff, 16, 16, 16, 16);
  gemmini_extended_preload(0x3870, 0x80000070, 16, 16, 16, 16);
  gemmini_extended_compute_preloaded(0x0, 0xffffffff, 16, 16, 16, 16);
  gemmini_extended_mvin2(B + 0x80, 0x3880, 64, 16);
  gemmini_extended_preload(0x3880, 0x80000080, 16, 16, 16, 16);
  gemmini_extended_compute_preloaded(0x0, 0xffffffff, 16, 16, 16, 16);
  gemmini_extended_preload(0x3890, 0x80000090, 16, 16, 16, 16);
  gemmini_extended_compute_preloaded(0x0, 0xffffffff, 16, 16, 16, 16);
  gemmini_extended_preload(0x38a0, 0x800000a0, 16, 16, 16, 16);
  gemmini_extended_compute_preloaded(0x0, 0xffffffff, 16, 16, 16, 16);
  gemmini_extended_preload(0x38b0, 0x800000b0, 16, 16, 16, 16);
  gemmini_extended_compute_preloaded(0x0, 0xffffffff, 16, 16, 16, 16);
  gemmini_extended_mvin2(B + 0xc0, 0x38c0, 64, 16);
  gemmini_extended_preload(0x38c0, 0x800000c0, 16, 16, 16, 16);
  gemmini_extended_compute_preloaded(0x0, 0xffffffff, 16, 16, 16, 16);
  gemmini_extended_preload(0x38d0, 0x800000d0, 16, 16, 16, 16);
  gemmini_extended_compute_preloaded(0x0, 0xffffffff, 16, 16, 16, 16);
  gemmini_extended_preload(0x38e0, 0x800000e0, 16, 16, 16, 16);
  gemmini_extended_compute_preloaded(0x0, 0xffffffff, 16, 16, 16, 16);
  gemmini_extended_preload(0x38f0, 0x800000f0, 16, 16, 16, 16);
  gemmini_extended_compute_preloaded(0x0, 0xffffffff, 16, 16, 16, 16);
  gemmini_extended_mvin2(B + 0x100, 0x3900, 64, 16);
  gemmini_extended_preload(0x3900, 0x80000100, 16, 16, 16, 16);
  gemmini_extended_compute_preloaded(0x0, 0xffffffff, 16, 16, 16, 16);
  gemmini_extended_preload(0x3910, 0x80000110, 16, 16, 16, 16);
  gemmini_extended_compute_preloaded(0x0, 0xffffffff, 16, 16, 16, 16);
  gemmini_extended_preload(0x3920, 0x80000120, 16, 16, 16, 16);
  gemmini_extended_compute_preloaded(0x0, 0xffffffff, 16, 16, 16, 16);
  gemmini_extended_preload(0x3930, 0x80000130, 16, 16, 16, 16);
  gemmini_extended_compute_preloaded(0x0, 0xffffffff, 16, 16, 16, 16);
  gemmini_extended_mvin2(B + 0x140, 0x3940, 64, 16);
  gemmini_extended_preload(0x3940, 0x80000140, 16, 16, 16, 16);
  gemmini_extended_compute_preloaded(0x0, 0xffffffff, 16, 16, 16, 16);
  gemmini_extended_preload(0x3950, 0x80000150, 16, 16, 16, 16);
  gemmini_extended_compute_preloaded(0x0, 0xffffffff, 16, 16, 16, 16);
  gemmini_extended_preload(0x3960, 0x80000160, 16, 16, 16, 16);
  gemmini_extended_compute_preloaded(0x0, 0xffffffff, 16, 16, 16, 16);
  gemmini_extended_preload(0x3970, 0x80000170, 16, 16, 16, 16);
  gemmini_extended_compute_preloaded(0x0, 0xffffffff, 16, 16, 16, 16);
  gemmini_extended_mvin2(B + 0x180, 0x3980, 64, 16);
  gemmini_extended_preload(0x3980, 0x80000180, 16, 16, 16, 16);
  gemmini_extended_compute_preloaded(0x0, 0xffffffff, 16, 16, 16, 16);
  gemmini_extended_preload(0x3990, 0x80000190, 16, 16, 16, 16);
  gemmini_extended_compute_preloaded(0x0, 0xffffffff, 16, 16, 16, 16);
  gemmini_extended_preload(0x39a0, 0x800001a0, 16, 16, 16, 16);
  gemmini_extended_compute_preloaded(0x0, 0xffffffff, 16, 16, 16, 16);
  gemmini_extended_preload(0x39b0, 0x800001b0, 16, 16, 16, 16);
  gemmini_extended_compute_preloaded(0x0, 0xffffffff, 16, 16, 16, 16);
  gemmini_extended_mvin2(B + 0x1c0, 0x39c0, 64, 16);
  gemmini_extended_preload(0x39c0, 0x800001c0, 16, 16, 16, 16);
  gemmini_extended_compute_preloaded(0x0, 0xffffffff, 16, 16, 16, 16);
  gemmini_extended_preload(0x39d0, 0x800001d0, 16, 16, 16, 16);
  gemmini_extended_compute_preloaded(0x0, 0xffffffff, 16, 16, 16, 16);
  gemmini_extended_preload(0x39e0, 0x800001e0, 16, 16, 16, 16);
  gemmini_extended_compute_preloaded(0x0, 0xffffffff, 16, 16, 16, 16);
  gemmini_extended_preload(0x39f0, 0x800001f0, 16, 16, 16, 16);
  gemmini_extended_compute_preloaded(0x0, 0xffffffff, 16, 16, 16, 16);
  gemmini_extended_mvin2(B + 0x200, 0x3a00, 64, 16);
  gemmini_extended_preload(0x3a00, 0x80000200, 16, 16, 16, 16);
  gemmini_extended_compute_preloaded(0x0, 0xffffffff, 16, 16, 16, 16);
  gemmini_extended_preload(0x3a10, 0x80000210, 16, 16, 16, 16);
  gemmini_extended_compute_preloaded(0x0, 0xffffffff, 16, 16, 16, 16);
  gemmini_extended_preload(0x3a20, 0x80000220, 16, 16, 16, 16);
  gemmini_extended_compute_preloaded(0x0, 0xffffffff, 16, 16, 16, 16);
  gemmini_extended_preload(0x3a30, 0x80000230, 16, 16, 16, 16);
  gemmini_extended_compute_preloaded(0x0, 0xffffffff, 16, 16, 16, 16);
  gemmini_extended_mvin2(B + 0x240, 0x3a40, 64, 16);
  gemmini_extended_preload(0x3a40, 0x80000240, 16, 16, 16, 16);
  gemmini_extended_compute_preloaded(0x0, 0xffffffff, 16, 16, 16, 16);
  gemmini_extended_preload(0x3a50, 0x80000250, 16, 16, 16, 16);
  gemmini_extended_compute_preloaded(0x0, 0xffffffff, 16, 16, 16, 16);
  gemmini_extended_preload(0x3a60, 0x80000260, 16, 16, 16, 16);
  gemmini_extended_compute_preloaded(0x0, 0xffffffff, 16, 16, 16, 16);
  gemmini_extended_preload(0x3a70, 0x80000270, 16, 16, 16, 16);
  gemmini_extended_compute_preloaded(0x0, 0xffffffff, 16, 16, 16, 16);
  gemmini_extended_mvin2(B + 0x280, 0x3a80, 64, 16);
  gemmini_extended_preload(0x3a80, 0x80000280, 16, 16, 16, 16);
  gemmini_extended_compute_preloaded(0x0, 0xffffffff, 16, 16, 16, 16);
  gemmini_extended_preload(0x3a90, 0x80000290, 16, 16, 16, 16);
  gemmini_extended_compute_preloaded(0x0, 0xffffffff, 16, 16, 16, 16);
  gemmini_extended_preload(0x3aa0, 0x800002a0, 16, 16, 16, 16);
  gemmini_extended_compute_preloaded(0x0, 0xffffffff, 16, 16, 16, 16);
  gemmini_extended_preload(0x3ab0, 0x800002b0, 16, 16, 16, 16);
  gemmini_extended_compute_preloaded(0x0, 0xffffffff, 16, 16, 16, 16);
  gemmini_extended_mvin2(B + 0x2c0, 0x3ac0, 64, 16);
  gemmini_extended_preload(0x3ac0, 0x800002c0, 16, 16, 16, 16);
  gemmini_extended_compute_preloaded(0x0, 0xffffffff, 16, 16, 16, 16);
  gemmini_extended_preload(0x3ad0, 0x800002d0, 16, 16, 16, 16);
  gemmini_extended_compute_preloaded(0x0, 0xffffffff, 16, 16, 16, 16);
  gemmini_extended_preload(0x3ae0, 0x800002e0, 16, 16, 16, 16);
  gemmini_extended_compute_preloaded(0x0, 0xffffffff, 16, 16, 16, 16);
  gemmini_extended_preload(0x3af0, 0x800002f0, 16, 16, 16, 16);
  gemmini_extended_compute_preloaded(0x0, 0xffffffff, 16, 16, 16, 16);
  gemmini_extended_mvin2(B + 0x300, 0x3b00, 64, 16);
  gemmini_extended_preload(0x3b00, 0x80000300, 16, 16, 16, 16);
  gemmini_extended_compute_preloaded(0x0, 0xffffffff, 16, 16, 16, 16);
  gemmini_extended_preload(0x3b10, 0x80000310, 16, 16, 16, 16);
  gemmini_extended_compute_preloaded(0x0, 0xffffffff, 16, 16, 16, 16);
  gemmini_extended_preload(0x3b20, 0x80000320, 16, 16, 16, 16);
  gemmini_extended_compute_preloaded(0x0, 0xffffffff, 16, 16, 16, 16);
  gemmini_extended_preload(0x3b30, 0x80000330, 16, 16, 16, 16);
  gemmini_extended_compute_preloaded(0x0, 0xffffffff, 16, 16, 16, 16);
  gemmini_extended_mvin2(B + 0x340, 0x3b40, 64, 16);
  gemmini_extended_preload(0x3b40, 0x80000340, 16, 16, 16, 16);
  gemmini_extended_compute_preloaded(0x0, 0xffffffff, 16, 16, 16, 16);
  gemmini_extended_preload(0x3b50, 0x80000350, 16, 16, 16, 16);
  gemmini_extended_compute_preloaded(0x0, 0xffffffff, 16, 16, 16, 16);
  gemmini_extended_preload(0x3b60, 0x80000360, 16, 16, 16, 16);
  gemmini_extended_compute_preloaded(0x0, 0xffffffff, 16, 16, 16, 16);
  gemmini_extended_preload(0x3b70, 0x80000370, 16, 16, 16, 16);
  gemmini_extended_compute_preloaded(0x0, 0xffffffff, 16, 16, 16, 16);
  gemmini_extended_mvin2(B + 0x380, 0x3b80, 64, 16);
  gemmini_extended_preload(0x3b80, 0x80000380, 16, 16, 16, 16);
  gemmini_extended_compute_preloaded(0x0, 0xffffffff, 16, 16, 16, 16);
  gemmini_extended_preload(0x3b90, 0x80000390, 16, 16, 16, 16);
  gemmini_extended_compute_preloaded(0x0, 0xffffffff, 16, 16, 16, 16);
  gemmini_extended_preload(0x3ba0, 0x800003a0, 16, 16, 16, 16);
  gemmini_extended_compute_preloaded(0x0, 0xffffffff, 16, 16, 16, 16);
  gemmini_extended_preload(0x3bb0, 0x800003b0, 16, 16, 16, 16);
  gemmini_extended_compute_preloaded(0x0, 0xffffffff, 16, 16, 16, 16);
  gemmini_extended_mvin2(B + 0x3c0, 0x3bc0, 64, 16);
  gemmini_extended_preload(0x3bc0, 0x800003c0, 16, 16, 16, 16);
  gemmini_extended_compute_preloaded(0x0, 0xffffffff, 16, 16, 16, 16);
  gemmini_extended_preload(0x3bd0, 0x800003d0, 16, 16, 16, 16);
  gemmini_extended_compute_preloaded(0x0, 0xffffffff, 16, 16, 16, 16);
  gemmini_extended_preload(0x3be0, 0x800003e0, 16, 16, 16, 16);
  gemmini_extended_compute_preloaded(0x0, 0xffffffff, 16, 16, 16, 16);
  gemmini_extended_preload(0x3bf0, 0x800003f0, 16, 16, 16, 16);
  gemmini_extended_compute_preloaded(0x0, 0xffffffff, 16, 16, 16, 16);
  gemmini_extended_mvin2(B + 0x4000, 0x3c00, 64, 16);
  gemmini_extended_preload(0x3c00, 0xc0000000, 16, 16, 16, 16);
  gemmini_extended_compute_preloaded(0x10, 0xffffffff, 16, 16, 16, 16);
  gemmini_extended_preload(0x3c10, 0xc0000010, 16, 16, 16, 16);
  gemmini_extended_compute_preloaded(0x10, 0xffffffff, 16, 16, 16, 16);
  gemmini_extended_preload(0x3c20, 0xc0000020, 16, 16, 16, 16);
  gemmini_extended_compute_preloaded(0x10, 0xffffffff, 16, 16, 16, 16);
  gemmini_extended_preload(0x3c30, 0xc0000030, 16, 16, 16, 16);
  gemmini_extended_compute_preloaded(0x10, 0xffffffff, 16, 16, 16, 16);
  gemmini_extended_mvout(C + 0x0, 0xc0000000, 64, 16);
  gemmini_extended_mvin2(B + 0x4040, 0x3c40, 64, 16);
  gemmini_extended_preload(0x3c40, 0xc0000040, 16, 16, 16, 16);
  gemmini_extended_compute_preloaded(0x10, 0xffffffff, 16, 16, 16, 16);
  gemmini_extended_preload(0x3c50, 0xc0000050, 16, 16, 16, 16);
  gemmini_extended_compute_preloaded(0x10, 0xffffffff, 16, 16, 16, 16);
  gemmini_extended_preload(0x3c60, 0xc0000060, 16, 16, 16, 16);
  gemmini_extended_compute_preloaded(0x10, 0xffffffff, 16, 16, 16, 16);
  gemmini_extended_preload(0x3c70, 0xc0000070, 16, 16, 16, 16);
  gemmini_extended_compute_preloaded(0x10, 0xffffffff, 16, 16, 16, 16);
  gemmini_extended_mvout(C + 0x40, 0xc0000040, 64, 16);
  gemmini_extended_mvin2(B + 0x4080, 0x3c80, 64, 16);
  gemmini_extended_preload(0x3c80, 0xc0000080, 16, 16, 16, 16);
  gemmini_extended_compute_preloaded(0x10, 0xffffffff, 16, 16, 16, 16);
  gemmini_extended_preload(0x3c90, 0xc0000090, 16, 16, 16, 16);
  gemmini_extended_compute_preloaded(0x10, 0xffffffff, 16, 16, 16, 16);
  gemmini_extended_preload(0x3ca0, 0xc00000a0, 16, 16, 16, 16);
  gemmini_extended_compute_preloaded(0x10, 0xffffffff, 16, 16, 16, 16);
  gemmini_extended_preload(0x3cb0, 0xc00000b0, 16, 16, 16, 16);
  gemmini_extended_compute_preloaded(0x10, 0xffffffff, 16, 16, 16, 16);
  gemmini_extended_mvout(C + 0x80, 0xc0000080, 64, 16);
  gemmini_extended_mvin2(B + 0x40c0, 0x3cc0, 64, 16);
  gemmini_extended_preload(0x3cc0, 0xc00000c0, 16, 16, 16, 16);
  gemmini_extended_compute_preloaded(0x10, 0xffffffff, 16, 16, 16, 16);
  gemmini_extended_preload(0x3cd0, 0xc00000d0, 16, 16, 16, 16);
  gemmini_extended_compute_preloaded(0x10, 0xffffffff, 16, 16, 16, 16);
  gemmini_extended_preload(0x3ce0, 0xc00000e0, 16, 16, 16, 16);
  gemmini_extended_compute_preloaded(0x10, 0xffffffff, 16, 16, 16, 16);
  gemmini_extended_preload(0x3cf0, 0xc00000f0, 16, 16, 16, 16);
  gemmini_extended_compute_preloaded(0x10, 0xffffffff, 16, 16, 16, 16);
  gemmini_extended_mvout(C + 0xc0, 0xc00000c0, 64, 16);
  gemmini_extended_mvin2(B + 0x4100, 0x3d00, 64, 16);
  gemmini_extended_preload(0x3d00, 0xc0000100, 16, 16, 16, 16);
  gemmini_extended_compute_preloaded(0x10, 0xffffffff, 16, 16, 16, 16);
  gemmini_extended_preload(0x3d10, 0xc0000110, 16, 16, 16, 16);
  gemmini_extended_compute_preloaded(0x10, 0xffffffff, 16, 16, 16, 16);
  gemmini_extended_preload(0x3d20, 0xc0000120, 16, 16, 16, 16);
  gemmini_extended_compute_preloaded(0x10, 0xffffffff, 16, 16, 16, 16);
  gemmini_extended_preload(0x3d30, 0xc0000130, 16, 16, 16, 16);
  gemmini_extended_compute_preloaded(0x10, 0xffffffff, 16, 16, 16, 16);
  gemmini_extended_mvout(C + 0x100, 0xc0000100, 64, 16);
  gemmini_extended_mvin2(B + 0x4140, 0x3d40, 64, 16);
  gemmini_extended_preload(0x3d40, 0xc0000140, 16, 16, 16, 16);
  gemmini_extended_compute_preloaded(0x10, 0xffffffff, 16, 16, 16, 16);
  gemmini_extended_preload(0x3d50, 0xc0000150, 16, 16, 16, 16);
  gemmini_extended_compute_preloaded(0x10, 0xffffffff, 16, 16, 16, 16);
  gemmini_extended_preload(0x3d60, 0xc0000160, 16, 16, 16, 16);
  gemmini_extended_compute_preloaded(0x10, 0xffffffff, 16, 16, 16, 16);
  gemmini_extended_preload(0x3d70, 0xc0000170, 16, 16, 16, 16);
  gemmini_extended_compute_preloaded(0x10, 0xffffffff, 16, 16, 16, 16);
  gemmini_extended_mvout(C + 0x140, 0xc0000140, 64, 16);
  gemmini_extended_mvin2(B + 0x4180, 0x3d80, 64, 16);
  gemmini_extended_preload(0x3d80, 0xc0000180, 16, 16, 16, 16);
  gemmini_extended_compute_preloaded(0x10, 0xffffffff, 16, 16, 16, 16);
  gemmini_extended_preload(0x3d90, 0xc0000190, 16, 16, 16, 16);
  gemmini_extended_compute_preloaded(0x10, 0xffffffff, 16, 16, 16, 16);
  gemmini_extended_preload(0x3da0, 0xc00001a0, 16, 16, 16, 16);
  gemmini_extended_compute_preloaded(0x10, 0xffffffff, 16, 16, 16, 16);
  gemmini_extended_preload(0x3db0, 0xc00001b0, 16, 16, 16, 16);
  gemmini_extended_compute_preloaded(0x10, 0xffffffff, 16, 16, 16, 16);
  gemmini_extended_mvout(C + 0x180, 0xc0000180, 64, 16);
  gemmini_extended_mvin2(B + 0x41c0, 0x3dc0, 64, 16);
  gemmini_extended_preload(0x3dc0, 0xc00001c0, 16, 16, 16, 16);
  gemmini_extended_compute_preloaded(0x10, 0xffffffff, 16, 16, 16, 16);
  gemmini_extended_preload(0x3dd0, 0xc00001d0, 16, 16, 16, 16);
  gemmini_extended_compute_preloaded(0x10, 0xffffffff, 16, 16, 16, 16);
  gemmini_extended_preload(0x3de0, 0xc00001e0, 16, 16, 16, 16);
  gemmini_extended_compute_preloaded(0x10, 0xffffffff, 16, 16, 16, 16);
  gemmini_extended_preload(0x3df0, 0xc00001f0, 16, 16, 16, 16);
  gemmini_extended_compute_preloaded(0x10, 0xffffffff, 16, 16, 16, 16);
  gemmini_extended_mvout(C + 0x1c0, 0xc00001c0, 64, 16);
  gemmini_extended_mvin2(B + 0x4200, 0x3e00, 64, 16);
  gemmini_extended_preload(0x3e00, 0xc0000200, 16, 16, 16, 16);
  gemmini_extended_compute_preloaded(0x10, 0xffffffff, 16, 16, 16, 16);
  gemmini_extended_preload(0x3e10, 0xc0000210, 16, 16, 16, 16);
  gemmini_extended_compute_preloaded(0x10, 0xffffffff, 16, 16, 16, 16);
  gemmini_extended_preload(0x3e20, 0xc0000220, 16, 16, 16, 16);
  gemmini_extended_compute_preloaded(0x10, 0xffffffff, 16, 16, 16, 16);
  gemmini_extended_preload(0x3e30, 0xc0000230, 16, 16, 16, 16);
  gemmini_extended_compute_preloaded(0x10, 0xffffffff, 16, 16, 16, 16);
  gemmini_extended_mvout(C + 0x200, 0xc0000200, 64, 16);
  gemmini_extended_mvin2(B + 0x4240, 0x3e40, 64, 16);
  gemmini_extended_preload(0x3e40, 0xc0000240, 16, 16, 16, 16);
  gemmini_extended_compute_preloaded(0x10, 0xffffffff, 16, 16, 16, 16);
  gemmini_extended_preload(0x3e50, 0xc0000250, 16, 16, 16, 16);
  gemmini_extended_compute_preloaded(0x10, 0xffffffff, 16, 16, 16, 16);
  gemmini_extended_preload(0x3e60, 0xc0000260, 16, 16, 16, 16);
  gemmini_extended_compute_preloaded(0x10, 0xffffffff, 16, 16, 16, 16);
  gemmini_extended_preload(0x3e70, 0xc0000270, 16, 16, 16, 16);
  gemmini_extended_compute_preloaded(0x10, 0xffffffff, 16, 16, 16, 16);
  gemmini_extended_mvout(C + 0x240, 0xc0000240, 64, 16);
  gemmini_extended_mvin2(B + 0x4280, 0x3e80, 64, 16);
  gemmini_extended_preload(0x3e80, 0xc0000280, 16, 16, 16, 16);
  gemmini_extended_compute_preloaded(0x10, 0xffffffff, 16, 16, 16, 16);
  gemmini_extended_preload(0x3e90, 0xc0000290, 16, 16, 16, 16);
  gemmini_extended_compute_preloaded(0x10, 0xffffffff, 16, 16, 16, 16);
  gemmini_extended_preload(0x3ea0, 0xc00002a0, 16, 16, 16, 16);
  gemmini_extended_compute_preloaded(0x10, 0xffffffff, 16, 16, 16, 16);
  gemmini_extended_preload(0x3eb0, 0xc00002b0, 16, 16, 16, 16);
  gemmini_extended_compute_preloaded(0x10, 0xffffffff, 16, 16, 16, 16);
  gemmini_extended_mvout(C + 0x280, 0xc0000280, 64, 16);
  gemmini_extended_mvin2(B + 0x42c0, 0x3ec0, 64, 16);
  gemmini_extended_preload(0x3ec0, 0xc00002c0, 16, 16, 16, 16);
  gemmini_extended_compute_preloaded(0x10, 0xffffffff, 16, 16, 16, 16);
  gemmini_extended_preload(0x3ed0, 0xc00002d0, 16, 16, 16, 16);
  gemmini_extended_compute_preloaded(0x10, 0xffffffff, 16, 16, 16, 16);
  gemmini_extended_preload(0x3ee0, 0xc00002e0, 16, 16, 16, 16);
  gemmini_extended_compute_preloaded(0x10, 0xffffffff, 16, 16, 16, 16);
  gemmini_extended_preload(0x3ef0, 0xc00002f0, 16, 16, 16, 16);
  gemmini_extended_compute_preloaded(0x10, 0xffffffff, 16, 16, 16, 16);
  gemmini_extended_mvout(C + 0x2c0, 0xc00002c0, 64, 16);
  gemmini_extended_mvin2(B + 0x4300, 0x3f00, 64, 16);
  gemmini_extended_preload(0x3f00, 0xc0000300, 16, 16, 16, 16);
  gemmini_extended_compute_preloaded(0x10, 0xffffffff, 16, 16, 16, 16);
  gemmini_extended_preload(0x3f10, 0xc0000310, 16, 16, 16, 16);
  gemmini_extended_compute_preloaded(0x10, 0xffffffff, 16, 16, 16, 16);
  gemmini_extended_preload(0x3f20, 0xc0000320, 16, 16, 16, 16);
  gemmini_extended_compute_preloaded(0x10, 0xffffffff, 16, 16, 16, 16);
  gemmini_extended_preload(0x3f30, 0xc0000330, 16, 16, 16, 16);
  gemmini_extended_compute_preloaded(0x10, 0xffffffff, 16, 16, 16, 16);
  gemmini_extended_mvout(C + 0x300, 0xc0000300, 64, 16);
  gemmini_extended_mvin2(B + 0x4340, 0x3f40, 64, 16);
  gemmini_extended_preload(0x3f40, 0xc0000340, 16, 16, 16, 16);
  gemmini_extended_compute_preloaded(0x10, 0xffffffff, 16, 16, 16, 16);
  gemmini_extended_preload(0x3f50, 0xc0000350, 16, 16, 16, 16);
  gemmini_extended_compute_preloaded(0x10, 0xffffffff, 16, 16, 16, 16);
  gemmini_extended_preload(0x3f60, 0xc0000360, 16, 16, 16, 16);
  gemmini_extended_compute_preloaded(0x10, 0xffffffff, 16, 16, 16, 16);
  gemmini_extended_preload(0x3f70, 0xc0000370, 16, 16, 16, 16);
  gemmini_extended_compute_preloaded(0x10, 0xffffffff, 16, 16, 16, 16);
  gemmini_extended_mvout(C + 0x340, 0xc0000340, 64, 16);
  gemmini_extended_mvin2(B + 0x4380, 0x3f80, 64, 16);
  gemmini_extended_preload(0x3f80, 0xc0000380, 16, 16, 16, 16);
  gemmini_extended_compute_preloaded(0x10, 0xffffffff, 16, 16, 16, 16);
  gemmini_extended_preload(0x3f90, 0xc0000390, 16, 16, 16, 16);
  gemmini_extended_compute_preloaded(0x10, 0xffffffff, 16, 16, 16, 16);
  gemmini_extended_preload(0x3fa0, 0xc00003a0, 16, 16, 16, 16);
  gemmini_extended_compute_preloaded(0x10, 0xffffffff, 16, 16, 16, 16);
  gemmini_extended_preload(0x3fb0, 0xc00003b0, 16, 16, 16, 16);
  gemmini_extended_compute_preloaded(0x10, 0xffffffff, 16, 16, 16, 16);
  gemmini_extended_mvout(C + 0x380, 0xc0000380, 64, 16);
  gemmini_extended_mvin2(B + 0x43c0, 0x3fc0, 64, 16);
  gemmini_extended_preload(0x3fc0, 0xc00003c0, 16, 16, 16, 16);
  gemmini_extended_compute_preloaded(0x10, 0xffffffff, 16, 16, 16, 16);
  gemmini_extended_preload(0x3fd0, 0xc00003d0, 16, 16, 16, 16);
  gemmini_extended_compute_preloaded(0x10, 0xffffffff, 16, 16, 16, 16);
  gemmini_extended_preload(0x3fe0, 0xc00003e0, 16, 16, 16, 16);
  gemmini_extended_compute_preloaded(0x10, 0xffffffff, 16, 16, 16, 16);
  gemmini_extended_preload(0x3ff0, 0xc00003f0, 16, 16, 16, 16);
  gemmini_extended_compute_preloaded(0x10, 0xffffffff, 16, 16, 16, 16);
  gemmini_extended_mvout(C + 0x3c0, 0xc00003c0, 64, 16);

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