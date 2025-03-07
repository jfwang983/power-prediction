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
#define INPUT_SIZE  128
#define HIDDEN_SIZE 256

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
  gemmini_extended_config_st(256, 0, ACC_SCALE_IDENTITY);
  gemmini_extended3_config_ld(128, MVIN_SCALE_IDENTITY, 0, 0);
  gemmini_extended3_config_ld(256, MVIN_SCALE_IDENTITY, 0, 1);
  gemmini_extended3_config_ld(1024, MVIN_SCALE_IDENTITY, 0, 2);
  gemmini_extended_mvin(A + 0x0, 0x0, 64, 16);
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
  gemmini_extended_mvin2(B + 0x1000, 0x3900, 64, 16);
  gemmini_extended_preload(0x3900, 0xc0000000, 16, 16, 16, 16);
  gemmini_extended_compute_preloaded(0x10, 0xffffffff, 16, 16, 16, 16);
  gemmini_extended_preload(0x3910, 0xc0000010, 16, 16, 16, 16);
  gemmini_extended_compute_preloaded(0x10, 0xffffffff, 16, 16, 16, 16);
  gemmini_extended_preload(0x3920, 0xc0000020, 16, 16, 16, 16);
  gemmini_extended_compute_preloaded(0x10, 0xffffffff, 16, 16, 16, 16);
  gemmini_extended_preload(0x3930, 0xc0000030, 16, 16, 16, 16);
  gemmini_extended_compute_preloaded(0x10, 0xffffffff, 16, 16, 16, 16);
  gemmini_extended_mvin2(B + 0x1040, 0x3940, 64, 16);
  gemmini_extended_preload(0x3940, 0xc0000040, 16, 16, 16, 16);
  gemmini_extended_compute_preloaded(0x10, 0xffffffff, 16, 16, 16, 16);
  gemmini_extended_preload(0x3950, 0xc0000050, 16, 16, 16, 16);
  gemmini_extended_compute_preloaded(0x10, 0xffffffff, 16, 16, 16, 16);
  gemmini_extended_preload(0x3960, 0xc0000060, 16, 16, 16, 16);
  gemmini_extended_compute_preloaded(0x10, 0xffffffff, 16, 16, 16, 16);
  gemmini_extended_preload(0x3970, 0xc0000070, 16, 16, 16, 16);
  gemmini_extended_compute_preloaded(0x10, 0xffffffff, 16, 16, 16, 16);
  gemmini_extended_mvin2(B + 0x1080, 0x3980, 64, 16);
  gemmini_extended_preload(0x3980, 0xc0000080, 16, 16, 16, 16);
  gemmini_extended_compute_preloaded(0x10, 0xffffffff, 16, 16, 16, 16);
  gemmini_extended_preload(0x3990, 0xc0000090, 16, 16, 16, 16);
  gemmini_extended_compute_preloaded(0x10, 0xffffffff, 16, 16, 16, 16);
  gemmini_extended_preload(0x39a0, 0xc00000a0, 16, 16, 16, 16);
  gemmini_extended_compute_preloaded(0x10, 0xffffffff, 16, 16, 16, 16);
  gemmini_extended_preload(0x39b0, 0xc00000b0, 16, 16, 16, 16);
  gemmini_extended_compute_preloaded(0x10, 0xffffffff, 16, 16, 16, 16);
  gemmini_extended_mvin2(B + 0x10c0, 0x39c0, 64, 16);
  gemmini_extended_preload(0x39c0, 0xc00000c0, 16, 16, 16, 16);
  gemmini_extended_compute_preloaded(0x10, 0xffffffff, 16, 16, 16, 16);
  gemmini_extended_preload(0x39d0, 0xc00000d0, 16, 16, 16, 16);
  gemmini_extended_compute_preloaded(0x10, 0xffffffff, 16, 16, 16, 16);
  gemmini_extended_preload(0x39e0, 0xc00000e0, 16, 16, 16, 16);
  gemmini_extended_compute_preloaded(0x10, 0xffffffff, 16, 16, 16, 16);
  gemmini_extended_preload(0x39f0, 0xc00000f0, 16, 16, 16, 16);
  gemmini_extended_compute_preloaded(0x10, 0xffffffff, 16, 16, 16, 16);
  gemmini_extended_mvin2(B + 0x2000, 0x3a00, 64, 16);
  gemmini_extended_preload(0x3a00, 0xc0000000, 16, 16, 16, 16);
  gemmini_extended_compute_preloaded(0x20, 0xffffffff, 16, 16, 16, 16);
  gemmini_extended_preload(0x3a10, 0xc0000010, 16, 16, 16, 16);
  gemmini_extended_compute_preloaded(0x20, 0xffffffff, 16, 16, 16, 16);
  gemmini_extended_preload(0x3a20, 0xc0000020, 16, 16, 16, 16);
  gemmini_extended_compute_preloaded(0x20, 0xffffffff, 16, 16, 16, 16);
  gemmini_extended_preload(0x3a30, 0xc0000030, 16, 16, 16, 16);
  gemmini_extended_compute_preloaded(0x20, 0xffffffff, 16, 16, 16, 16);
  gemmini_extended_mvin2(B + 0x2040, 0x3a40, 64, 16);
  gemmini_extended_preload(0x3a40, 0xc0000040, 16, 16, 16, 16);
  gemmini_extended_compute_preloaded(0x20, 0xffffffff, 16, 16, 16, 16);
  gemmini_extended_preload(0x3a50, 0xc0000050, 16, 16, 16, 16);
  gemmini_extended_compute_preloaded(0x20, 0xffffffff, 16, 16, 16, 16);
  gemmini_extended_preload(0x3a60, 0xc0000060, 16, 16, 16, 16);
  gemmini_extended_compute_preloaded(0x20, 0xffffffff, 16, 16, 16, 16);
  gemmini_extended_preload(0x3a70, 0xc0000070, 16, 16, 16, 16);
  gemmini_extended_compute_preloaded(0x20, 0xffffffff, 16, 16, 16, 16);
  gemmini_extended_mvin2(B + 0x2080, 0x3a80, 64, 16);
  gemmini_extended_preload(0x3a80, 0xc0000080, 16, 16, 16, 16);
  gemmini_extended_compute_preloaded(0x20, 0xffffffff, 16, 16, 16, 16);
  gemmini_extended_preload(0x3a90, 0xc0000090, 16, 16, 16, 16);
  gemmini_extended_compute_preloaded(0x20, 0xffffffff, 16, 16, 16, 16);
  gemmini_extended_preload(0x3aa0, 0xc00000a0, 16, 16, 16, 16);
  gemmini_extended_compute_preloaded(0x20, 0xffffffff, 16, 16, 16, 16);
  gemmini_extended_preload(0x3ab0, 0xc00000b0, 16, 16, 16, 16);
  gemmini_extended_compute_preloaded(0x20, 0xffffffff, 16, 16, 16, 16);
  gemmini_extended_mvin2(B + 0x20c0, 0x3ac0, 64, 16);
  gemmini_extended_preload(0x3ac0, 0xc00000c0, 16, 16, 16, 16);
  gemmini_extended_compute_preloaded(0x20, 0xffffffff, 16, 16, 16, 16);
  gemmini_extended_preload(0x3ad0, 0xc00000d0, 16, 16, 16, 16);
  gemmini_extended_compute_preloaded(0x20, 0xffffffff, 16, 16, 16, 16);
  gemmini_extended_preload(0x3ae0, 0xc00000e0, 16, 16, 16, 16);
  gemmini_extended_compute_preloaded(0x20, 0xffffffff, 16, 16, 16, 16);
  gemmini_extended_preload(0x3af0, 0xc00000f0, 16, 16, 16, 16);
  gemmini_extended_compute_preloaded(0x20, 0xffffffff, 16, 16, 16, 16);
  gemmini_extended_mvin2(B + 0x3000, 0x3b00, 64, 16);
  gemmini_extended_preload(0x3b00, 0xc0000000, 16, 16, 16, 16);
  gemmini_extended_compute_preloaded(0x30, 0xffffffff, 16, 16, 16, 16);
  gemmini_extended_preload(0x3b10, 0xc0000010, 16, 16, 16, 16);
  gemmini_extended_compute_preloaded(0x30, 0xffffffff, 16, 16, 16, 16);
  gemmini_extended_preload(0x3b20, 0xc0000020, 16, 16, 16, 16);
  gemmini_extended_compute_preloaded(0x30, 0xffffffff, 16, 16, 16, 16);
  gemmini_extended_preload(0x3b30, 0xc0000030, 16, 16, 16, 16);
  gemmini_extended_compute_preloaded(0x30, 0xffffffff, 16, 16, 16, 16);
  gemmini_extended_mvin2(B + 0x3040, 0x3b40, 64, 16);
  gemmini_extended_preload(0x3b40, 0xc0000040, 16, 16, 16, 16);
  gemmini_extended_compute_preloaded(0x30, 0xffffffff, 16, 16, 16, 16);
  gemmini_extended_preload(0x3b50, 0xc0000050, 16, 16, 16, 16);
  gemmini_extended_compute_preloaded(0x30, 0xffffffff, 16, 16, 16, 16);
  gemmini_extended_preload(0x3b60, 0xc0000060, 16, 16, 16, 16);
  gemmini_extended_compute_preloaded(0x30, 0xffffffff, 16, 16, 16, 16);
  gemmini_extended_preload(0x3b70, 0xc0000070, 16, 16, 16, 16);
  gemmini_extended_compute_preloaded(0x30, 0xffffffff, 16, 16, 16, 16);
  gemmini_extended_mvin2(B + 0x3080, 0x3b80, 64, 16);
  gemmini_extended_preload(0x3b80, 0xc0000080, 16, 16, 16, 16);
  gemmini_extended_compute_preloaded(0x30, 0xffffffff, 16, 16, 16, 16);
  gemmini_extended_preload(0x3b90, 0xc0000090, 16, 16, 16, 16);
  gemmini_extended_compute_preloaded(0x30, 0xffffffff, 16, 16, 16, 16);
  gemmini_extended_preload(0x3ba0, 0xc00000a0, 16, 16, 16, 16);
  gemmini_extended_compute_preloaded(0x30, 0xffffffff, 16, 16, 16, 16);
  gemmini_extended_preload(0x3bb0, 0xc00000b0, 16, 16, 16, 16);
  gemmini_extended_compute_preloaded(0x30, 0xffffffff, 16, 16, 16, 16);
  gemmini_extended_mvin2(B + 0x30c0, 0x3bc0, 64, 16);
  gemmini_extended_preload(0x3bc0, 0xc00000c0, 16, 16, 16, 16);
  gemmini_extended_compute_preloaded(0x30, 0xffffffff, 16, 16, 16, 16);
  gemmini_extended_preload(0x3bd0, 0xc00000d0, 16, 16, 16, 16);
  gemmini_extended_compute_preloaded(0x30, 0xffffffff, 16, 16, 16, 16);
  gemmini_extended_preload(0x3be0, 0xc00000e0, 16, 16, 16, 16);
  gemmini_extended_compute_preloaded(0x30, 0xffffffff, 16, 16, 16, 16);
  gemmini_extended_preload(0x3bf0, 0xc00000f0, 16, 16, 16, 16);
  gemmini_extended_compute_preloaded(0x30, 0xffffffff, 16, 16, 16, 16);
  gemmini_extended_mvin(A + 0x40, 0x40, 64, 16);
  gemmini_extended_mvin2(B + 0x4000, 0x3c00, 64, 16);
  gemmini_extended_preload(0x3c00, 0xc0000000, 16, 16, 16, 16);
  gemmini_extended_compute_preloaded(0x40, 0xffffffff, 16, 16, 16, 16);
  gemmini_extended_preload(0x3c10, 0xc0000010, 16, 16, 16, 16);
  gemmini_extended_compute_preloaded(0x40, 0xffffffff, 16, 16, 16, 16);
  gemmini_extended_preload(0x3c20, 0xc0000020, 16, 16, 16, 16);
  gemmini_extended_compute_preloaded(0x40, 0xffffffff, 16, 16, 16, 16);
  gemmini_extended_preload(0x3c30, 0xc0000030, 16, 16, 16, 16);
  gemmini_extended_compute_preloaded(0x40, 0xffffffff, 16, 16, 16, 16);
  gemmini_extended_mvin2(B + 0x4040, 0x3c40, 64, 16);
  gemmini_extended_preload(0x3c40, 0xc0000040, 16, 16, 16, 16);
  gemmini_extended_compute_preloaded(0x40, 0xffffffff, 16, 16, 16, 16);
  gemmini_extended_preload(0x3c50, 0xc0000050, 16, 16, 16, 16);
  gemmini_extended_compute_preloaded(0x40, 0xffffffff, 16, 16, 16, 16);
  gemmini_extended_preload(0x3c60, 0xc0000060, 16, 16, 16, 16);
  gemmini_extended_compute_preloaded(0x40, 0xffffffff, 16, 16, 16, 16);
  gemmini_extended_preload(0x3c70, 0xc0000070, 16, 16, 16, 16);
  gemmini_extended_compute_preloaded(0x40, 0xffffffff, 16, 16, 16, 16);
  gemmini_extended_mvin2(B + 0x4080, 0x3c80, 64, 16);
  gemmini_extended_preload(0x3c80, 0xc0000080, 16, 16, 16, 16);
  gemmini_extended_compute_preloaded(0x40, 0xffffffff, 16, 16, 16, 16);
  gemmini_extended_preload(0x3c90, 0xc0000090, 16, 16, 16, 16);
  gemmini_extended_compute_preloaded(0x40, 0xffffffff, 16, 16, 16, 16);
  gemmini_extended_preload(0x3ca0, 0xc00000a0, 16, 16, 16, 16);
  gemmini_extended_compute_preloaded(0x40, 0xffffffff, 16, 16, 16, 16);
  gemmini_extended_preload(0x3cb0, 0xc00000b0, 16, 16, 16, 16);
  gemmini_extended_compute_preloaded(0x40, 0xffffffff, 16, 16, 16, 16);
  gemmini_extended_mvin2(B + 0x40c0, 0x3cc0, 64, 16);
  gemmini_extended_preload(0x3cc0, 0xc00000c0, 16, 16, 16, 16);
  gemmini_extended_compute_preloaded(0x40, 0xffffffff, 16, 16, 16, 16);
  gemmini_extended_preload(0x3cd0, 0xc00000d0, 16, 16, 16, 16);
  gemmini_extended_compute_preloaded(0x40, 0xffffffff, 16, 16, 16, 16);
  gemmini_extended_preload(0x3ce0, 0xc00000e0, 16, 16, 16, 16);
  gemmini_extended_compute_preloaded(0x40, 0xffffffff, 16, 16, 16, 16);
  gemmini_extended_preload(0x3cf0, 0xc00000f0, 16, 16, 16, 16);
  gemmini_extended_compute_preloaded(0x40, 0xffffffff, 16, 16, 16, 16);
  gemmini_extended_mvin2(B + 0x5000, 0x3d00, 64, 16);
  gemmini_extended_preload(0x3d00, 0xc0000000, 16, 16, 16, 16);
  gemmini_extended_compute_preloaded(0x50, 0xffffffff, 16, 16, 16, 16);
  gemmini_extended_preload(0x3d10, 0xc0000010, 16, 16, 16, 16);
  gemmini_extended_compute_preloaded(0x50, 0xffffffff, 16, 16, 16, 16);
  gemmini_extended_preload(0x3d20, 0xc0000020, 16, 16, 16, 16);
  gemmini_extended_compute_preloaded(0x50, 0xffffffff, 16, 16, 16, 16);
  gemmini_extended_preload(0x3d30, 0xc0000030, 16, 16, 16, 16);
  gemmini_extended_compute_preloaded(0x50, 0xffffffff, 16, 16, 16, 16);
  gemmini_extended_mvin2(B + 0x5040, 0x3d40, 64, 16);
  gemmini_extended_preload(0x3d40, 0xc0000040, 16, 16, 16, 16);
  gemmini_extended_compute_preloaded(0x50, 0xffffffff, 16, 16, 16, 16);
  gemmini_extended_preload(0x3d50, 0xc0000050, 16, 16, 16, 16);
  gemmini_extended_compute_preloaded(0x50, 0xffffffff, 16, 16, 16, 16);
  gemmini_extended_preload(0x3d60, 0xc0000060, 16, 16, 16, 16);
  gemmini_extended_compute_preloaded(0x50, 0xffffffff, 16, 16, 16, 16);
  gemmini_extended_preload(0x3d70, 0xc0000070, 16, 16, 16, 16);
  gemmini_extended_compute_preloaded(0x50, 0xffffffff, 16, 16, 16, 16);
  gemmini_extended_mvin2(B + 0x5080, 0x3d80, 64, 16);
  gemmini_extended_preload(0x3d80, 0xc0000080, 16, 16, 16, 16);
  gemmini_extended_compute_preloaded(0x50, 0xffffffff, 16, 16, 16, 16);
  gemmini_extended_preload(0x3d90, 0xc0000090, 16, 16, 16, 16);
  gemmini_extended_compute_preloaded(0x50, 0xffffffff, 16, 16, 16, 16);
  gemmini_extended_preload(0x3da0, 0xc00000a0, 16, 16, 16, 16);
  gemmini_extended_compute_preloaded(0x50, 0xffffffff, 16, 16, 16, 16);
  gemmini_extended_preload(0x3db0, 0xc00000b0, 16, 16, 16, 16);
  gemmini_extended_compute_preloaded(0x50, 0xffffffff, 16, 16, 16, 16);
  gemmini_extended_mvin2(B + 0x50c0, 0x3dc0, 64, 16);
  gemmini_extended_preload(0x3dc0, 0xc00000c0, 16, 16, 16, 16);
  gemmini_extended_compute_preloaded(0x50, 0xffffffff, 16, 16, 16, 16);
  gemmini_extended_preload(0x3dd0, 0xc00000d0, 16, 16, 16, 16);
  gemmini_extended_compute_preloaded(0x50, 0xffffffff, 16, 16, 16, 16);
  gemmini_extended_preload(0x3de0, 0xc00000e0, 16, 16, 16, 16);
  gemmini_extended_compute_preloaded(0x50, 0xffffffff, 16, 16, 16, 16);
  gemmini_extended_preload(0x3df0, 0xc00000f0, 16, 16, 16, 16);
  gemmini_extended_compute_preloaded(0x50, 0xffffffff, 16, 16, 16, 16);
  gemmini_extended_mvin2(B + 0x6000, 0x3e00, 64, 16);
  gemmini_extended_preload(0x3e00, 0xc0000000, 16, 16, 16, 16);
  gemmini_extended_compute_preloaded(0x60, 0xffffffff, 16, 16, 16, 16);
  gemmini_extended_preload(0x3e10, 0xc0000010, 16, 16, 16, 16);
  gemmini_extended_compute_preloaded(0x60, 0xffffffff, 16, 16, 16, 16);
  gemmini_extended_preload(0x3e20, 0xc0000020, 16, 16, 16, 16);
  gemmini_extended_compute_preloaded(0x60, 0xffffffff, 16, 16, 16, 16);
  gemmini_extended_preload(0x3e30, 0xc0000030, 16, 16, 16, 16);
  gemmini_extended_compute_preloaded(0x60, 0xffffffff, 16, 16, 16, 16);
  gemmini_extended_mvin2(B + 0x6040, 0x3e40, 64, 16);
  gemmini_extended_preload(0x3e40, 0xc0000040, 16, 16, 16, 16);
  gemmini_extended_compute_preloaded(0x60, 0xffffffff, 16, 16, 16, 16);
  gemmini_extended_preload(0x3e50, 0xc0000050, 16, 16, 16, 16);
  gemmini_extended_compute_preloaded(0x60, 0xffffffff, 16, 16, 16, 16);
  gemmini_extended_preload(0x3e60, 0xc0000060, 16, 16, 16, 16);
  gemmini_extended_compute_preloaded(0x60, 0xffffffff, 16, 16, 16, 16);
  gemmini_extended_preload(0x3e70, 0xc0000070, 16, 16, 16, 16);
  gemmini_extended_compute_preloaded(0x60, 0xffffffff, 16, 16, 16, 16);
  gemmini_extended_mvin2(B + 0x6080, 0x3e80, 64, 16);
  gemmini_extended_preload(0x3e80, 0xc0000080, 16, 16, 16, 16);
  gemmini_extended_compute_preloaded(0x60, 0xffffffff, 16, 16, 16, 16);
  gemmini_extended_preload(0x3e90, 0xc0000090, 16, 16, 16, 16);
  gemmini_extended_compute_preloaded(0x60, 0xffffffff, 16, 16, 16, 16);
  gemmini_extended_preload(0x3ea0, 0xc00000a0, 16, 16, 16, 16);
  gemmini_extended_compute_preloaded(0x60, 0xffffffff, 16, 16, 16, 16);
  gemmini_extended_preload(0x3eb0, 0xc00000b0, 16, 16, 16, 16);
  gemmini_extended_compute_preloaded(0x60, 0xffffffff, 16, 16, 16, 16);
  gemmini_extended_mvin2(B + 0x60c0, 0x3ec0, 64, 16);
  gemmini_extended_preload(0x3ec0, 0xc00000c0, 16, 16, 16, 16);
  gemmini_extended_compute_preloaded(0x60, 0xffffffff, 16, 16, 16, 16);
  gemmini_extended_preload(0x3ed0, 0xc00000d0, 16, 16, 16, 16);
  gemmini_extended_compute_preloaded(0x60, 0xffffffff, 16, 16, 16, 16);
  gemmini_extended_preload(0x3ee0, 0xc00000e0, 16, 16, 16, 16);
  gemmini_extended_compute_preloaded(0x60, 0xffffffff, 16, 16, 16, 16);
  gemmini_extended_preload(0x3ef0, 0xc00000f0, 16, 16, 16, 16);
  gemmini_extended_compute_preloaded(0x60, 0xffffffff, 16, 16, 16, 16);
  gemmini_extended_mvin2(B + 0x7000, 0x3f00, 64, 16);
  gemmini_extended_preload(0x3f00, 0xc0000000, 16, 16, 16, 16);
  gemmini_extended_compute_preloaded(0x70, 0xffffffff, 16, 16, 16, 16);
  gemmini_extended_preload(0x3f10, 0xc0000010, 16, 16, 16, 16);
  gemmini_extended_compute_preloaded(0x70, 0xffffffff, 16, 16, 16, 16);
  gemmini_extended_preload(0x3f20, 0xc0000020, 16, 16, 16, 16);
  gemmini_extended_compute_preloaded(0x70, 0xffffffff, 16, 16, 16, 16);
  gemmini_extended_preload(0x3f30, 0xc0000030, 16, 16, 16, 16);
  gemmini_extended_compute_preloaded(0x70, 0xffffffff, 16, 16, 16, 16);
  gemmini_extended_mvout(C + 0x0, 0xc0000000, 64, 16);
  gemmini_extended_mvin2(B + 0x7040, 0x3f40, 64, 16);
  gemmini_extended_preload(0x3f40, 0xc0000040, 16, 16, 16, 16);
  gemmini_extended_compute_preloaded(0x70, 0xffffffff, 16, 16, 16, 16);
  gemmini_extended_preload(0x3f50, 0xc0000050, 16, 16, 16, 16);
  gemmini_extended_compute_preloaded(0x70, 0xffffffff, 16, 16, 16, 16);
  gemmini_extended_preload(0x3f60, 0xc0000060, 16, 16, 16, 16);
  gemmini_extended_compute_preloaded(0x70, 0xffffffff, 16, 16, 16, 16);
  gemmini_extended_preload(0x3f70, 0xc0000070, 16, 16, 16, 16);
  gemmini_extended_compute_preloaded(0x70, 0xffffffff, 16, 16, 16, 16);
  gemmini_extended_mvout(C + 0x40, 0xc0000040, 64, 16);
  gemmini_extended_mvin2(B + 0x7080, 0x3f80, 64, 16);
  gemmini_extended_preload(0x3f80, 0xc0000080, 16, 16, 16, 16);
  gemmini_extended_compute_preloaded(0x70, 0xffffffff, 16, 16, 16, 16);
  gemmini_extended_preload(0x3f90, 0xc0000090, 16, 16, 16, 16);
  gemmini_extended_compute_preloaded(0x70, 0xffffffff, 16, 16, 16, 16);
  gemmini_extended_preload(0x3fa0, 0xc00000a0, 16, 16, 16, 16);
  gemmini_extended_compute_preloaded(0x70, 0xffffffff, 16, 16, 16, 16);
  gemmini_extended_preload(0x3fb0, 0xc00000b0, 16, 16, 16, 16);
  gemmini_extended_compute_preloaded(0x70, 0xffffffff, 16, 16, 16, 16);
  gemmini_extended_mvout(C + 0x80, 0xc0000080, 64, 16);
  gemmini_extended_mvin2(B + 0x70c0, 0x3fc0, 64, 16);
  gemmini_extended_preload(0x3fc0, 0xc00000c0, 16, 16, 16, 16);
  gemmini_extended_compute_preloaded(0x70, 0xffffffff, 16, 16, 16, 16);
  gemmini_extended_preload(0x3fd0, 0xc00000d0, 16, 16, 16, 16);
  gemmini_extended_compute_preloaded(0x70, 0xffffffff, 16, 16, 16, 16);
  gemmini_extended_preload(0x3fe0, 0xc00000e0, 16, 16, 16, 16);
  gemmini_extended_compute_preloaded(0x70, 0xffffffff, 16, 16, 16, 16);
  gemmini_extended_preload(0x3ff0, 0xc00000f0, 16, 16, 16, 16);
  gemmini_extended_compute_preloaded(0x70, 0xffffffff, 16, 16, 16, 16);
  gemmini_extended_mvout(C + 0xc0, 0xc00000c0, 64, 16);

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