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
#define INPUT_SIZE  128
#define HIDDEN_SIZE 32
#define HIDDEN_SIZE2 64
#define HIDDEN_SIZE3 256
#define OUTPUT_SIZE 128

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

  // Input Layer
  // Matrix input_mat, elem_t, BATCH_SIZE, INPUT_SIZE Setup

  // Matrix weights0, elem_t, INPUT_SIZE, HIDDEN_SIZE Setup
  
  elem_t inter_results0[BATCH_SIZE][HIDDEN_SIZE];

  // Hidden Layer #1
  // Matrix weights1, elem_t, HIDDEN_SIZE, HIDDEN_SIZE Setup
  
  elem_t inter_results1[BATCH_SIZE][HIDDEN_SIZE];

  // Hidden Layer #2
  // Matrix weights2, elem_t, HIDDEN_SIZE, HIDDEN_SIZE2 Setup
  
  elem_t inter_results2[BATCH_SIZE][HIDDEN_SIZE2];

  // Hidden Layer #3
  // Matrix weights3, elem_t, HIDDEN_SIZE2, HIDDEN_SIZE3 Setup
  
  elem_t inter_results3[BATCH_SIZE][HIDDEN_SIZE3];

  // Output Layer
  // Matrix weights4, elem_t, HIDDEN_SIZE3, OUTPUT_SIZE Setup
  
  elem_t output_mat[BATCH_SIZE][OUTPUT_SIZE];

  // Setup end
  setup_end = read_cycles();

  // Gemmini instructions start
  gemmini_fence();

  // Tiled matmul start
  start = read_cycles();

  // Clear TLB
  gemmini_flush(0);
  
  // MLP Layer Matmuls
  sp_tiled_matmul_auto_ws(BATCH_SIZE, HIDDEN_SIZE, INPUT_SIZE, input_mat, weights0, NULL, inter_results0, NO_ACTIVATION);

  sp_tiled_matmul_auto_ws(BATCH_SIZE, HIDDEN_SIZE, HIDDEN_SIZE, inter_results0, weights1, NULL, inter_results1, NO_ACTIVATION);

  sp_tiled_matmul_auto_ws(BATCH_SIZE, HIDDEN_SIZE2, HIDDEN_SIZE, inter_results1, weights2, NULL, inter_results2, NO_ACTIVATION);

  sp_tiled_matmul_auto_ws(BATCH_SIZE, HIDDEN_SIZE3, HIDDEN_SIZE2, inter_results2, weights3, NULL, inter_results3, NO_ACTIVATION);

  sp_tiled_matmul_auto_ws(BATCH_SIZE, OUTPUT_SIZE, HIDDEN_SIZE3, inter_results3, weights4, NULL, output_mat, NO_ACTIVATION);

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