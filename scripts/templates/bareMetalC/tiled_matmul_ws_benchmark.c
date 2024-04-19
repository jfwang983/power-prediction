// See LICENSE for license details.

#include <stdint.h>
#include <stddef.h>
#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#ifndef BAREMETAL
#include <sys/mman.h>
#endif
#include "include/gemmini_testutils.h"

#define BATCH_SIZE  16
#define INPUT_SIZE  64
#define HIDDEN_SIZE 256

void sp_tiled_matmul_ws_simple(const elem_t * A, const elem_t * B, const void * D, void * C,
    scale_t A_scale_factor, scale_t B_scale_factor, scale_acc_t D_scale_factor,
    size_t I, size_t J, size_t K, size_t pad_I, size_t pad_J, size_t pad_K,
    size_t A_row_stride, size_t B_row_stride, size_t D_row_stride, size_t C_row_stride,
    bool a_transpose, bool b_transpose, bool full_C, bool low_D, bool no_bias, bool repeating_bias,
    int act, int a_spad_id, int b_spad_id) {

    const uint32_t A_sp_addr_start = 0;
    const uint32_t B_sp_addr_start = BANK_NUM * BANK_ROWS - K * J * DIM;
    const uint32_t D_sp_addr_start = 1 << (ADDR_LEN-1);
    const uint32_t C_sp_addr_start = 3 << (ADDR_LEN-2) | (full_C << (ADDR_LEN-3));
    const int A_blocks = a_transpose ? (I <= MAX_BLOCK_LEN ? I : MAX_BLOCK_LEN) : (K <= MAX_BLOCK_LEN ? K : MAX_BLOCK_LEN);
    const int B_blocks = b_transpose ? (K <= MAX_BLOCK_LEN ? K : MAX_BLOCK_LEN) : (J <= MAX_BLOCK_LEN ? J : MAX_BLOCK_LEN);
    const int D_blocks = low_D ? (J <= MAX_BLOCK_LEN ? J : MAX_BLOCK_LEN) : (J <= MAX_BLOCK_LEN_ACC ? J : MAX_BLOCK_LEN_ACC);
    const int C_blocks = full_C ? 1 : (J <= MAX_BLOCK_LEN ? J : MAX_BLOCK_LEN);
    const size_t sizeof_D = low_D ? sizeof(elem_t) : sizeof(acc_t);
    const size_t sizeof_C = full_C ? sizeof(acc_t) : sizeof(elem_t);

    // Move-in D
    if (D != NULL && !no_bias) {
      for (size_t i = 0; i < I; i++) {
        const size_t rows = DIM - (i == I-1 ? pad_I : 0);
        for (size_t j = 0; j < J; j += D_blocks) {
          const size_t bias_row = repeating_bias ? 0 : i;
          const void * const D_dram_addr = (int8_t *)D + (bias_row * D_row_stride + j)*DIM*sizeof_D;
          const uint32_t D_sp_addr_acc = D_sp_addr_start + (i*J + j)*DIM;
          size_t blocks = j + D_blocks <= J ? D_blocks : J-j;
          const size_t cols = blocks * DIM - (j + blocks >= J ? pad_J : 0);
          gemmini_extended_mvin3(D_dram_addr, D_sp_addr_acc, cols, rows);
          // printf("gemmini_extended_mvin3(D + %p, %p, %u, %u);\n", (bias_row * D_row_stride + j)*DIM*sizeof_D, D_sp_addr_acc, cols, rows);
        }
      }
    }
    for (size_t k = 0; k < K; k++) {
      for (size_t j = 0; j < J; j++) {
        for (size_t i = 0; i < I; i++) {
          const uint32_t A_sp_addr = a_transpose ? (A_sp_addr_start + (k*I + i)*DIM) : (A_sp_addr_start + (i*K + k)*DIM);
          const uint32_t B_sp_addr = b_transpose ? (B_sp_addr_start + (j*K + k)*DIM) : (B_sp_addr_start + (k*J + j)*DIM);
          const uint32_t C_sp_addr = C_sp_addr_start + (i*J + j)*DIM;
          // Mvin A
          if (a_transpose) {
            if (j == 0 && i % A_blocks == 0) {
              const elem_t * const A_dram_addr = A + (k*A_row_stride + i)*DIM;
              const size_t blocks = i + A_blocks <= I ? A_blocks : I-i;
              const size_t cols = blocks * DIM - (i + blocks >= I ? pad_I : 0);
              const size_t rows = DIM - (k == K-1 ? pad_K : 0);
              gemmini_extended_mvin(A_dram_addr, A_sp_addr, cols, rows);
              // printf("gemmini_extended_mvin(A + %p, %p, %u, %u);\n", (k*A_row_stride+i)*DIM, A_sp_addr, cols, rows);
            }
          } else {
            if (j == 0 && k % A_blocks == 0) {
              const elem_t * const A_dram_addr = A + (i*A_row_stride + k)*DIM;
              const size_t blocks = k + A_blocks <= K ? A_blocks : K-k;
              const size_t cols = blocks * DIM - (k + blocks >= K ? pad_K : 0);
              const size_t rows = DIM - (i == I-1 ? pad_I : 0);
              gemmini_extended_mvin(A_dram_addr, A_sp_addr, cols, rows);
              // printf("gemmini_extended_mvin(A + %p, %p, %u, %u);\n", (i*A_row_stride+k)*DIM, A_sp_addr, cols, rows);
            }
          }
          // Mvin B
          if (b_transpose) {
            if (i == 0 && k % B_blocks == 0) {
              const elem_t * const B_dram_addr = B + (j*B_row_stride + k)*DIM;
              const size_t blocks = k + B_blocks <= K ? B_blocks : K-k;
              const size_t cols = blocks * DIM - (k + blocks >= K ? pad_K : 0);
              const size_t rows = DIM - (j == J-1 ? pad_J : 0);
              gemmini_extended_mvin2(B_dram_addr, B_sp_addr, cols, rows);
              // printf("gemmini_extended_mvin2(B + %p, %p, %u, %u);\n", (j*B_row_stride+k)*DIM, B_sp_addr, cols, rows);
            }
          } else {
            if (i == 0 && j % B_blocks == 0) {
              const elem_t * const B_dram_addr = B + (k*B_row_stride + j)*DIM;
              const size_t blocks = j + B_blocks <= J ? B_blocks : J-j;
              const size_t cols = blocks * DIM - (j + blocks >= J ? pad_J : 0);
              const size_t rows = DIM - (k == K-1 ? pad_K : 0);
              gemmini_extended_mvin2(B_dram_addr, B_sp_addr, cols, rows);
              // printf("gemmini_extended_mvin2(B + %p, %p, %u, %u);\n", (k*B_row_stride+j)*DIM, B_sp_addr, cols, rows);
            }
          }
          // Compute
          {
          uint32_t pre_sp_addr = i == 0 ? B_sp_addr : GARBAGE_ADDR;
          uint32_t out_sp_addr = C_sp_addr;
          // If we're not using a bias, then we want to overwrite what's in the
          // accumulator, rather than writing over it
          int no_bias_new_matrix = no_bias && D != NULL && k == 0;
          if (no_bias_new_matrix) {
            out_sp_addr &= ~(1 << (ADDR_LEN-2));
          }
          const size_t A_cols = DIM - (k == K - 1 ? pad_K : 0);
          const size_t A_rows = DIM - (i == I - 1 ? pad_I : 0);
          const size_t B_cols = DIM - (j == J - 1 ? pad_J : 0);
          const size_t B_rows = DIM - (k == K - 1 ? pad_K : 0);
          const size_t C_cols = DIM - (j == J - 1 ? pad_J : 0);
          const size_t C_rows = DIM - (i == I - 1 ? pad_I : 0);
          gemmini_extended_preload(pre_sp_addr, out_sp_addr, B_cols, B_rows, C_cols, C_rows);
          // printf("gemmini_extended_preload(%p, 0x%x, %u, %u, %u, %u);\n", pre_sp_addr, out_sp_addr, B_cols, B_rows, C_cols, C_rows);
          if (i == 0) { // First iteration
            gemmini_extended_compute_preloaded(A_sp_addr, GARBAGE_ADDR, A_cols, A_rows, DIM, DIM);
            // printf("gemmini_extended_compute_preloaded(%p, 0x%x, %u, %u, %u, %u);\n", A_sp_addr, GARBAGE_ADDR, A_cols, A_rows, DIM, DIM);
          } else { // All other iterations
            gemmini_extended_compute_accumulated(A_sp_addr, GARBAGE_ADDR, A_cols, A_rows, DIM, DIM);
            // printf("gemmini_extended_compute_accumulated(%p, 0x%x, %u, %u, %u, %u);\n", A_sp_addr, GARBAGE_ADDR, A_cols, A_rows, DIM, DIM);
          }
        }
        if (C != NULL && k == K-1) {
          // Move-out C (if not normalizing)
          if (((act != LAYERNORM) && (act != SOFTMAX)) && (j == J-1 || j % C_blocks == C_blocks-1)) {
            const size_t rounded_j = (j / C_blocks) * C_blocks;
            const uint32_t rounded_C_sp_addr = C_sp_addr_start + (i*J + rounded_j)*DIM;
            void * const C_dram_addr = (int8_t*)C + (i*C_row_stride + rounded_j)*DIM*sizeof_C;
            const size_t blocks = rounded_j + C_blocks <= J ? C_blocks : J-rounded_j;
            const size_t cols = blocks * DIM - (rounded_j + blocks >= J ? pad_J : 0);
            const size_t rows = DIM - (i == I - 1 ? pad_I : 0);
            gemmini_extended_mvout(C_dram_addr, rounded_C_sp_addr, cols, rows);
            // printf("gemmini_extended_mvout(C + %p, 0x%x, %u, %u);\n", (i*C_row_stride+rounded_j)*DIM*sizeof_C, rounded_C_sp_addr, cols, rows);
          }
        }
      } 
    }
  }
}

void sp_tiled_matmul_auto_ws(size_t dim_I, size_t dim_J, size_t dim_K,
    const elem_t* A, const elem_t* B, const void * D, void * C, int act) {
        
    size_t stride_A = dim_K;
    size_t stride_B = dim_J;
    size_t stride_D = dim_J;
    size_t stride_C = dim_J;

    scale_t A_scale_factor = MVIN_SCALE_IDENTITY;
    scale_t B_scale_factor = MVIN_SCALE_IDENTITY;
    scale_acc_t D_scale_factor = MVIN_SCALE_IDENTITY;
    acc_scale_t scale = ACC_SCALE_IDENTITY;
    acc_scale_t bert_scale = ACC_SCALE_IDENTITY;
    bool repeating_bias = false;
    bool transpose_A = false;
    bool transpose_B = false;

    bool full_C = false;
    bool low_D = 0;
    uint8_t weightA = 0;
    enum tiled_matmul_type_t tiled_matmul_type = WS;
    const size_t sizeof_D = low_D ? sizeof(elem_t) : sizeof(acc_t) ;
    const size_t sizeof_C = full_C ? sizeof(acc_t) : sizeof(elem_t);

    gemmini_extended_config_ex(WS, act & 3, 0, 1, transpose_A, transpose_B);
    gemmini_extended_config_st(stride_C * sizeof_C, act & 3, scale);
    gemmini_extended3_config_ld(stride_A * sizeof(elem_t), A_scale_factor, false, 0);
    gemmini_extended3_config_ld(stride_B * sizeof(elem_t), B_scale_factor, false, 1)
    gemmini_extended3_config_ld(repeating_bias ? 0 : (stride_D * sizeof_D), D_scale_factor, low_D, 2);

    // printf("gemmini_extended_config_ex(WS, %d, 0, 1, %d, %d);\n", act & 3, transpose_A, transpose_B);
    // printf("gemmini_extended_config_st(%d, %d, ACC_SCALE_IDENTITY);\n", stride_C * sizeof_C, act & 3);
    // printf("gemmini_extended3_config_ld(%d, MVIN_SCALE_IDENTITY, %d, %d);\n", stride_A * sizeof(elem_t), false, 0);
    // printf("gemmini_extended3_config_ld(%d, MVIN_SCALE_IDENTITY, %d, %d);\n", stride_B * sizeof(elem_t), false, 1);
    // printf("gemmini_extended3_config_ld(%d, MVIN_SCALE_IDENTITY, %d, %d);\n", repeating_bias ? 0 : (stride_D * sizeof_D), low_D, 2);
        
    sp_tiled_matmul_ws_simple(A, B, D == NULL ? 0x1 : D, C, A_scale_factor, B_scale_factor, D_scale_factor,
        dim_I / DIM, dim_J / DIM, dim_K / DIM, 0, 0, 0, stride_A, stride_B, stride_D, stride_C,
        transpose_A, transpose_B,full_C, low_D, true, repeating_bias, act, 1, 1);
}

int main() {
#ifndef BAREMETAL
    if (mlockall(MCL_CURRENT | MCL_FUTURE) != 0) {
      perror("mlockall failed");
      exit(1);
    }
#endif

  elem_t A[BATCH_SIZE][INPUT_SIZE];
  elem_t B[INPUT_SIZE][HIDDEN_SIZE];
  elem_t C[BATCH_SIZE][HIDDEN_SIZE];

  int iterations = 100;

  // Matrix Setup
  for(int i = 0; i < BATCH_SIZE; i++) {
    for(int j = 0; j < INPUT_SIZE; j++) {
      A[i][j] = rand() % 256 - 128;
    } 
  }

  for(int i = 0; i < INPUT_SIZE; i++) {
    for(int j = 0; j < HIDDEN_SIZE; j++) {
      B[i][j] = i == j;
    } 
  }

  unsigned long start, end, benchmark_cycles;
  start = read_cycles();

  // Gemmini instructions start
  gemmini_flush(0);

  // Main benchmark code
  for(int i = 0; i < iterations; i++) {
    sp_tiled_matmul_auto_ws(BATCH_SIZE, HIDDEN_SIZE, INPUT_SIZE, A, B, NULL, C, NO_ACTIVATION);
  }

  gemmini_fence();
  // Gemmini instructions end

  end = read_cycles();
  benchmark_cycles = end - start;
  printf("Cycles taken: %u\n", benchmark_cycles);

  exit(0);
}