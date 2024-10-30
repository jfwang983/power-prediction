#include <stdint.h>
#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>
#include "include/gemmini_testutils.h"

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

void sp_tiled_conv_simple(int batch_size, int in_row_dim, int in_col_dim, int in_channels,
    int out_channels, int out_row_dim, int out_col_dim, int stride, int padding,
    int kernel_dim, int kernel_dilation, int pool_out_row_dim, int pool_out_col_dim,
    int pool_size, int pool_stride, int pool_padding, int batches,
    int porows, int pocols, int pochs, int krows, int kcols, int kchs,
    int lpad, int rpad, int upad, int dpad, int plpad, int prpad, int pupad, int pdpad,
    const elem_t * input, const elem_t * weights, elem_t * output, const acc_t * bias,
    int act, acc_scale_t scale, bool wrot180, bool trans_output_1203, bool trans_input_3120,
    bool trans_weight_1203, bool trans_weight_0132, bool no_bias, bool no_pool,
    bool downsample, bool input_dilated, bool dw) {

    // When dw convs are true, we assume that kchs and ochs are 1
    if (dw) { kchs = 1; pochs = 1; }

    const int orows = porows * pool_stride + pool_size - 1 - pupad - pdpad;
    const int ocols = pocols * pool_stride + pool_size - 1 - plpad - prpad;
    const int ochs = pochs;

    // Calculate image dimensions
    // Note: "irows" and "icols" includes padding
    const int dilated_krows = krows + (kernel_dilation - 1)*(krows - 1);
    const int dilated_kcols = kcols + (kernel_dilation - 1)*(kcols - 1);
    int irows = orows * stride + dilated_krows - 1;
    int icols = ocols * stride + dilated_kcols - 1;
    int irows_unpadded = irows - upad - dpad;
    int icols_unpadded = icols - lpad - rpad;
    const int ichs = kchs;

    #define UNDILATED(x) ((input_dilated) ? (((x)+1)/2) : (x))

    if (input_dilated) {
      irows_unpadded = (irows_unpadded+1)/2;
      icols_unpadded = (icols_unpadded+1)/2;

      irows = irows_unpadded + UNDILATED(upad) + UNDILATED(dpad);
      icols = icols_unpadded + UNDILATED(lpad) + UNDILATED(rpad);
    }

    #ifdef HAS_FIRST_LAYER_OPTIMIZATIONS
      const bool transposed = trans_output_1203 || trans_input_3120 || trans_weight_1203 || trans_weight_0132;
      int max_pixels_per_row = transposed || wrot180 || downsample || input_dilated || kernel_dilation > 1 || ichs > DIM ? 1 : DIM/ichs;
      if (max_pixels_per_row > kcols) max_pixels_per_row = kcols;
    #else
      const int max_pixels_per_row = 1;
    #endif

    // Calculate spad address offsets
    const int out_channels_per_bank = ochs / DIM + (ochs % DIM != 0);
    const int in_channels_per_bank = kchs / DIM + (kchs % DIM != 0);
    const int B_rows = trans_weight_0132 ? in_channels_per_bank * kcols * krows * ochs : out_channels_per_bank * kcols * krows * kchs;

    static uint32_t D_sp_addr_row = 0;
    static uint32_t C_sp_addr_row = 0;

    const uint32_t A_sp_addr_start = 0;
    const uint32_t B_sp_addr_start = BANK_NUM * BANK_ROWS - B_rows;
    const uint32_t D_sp_addr_start = (1 << (ADDR_LEN - 1)) + D_sp_addr_row;
    const uint32_t C_sp_addr_start = (3 << (ADDR_LEN - 2)) + C_sp_addr_row;

    if (bias != 0) {
      D_sp_addr_row = (D_sp_addr_row + ACC_ROWS / 2) % ACC_ROWS;
    }

    if (output != 0) {
      C_sp_addr_row = (C_sp_addr_row + ACC_ROWS / 2) % ACC_ROWS;
    }

    if (!no_pool) {
      printf("Pooling with rectangular convolutions is currently not supported.\n");
      exit(1);
    }

    // Only rectangular convolutions will use the following C code

    // mvin bias
    if (bias != NULL) {
      // TODO we probably don't need quite this many nested loops for this part
      const int max_ochs_per_mvin = ochs < MAX_BLOCK_LEN_ACC * DIM ? ochs : MAX_BLOCK_LEN_ACC * DIM;

      for (int b = 0; b < batches; b++) {
        for (int orow = 0; orow < orows; orow++) {
          for (int ocol = 0; ocol < ocols; ocol += DIM) {
            const int I = ocols - ocol > DIM ? DIM : ocols - ocol;
            for (int och = 0; och < ochs; och += max_ochs_per_mvin) {
              const int J = ochs - och > max_ochs_per_mvin ? max_ochs_per_mvin : ochs - och;
              const uint32_t D_sp_addr = D_sp_addr_start + (och / DIM) * batches * orows * ocols + b * orows * ocols + orow * ocols + ocol;
              const acc_t * bias_dram_addr = no_bias ? NULL : bias + och;
              gemmini_extended_mvin3(bias_dram_addr, D_sp_addr, J, I);
              // printf("gemmini_extended_mvin3(bias + %p, %p, %u, %u);\n", och, D_sp_addr, J, I);
            }
          }
        }
      }
    }

    // mvin input
    {
      int max_chs_per_mvin = ichs < MAX_BLOCK_LEN * DIM ? ichs : MAX_BLOCK_LEN * DIM;
      if (trans_input_3120) {
        max_chs_per_mvin = batches < MAX_BLOCK_LEN * DIM ? batches : MAX_BLOCK_LEN * DIM;
      }

      const int dram_stride = trans_input_3120 ? batch_size * sizeof(elem_t) : in_channels * sizeof(elem_t);

      const int b_it = trans_input_3120 ? max_chs_per_mvin : 1;
      const int ich_it = trans_input_3120 ? 1 : max_chs_per_mvin;

      for (int b = 0; b < batches; b += b_it) {
        for (int irow = -UNDILATED(upad); irow < irows_unpadded + UNDILATED(dpad); irow += 1 + downsample) {
          const int irow_padded = irow + UNDILATED(upad);

          for (int icol = -UNDILATED(lpad); icol < icols_unpadded + UNDILATED(rpad);) {
            // TODO There might be some unnecessary mvins here at the edge of the image

            int I = icols_unpadded - icol > (DIM << downsample) ? (DIM << downsample) : icols_unpadded - icol;

            if (icol < 0) {
              I = -icol > DIM ? DIM : -icol;
            } else if (icol >= icols_unpadded) {
              I = icols_unpadded + UNDILATED(rpad) - icol > DIM ? DIM : icols_unpadded + UNDILATED(rpad) - icol;
            }

            const int icol_padded = icol + UNDILATED(lpad);

            for (int ich = 0; ich < ichs; ich += ich_it) {
              int K = ichs - ich > max_chs_per_mvin ? max_chs_per_mvin : ichs - ich;
              if (trans_input_3120) {
                K = batches - b > max_chs_per_mvin ? max_chs_per_mvin : batches - b;
              }

              #define DS(x) ((x) >> (downsample))

              uint32_t A_sp_addr = A_sp_addr_start + (ich / DIM) * batches * DS(irows) * DS(icols) + b * DS(irows) * DS(icols) + DS(irow_padded) * DS(icols) + DS(icol_padded);
              if (trans_input_3120) {
                A_sp_addr = A_sp_addr_start + (b / DIM) * ichs * DS(irows) * DS(icols) + ich * DS(irows) * DS(icols) + DS(irow_padded) * DS(icols) + DS(icol_padded);
              }

              const bool is_zeros = irow < 0 || irow >= irows_unpadded || icol < 0 || icol >= icols_unpadded;

              const elem_t * in = input + (b*in_row_dim*in_col_dim + irow*in_col_dim + icol) * in_channels + ich;
              if (is_zeros) {
                in = NULL;
              } else if (trans_input_3120) {
                in = input + (ich*in_row_dim*in_col_dim + irow*in_col_dim + icol) * batch_size + b;
              }

              gemmini_extended_mvin(in, A_sp_addr, K, I >> downsample);
              // printf("gemmini_extended_mvin(input + %p, %p, %u, %u);\n", (b*in_row_dim*in_col_dim + irow*in_col_dim + icol) * in_channels + ich, A_sp_addr, K, I >> downsample);
            }
            icol += I;
          }
        }
      }
    }

    // mvin weights
    {
      int max_chs_per_mvin = ochs < MAX_BLOCK_LEN * DIM ? ochs : MAX_BLOCK_LEN * DIM;
      if (trans_weight_0132) {
        max_chs_per_mvin = kchs < MAX_BLOCK_LEN * DIM ? kchs : MAX_BLOCK_LEN * DIM;
      }

      size_t dram_stride = out_channels * sizeof(elem_t);
      if (dw) {
        dram_stride = sizeof(elem_t);
      } else if (trans_weight_1203) {
        dram_stride = kernel_dim * kernel_dim * out_channels * sizeof(elem_t);
      } else if (trans_weight_0132) {
        dram_stride = in_channels * sizeof(elem_t);
      }

      const size_t och_it = trans_weight_0132 ? DIM : max_chs_per_mvin;
      const size_t kch_it = trans_weight_0132 ? max_chs_per_mvin : DIM;

      for (int och = 0; och < ochs; och += och_it) {
        for (int krow = 0; krow < krows; krow++) {
          for (int kcol = 0; kcol < kcols; kcol++) {
            for (int kch = 0; kch < kchs; kch += kch_it) {
              int K = kchs - kch > DIM ? DIM : kchs - kch;
              int J = ochs - och > max_chs_per_mvin ? max_chs_per_mvin : ochs - och;
              if (trans_weight_0132) {
                K = ochs - och > DIM ? DIM : ochs - och;
                J = kchs - kch > max_chs_per_mvin ? max_chs_per_mvin : kchs - kch;
              }

              uint32_t B_sp_addr = B_sp_addr_start + (och / DIM) * krows * kcols * kchs + krow * kcols * kchs + kcol * kchs + kch;
              if (trans_weight_0132) {
                B_sp_addr = B_sp_addr_start + (kch / DIM) * krows * kcols * ochs + krow * kcols * ochs + kcol * ochs + och;
              }

              const elem_t * w = weights + (krow*kernel_dim*in_channels + kcol*in_channels + kch) * out_channels + och;
              if (dw) {
                w = weights + krow * kernel_dim + kcol;
              } else if (trans_weight_1203) {
                w = weights + (kch * kernel_dim * kernel_dim + krow * kernel_dim + kcol) * out_channels + och;
              } else if (trans_weight_0132) {
                w = weights + (krow * kernel_dim * out_channels + kcol * out_channels + och) * in_channels + kch;
              }

              gemmini_extended_mvin2(w, B_sp_addr, J, K);
              // printf("gemmini_extended_mvin2(weights + %p, %p, %u, %u);\n", (krow*kernel_dim*in_channels + kcol*in_channels + kch) * out_channels + och, B_sp_addr, J, K);
            }
          }
        }
      }
    }

    // Compute
    {
      const int b_it = trans_input_3120 ? DIM : 1;
      const int ocol_it = trans_input_3120 ? 1 : (DIM << input_dilated);

      for (int och = 0; och < ochs; och += DIM) {
        for (int krow = 0; krow < krows; krow++) {
          for (int kcol = 0; kcol < kcols; kcol += max_pixels_per_row) {
            for (int kch = 0; kch < kchs; kch += DIM) {
              bool new_weights = true;
              for (int b = 0; b < batches; b += b_it) {
                for (int orow = 0; orow < orows; orow++) {
                  // Skip some kernel rows due to input-dilation
                  if (input_dilated && ((krow * kernel_dilation + orow * stride - upad) % 2 != 0)) {
                    continue;
                  }

                  for (int ocol = 0; ocol < ocols;) {
                    // Skip some cols dimensions due to input-dilation
                    if (input_dilated && ((kcol + ocol * stride - lpad) % 2 != 0)) {
                      ocol++;
                      continue;
                    }

                    int irow = orow * stride + krow * kernel_dilation;
                    int icol = ocol * stride + kcol * kernel_dilation;

                    if (input_dilated) {
                      irow = (irow + 1) / 2;
                      icol = (icol + 1) / 2;
                    }

                    const int pixels = kcols - kcol > max_pixels_per_row ? max_pixels_per_row : kcols - kcol;

                    const uint32_t C_sp_addr = C_sp_addr_start + (och / DIM) * batches * orows * ocols + b * orows * ocols + orow * ocols + ocol;

                    // Over here, construct a new matrix
                    //
                    // Let us assume that we only ever operate on
                    // one pixel in one row.
                    // Thus, krows == kcols == 1
                    //
                    // Then, for every set of I, J, and K values
                    //     - I = ocols
                    //     - J = ochs
                    //     - K = kchs

                    int I = UNDILATED(ocols - ocol > (DIM << input_dilated) ? (DIM << input_dilated) : ocols - ocol);
                    const int J = ochs - och > DIM ? DIM : ochs - och;
                    const int K = pixels * (kchs - kch > DIM ? DIM : kchs - kch);

                    if (trans_input_3120) {
                      I = batches - b > DIM ? DIM : batches - b;
                    }

                    uint32_t A_sp_addr = A_sp_addr_start + (kch / DIM) * batches * DS(irows) * DS(icols) + b * DS(irows) * DS(icols) + DS(irow) * DS(icols) + DS(icol);
                    if (trans_input_3120) {
                      A_sp_addr = A_sp_addr_start + (b / DIM) * kchs * DS(irows) * DS(icols) + kch * DS(irows) * DS(icols) + DS(irow) * DS(icols) + DS(icol);
                    }

                    const int krow_ = wrot180 ? krows - krow - 1 : krow;
                    const int kcol_ = wrot180 ? kcols - kcol - 1 : kcol;

                    uint32_t B_sp_addr = B_sp_addr_start + (och / DIM) * krows * kcols * kchs + krow_ * kcols * kchs + kcol_ * kchs + kch;
                    if (trans_weight_0132) {
                      B_sp_addr = B_sp_addr_start + (kch / DIM) * krows * kcols * ochs + krow_ * kcols * ochs + kcol_ * ochs + och;
                    }

                    const uint32_t pre_sp_addr = new_weights ? B_sp_addr : GARBAGE_ADDR;

                    // perform matmul
                    gemmini_extended_preload(pre_sp_addr, C_sp_addr, J, K, J, I);
                    // printf("gemmini_extended_preload(%p, 0x%x, %u, %u, %u, %u);\n", pre_sp_addr, C_sp_addr, J, K, J, I);
                    if (new_weights) {
                      gemmini_extended_compute_preloaded(A_sp_addr, GARBAGE_ADDR, K, I, J, I);
                      // printf("gemmini_extended_compute_preloaded(%p, 0x%x, %u, %u, %u, %u);\n", A_sp_addr, GARBAGE_ADDR, K, I, J, I);
                    } else {
                      gemmini_extended_compute_accumulated(A_sp_addr, GARBAGE_ADDR, K, I, J, I);
                      // printf("gemmini_extended_compute_accumulated(%p, 0x%x, %u, %u, %u, %u);\n", A_sp_addr, GARBAGE_ADDR, K, I, J, I);
                    }

                    ocol += ocol_it;
                    new_weights = false;
                  }
                }
              }
            }
          }
        }
      }
    }

    #undef DS
    #undef UNDILATED

    // mvout output
    if (output != NULL) {
      if (no_pool) {
        for (int b = 0; b < batches; b++) {
          for (int orow = 0; orow < orows; orow++) {
            for (int ocol = 0; ocol < ocols; ocol += DIM) {
              const int I = ocols - ocol > DIM ? DIM : ocols - ocol;
              for (int och = 0; och < ochs; och += DIM) {
                const int J = ochs - och > DIM ? DIM : ochs - och;
                const uint32_t C_sp_addr = C_sp_addr_start + (och / DIM) * batches * orows * ocols + b * orows * ocols + orow * ocols + ocol;

                elem_t * out = output + (b*out_row_dim*out_col_dim + orow*out_col_dim + ocol) * out_channels + och;
                if (trans_output_1203) {
                  out = output + (orow*out_col_dim*batch_size + ocol*batch_size + b) * out_channels + och;
                }

                gemmini_extended_mvout(out, C_sp_addr, J, I);
                // printf("gemmini_extended_mvout(output + %p, 0x%x, %u, %u);\n", (b*out_row_dim*out_col_dim + orow*out_col_dim + ocol) * out_channels + och, C_sp_addr, J, I);
              }
            }
          }
        }
      } else {
        printf("Pooling with rectangular convolutions is currently not supported.\n");
        exit(1);
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
    gemmini_extended3_config_ld(stride_B * sizeof(elem_t), B_scale_factor, false, 1);
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

void sp_tiled_conv_auto(int batch_size, int in_row_dim, int in_col_dim, int in_channels,
    int out_channels, int out_row_dim, int out_col_dim, int stride, int padding, int kernel_dim,
    const elem_t * input, const elem_t * weights, elem_t * output, const acc_t * bias, int act) {
    
    int kernel_dilation = 1;

    int pool_out_row_dim = 0;
    int pool_out_col_dim = 0;
    int pool_size = 1;
    int pool_stride = 1;
    int pool_padding = 0;

    int batches = batch_size;
    int krows = kernel_dim;
    int kcols = kernel_dim;
    int kchs = in_channels;
    
    int porows = out_row_dim;
    int pocols = out_col_dim;
    int pochs = out_channels;
    
    int lpad = padding;
    int rpad = padding;
    int upad = padding;
    int dpad = padding;

    int plpad = 0;
    int prpad = 0;
    int pupad = 0;
    int pdpad = 0;
    
    acc_scale_t scale = ACC_SCALE_IDENTITY;

    bool wrot180 = false;
    bool trans_output_1203 = false;
    bool trans_input_3120 = false;
    bool trans_weight_1203 = false;
    bool trans_weight_0132 = false;

    bool no_bias = bias == NULL;
    bool no_pool = true;
    bool downsample = false;
    bool input_dilated = false;
    bool dw = false;

    int irows = porows * stride + krows - 1;
    int icols = pocols * stride + kcols - 1;
    const int dram_stride = in_channels * sizeof(elem_t);
    const int spad_stride = batches * irows * icols;
    const size_t spad_block_stride = krows * kcols * kchs;

    int max_pixels_per_row = kchs > DIM ? 1 : DIM/kchs;
    max_pixels_per_row = (max_pixels_per_row > kcols) ? kcols : max_pixels_per_row;

    gemmini_extended4_config_ld(0, MVIN_SCALE_IDENTITY, false, batches * porows * pocols, 2);
    gemmini_extended5_config_ld(dram_stride, MVIN_SCALE_IDENTITY, false, spad_stride, max_pixels_per_row, 0);
    gemmini_extended4_config_ld(dram_stride, MVIN_SCALE_IDENTITY, false, spad_block_stride, 1);
    gemmini_config_ex(WS, act, 0);
    gemmini_extended_config_st(out_channels * sizeof(elem_t), act, scale);
  
    // printf("gemmini_extended4_config_ld(0, MVIN_SCALE_IDENTITY, false, %d, 2);\n", batches * porows * pocols);
    // printf("gemmini_extended5_config_ld(%d, MVIN_SCALE_IDENTITY, false, %d, %d, 0);\n", dram_stride, spad_stride, max_pixels_per_row);
    // printf("gemmini_extended4_config_ld(%d, MVIN_SCALE_IDENTITY, false, %d, 1);\n", dram_stride, spad_block_stride);
    // printf("gemmini_config_ex(WS, %d, 0);\n", act);
    // printf("gemmini_extended_config_st(%d, %d, ACC_SCALE_IDENTITY);\n", out_channels * sizeof(elem_t), act);

    sp_tiled_conv_simple(batch_size, in_row_dim, in_col_dim, in_channels,
        out_channels, out_row_dim, out_col_dim, stride, padding,
        kernel_dim, kernel_dilation, pool_out_row_dim, pool_out_col_dim,
        pool_size, pool_stride, pool_padding, batches,
        porows, pocols, pochs, krows, kcols, kchs,
        lpad, rpad, upad, dpad, plpad, prpad, pupad, pdpad,
        input, weights, output, bias, act, scale, wrot180,
        trans_output_1203, trans_input_3120, trans_weight_1203, trans_weight_0132,
        no_bias, no_pool, downsample, input_dilated, dw);
}





// void sp_tiled_conv_simple(int batch_size, int in_row_dim, int in_col_dim, int in_channels,
//     int out_channels, int out_row_dim, int out_col_dim, int stride, int padding,
//     int kernel_dim, int kernel_dilation, int pool_out_row_dim, int pool_out_col_dim,
//     int pool_size, int pool_stride, int pool_padding, int batches,
//     int porows, int pocols, int pochs, int krows, int kcols, int kchs,
//     int lpad, int rpad, int upad, int dpad, int plpad, int prpad, int pupad, int pdpad,
//     const elem_t * input, const elem_t * weights, elem_t * output, const acc_t * bias,
//     int act, acc_scale_t scale, bool wrot180, bool trans_output_1203, bool trans_input_3120,
//     bool trans_weight_1203, bool trans_weight_0132, bool no_bias, bool no_pool,
//     bool downsample, bool input_dilated, bool dw) {

//     // When dw convs are true, we assume that kchs and ochs are 1
//     if (dw) { kchs = 1; pochs = 1; }

//     const int orows = porows * pool_stride + pool_size - 1 - pupad - pdpad;
//     const int ocols = pocols * pool_stride + pool_size - 1 - plpad - prpad;
//     const int ochs = pochs;

//     // Calculate image dimensions
//     // Note: "irows" and "icols" includes padding
//     const int dilated_krows = krows + (kernel_dilation - 1)*(krows - 1);
//     const int dilated_kcols = kcols + (kernel_dilation - 1)*(kcols - 1);
//     int irows = orows * stride + dilated_krows - 1;
//     int icols = ocols * stride + dilated_kcols - 1;
//     int irows_unpadded = irows - upad - dpad;
//     int icols_unpadded = icols - lpad - rpad;
//     const int ichs = kchs;

//     #define UNDILATED(x) ((input_dilated) ? (((x)+1)/2) : (x))

//     if (input_dilated) {
//       irows_unpadded = (irows_unpadded+1)/2;
//       icols_unpadded = (icols_unpadded+1)/2;

//       irows = irows_unpadded + UNDILATED(upad) + UNDILATED(dpad);
//       icols = icols_unpadded + UNDILATED(lpad) + UNDILATED(rpad);
//     }

//     #ifdef HAS_FIRST_LAYER_OPTIMIZATIONS
//       const bool transposed = trans_output_1203 || trans_input_3120 || trans_weight_1203 || trans_weight_0132;
//       int max_pixels_per_row = transposed || wrot180 || downsample || input_dilated || kernel_dilation > 1 || ichs > DIM ? 1 : DIM/ichs;
//       if (max_pixels_per_row > kcols) max_pixels_per_row = kcols;
//     #else
//       const int max_pixels_per_row = 1;
//     #endif

//     // Calculate spad address offsets
//     const int out_channels_per_bank = ochs / DIM + (ochs % DIM != 0);
//     const int in_channels_per_bank = kchs / DIM + (kchs % DIM != 0);
//     const int B_rows = trans_weight_0132 ? in_channels_per_bank * kcols * krows * ochs : out_channels_per_bank * kcols * krows * kchs;

//     static uint32_t D_sp_addr_row = 0;
//     static uint32_t C_sp_addr_row = 0;

//     const uint32_t A_sp_addr_start = 0;
//     const uint32_t B_sp_addr_start = BANK_NUM * BANK_ROWS - B_rows;
//     const uint32_t D_sp_addr_start = (1 << (ADDR_LEN - 1)) + D_sp_addr_row;
//     const uint32_t C_sp_addr_start = (3 << (ADDR_LEN - 2)) + C_sp_addr_row;

//     if (bias != 0) {
//       D_sp_addr_row = (D_sp_addr_row + ACC_ROWS / 2) % ACC_ROWS;
//     }

//     if (output != 0) {
//       C_sp_addr_row = (C_sp_addr_row + ACC_ROWS / 2) % ACC_ROWS;
//     }

//     if (!no_pool) {
//       printf("Pooling with rectangular convolutions is currently not supported.\n");
//       exit(1);
//     }

//     // Only rectangular convolutions will use the following C code

//     // mvin bias
//     if (bias != NULL) {
//       // TODO we probably don't need quite this many nested loops for this part
//       const int max_ochs_per_mvin = ochs < MAX_BLOCK_LEN_ACC * DIM ? ochs : MAX_BLOCK_LEN_ACC * DIM;
//       gemmini_extended4_config_ld(0, MVIN_SCALE_IDENTITY, false, batches * orows * ocols, 2);

//       for (int b = 0; b < batches; b++)
//         for (int orow = 0; orow < orows; orow++)
//           for (int ocol = 0; ocol < ocols; ocol += DIM) {
//             const int I = ocols - ocol > DIM ? DIM : ocols - ocol;
//             for (int och = 0; och < ochs; och += max_ochs_per_mvin) {
//               const int J = ochs - och > max_ochs_per_mvin ? max_ochs_per_mvin : ochs - och;
//               const uint32_t D_sp_addr = D_sp_addr_start + (och / DIM) * batches * orows * ocols + b * orows * ocols + orow * ocols + ocol;
//               const acc_t * bias_dram_addr = no_bias ? NULL : bias + och;
//               gemmini_extended_mvin3(bias_dram_addr, D_sp_addr, J, I);
//             }
//           }
//     }

//     // mvin input
//     {
//       int max_chs_per_mvin = ichs < MAX_BLOCK_LEN * DIM ? ichs : MAX_BLOCK_LEN * DIM;
//       if (trans_input_3120) {
//         max_chs_per_mvin = batches < MAX_BLOCK_LEN * DIM ? batches : MAX_BLOCK_LEN * DIM;
//       }

//       const int dram_stride = trans_input_3120 ? batch_size * sizeof(elem_t) : in_channels * sizeof(elem_t);

//       const int spad_stride = trans_input_3120 ? ichs * (irows >> downsample) * (icols >> downsample) : batches * (irows >> downsample) * (icols >> downsample);

//       gemmini_extended5_config_ld(dram_stride << downsample, MVIN_SCALE_IDENTITY, false, spad_stride, max_pixels_per_row, 0);

//       const int b_it = trans_input_3120 ? max_chs_per_mvin : 1;
//       const int ich_it = trans_input_3120 ? 1 : max_chs_per_mvin;

//       for (int b = 0; b < batches; b += b_it)
//         for (int irow = -UNDILATED(upad); irow < irows_unpadded + UNDILATED(dpad); irow += 1 + downsample) {
//           const int irow_padded = irow + UNDILATED(upad);

//           for (int icol = -UNDILATED(lpad); icol < icols_unpadded + UNDILATED(rpad);) {
//             // TODO There might be some unnecessary mvins here at the edge of the image

//             int I = icols_unpadded - icol > (DIM << downsample) ? (DIM << downsample) : icols_unpadded - icol;

//             if (icol < 0) {
//               I = -icol > DIM ? DIM : -icol;
//             } else if (icol >= icols_unpadded) {
//               I = icols_unpadded + UNDILATED(rpad) - icol > DIM ? DIM : icols_unpadded + UNDILATED(rpad) - icol;
//             }

//             const int icol_padded = icol + UNDILATED(lpad);

//             for (int ich = 0; ich < ichs; ich += ich_it) {
//               int K = ichs - ich > max_chs_per_mvin ? max_chs_per_mvin : ichs - ich;
//               if (trans_input_3120) {
//                 K = batches - b > max_chs_per_mvin ? max_chs_per_mvin : batches - b;
//               }

//               #define DS(x) ((x) >> (downsample))

//               uint32_t A_sp_addr = A_sp_addr_start + (ich / DIM) * batches * DS(irows) * DS(icols) + b * DS(irows) * DS(icols) + DS(irow_padded) * DS(icols) + DS(icol_padded);
//               if (trans_input_3120) {
//                 A_sp_addr = A_sp_addr_start + (b / DIM) * ichs * DS(irows) * DS(icols) + ich * DS(irows) * DS(icols) + DS(irow_padded) * DS(icols) + DS(icol_padded);
//               }

//               const bool is_zeros = irow < 0 || irow >= irows_unpadded || icol < 0 || icol >= icols_unpadded;

//               const elem_t * in = input + (b*in_row_dim*in_col_dim + irow*in_col_dim + icol) * in_channels + ich;
//               if (is_zeros) {
//                 in = NULL;
//               } else if (trans_input_3120) {
//                 in = input + (ich*in_row_dim*in_col_dim + irow*in_col_dim + icol) * batch_size + b;
//               }

//               gemmini_extended_mvin(in, A_sp_addr, K, I >> downsample);
//             }
//             icol += I;
//           }
//         }
//     }

//     // mvin weights
//     {
//       int max_chs_per_mvin = ochs < MAX_BLOCK_LEN * DIM ? ochs : MAX_BLOCK_LEN * DIM;
//       if (trans_weight_0132) {
//         max_chs_per_mvin = kchs < MAX_BLOCK_LEN * DIM ? kchs : MAX_BLOCK_LEN * DIM;
//       }

//       size_t dram_stride = out_channels * sizeof(elem_t);
//       if (dw) {
//         dram_stride = sizeof(elem_t);
//       } else if (trans_weight_1203) {
//         dram_stride = kernel_dim * kernel_dim * out_channels * sizeof(elem_t);
//       } else if (trans_weight_0132) {
//         dram_stride = in_channels * sizeof(elem_t);
//       }

//       const size_t spad_block_stride = trans_weight_0132 ? krows * kcols * ochs : krows * kcols * kchs;

//       gemmini_extended4_config_ld(dram_stride, MVIN_SCALE_IDENTITY, false, spad_block_stride, 1);

//       const size_t och_it = trans_weight_0132 ? DIM : max_chs_per_mvin;
//       const size_t kch_it = trans_weight_0132 ? max_chs_per_mvin : DIM;

//       for (int och = 0; och < ochs; och += och_it) {
//         for (int krow = 0; krow < krows; krow++)
//           for (int kcol = 0; kcol < kcols; kcol++)
//             for (int kch = 0; kch < kchs; kch += kch_it) {
//               int K = kchs - kch > DIM ? DIM : kchs - kch;
//               int J = ochs - och > max_chs_per_mvin ? max_chs_per_mvin : ochs - och;
//               if (trans_weight_0132) {
//                 K = ochs - och > DIM ? DIM : ochs - och;
//                 J = kchs - kch > max_chs_per_mvin ? max_chs_per_mvin : kchs - kch;
//               }

//               uint32_t B_sp_addr = B_sp_addr_start + (och / DIM) * krows * kcols * kchs + krow * kcols * kchs + kcol * kchs + kch;
//               if (trans_weight_0132) {
//                 B_sp_addr = B_sp_addr_start + (kch / DIM) * krows * kcols * ochs + krow * kcols * ochs + kcol * ochs + och;
//               }

//               const elem_t * w = weights + (krow*kernel_dim*in_channels + kcol*in_channels + kch) * out_channels + och;
//               if (dw) {
//                 w = weights + krow * kernel_dim + kcol;
//               } else if (trans_weight_1203) {
//                 w = weights + (kch * kernel_dim * kernel_dim + krow * kernel_dim + kcol) * out_channels + och;
//               } else if (trans_weight_0132) {
//                 w = weights + (krow * kernel_dim * out_channels + kcol * out_channels + och) * in_channels + kch;
//               }

//               gemmini_extended_mvin2(w, B_sp_addr, J, K);
//             }
//       }
//     }

//     // Compute
//     {
//       const int b_it = trans_input_3120 ? DIM : 1;
//       const int ocol_it = trans_input_3120 ? 1 : (DIM << input_dilated);

//       if (trans_input_3120) {
//         gemmini_extended3_config_ex(0, 0, 0, 0, orows * ocols, irows * icols, 0, 0, true);
//       } else {
//         gemmini_config_ex(WS, NO_ACTIVATION, 0);
//       }

//       for (int och = 0; och < ochs; och += DIM) {
//         for (int krow = 0; krow < krows; krow++) {
//           for (int kcol = 0; kcol < kcols; kcol += max_pixels_per_row) {
//             for (int kch = 0; kch < kchs; kch += DIM) {
//               bool new_weights = true;
//               for (int b = 0; b < batches; b += b_it) {
//                 for (int orow = 0; orow < orows; orow++) {
//                   // Skip some kernel rows due to input-dilation
//                   if (input_dilated && ((krow * kernel_dilation + orow * stride - upad) % 2 != 0)) {
//                     continue;
//                   }

//                   for (int ocol = 0; ocol < ocols;) {
//                     // Skip some cols dimensions due to input-dilation
//                     if (input_dilated && ((kcol + ocol * stride - lpad) % 2 != 0)) {
//                       ocol++;
//                       continue;
//                     }

//                     int irow = orow * stride + krow * kernel_dilation;
//                     int icol = ocol * stride + kcol * kernel_dilation;

//                     if (input_dilated) {
//                       irow = (irow + 1) / 2;
//                       icol = (icol + 1) / 2;
//                     }

//                     const int pixels = kcols - kcol > max_pixels_per_row ? max_pixels_per_row : kcols - kcol;

//                     const uint32_t C_sp_addr = C_sp_addr_start + (och / DIM) * batches * orows * ocols + b * orows * ocols + orow * ocols + ocol;

//                     // Over here, construct a new matrix
//                     //
//                     // Let us assume that we only ever operate on
//                     // one pixel in one row.
//                     // Thus, krows == kcols == 1
//                     //
//                     // Then, for every set of I, J, and K values
//                     //     - I = ocols
//                     //     - J = ochs
//                     //     - K = kchs

//                     int I = UNDILATED(ocols - ocol > (DIM << input_dilated) ? (DIM << input_dilated) : ocols - ocol);
//                     const int J = ochs - och > DIM ? DIM : ochs - och;
//                     const int K = pixels * (kchs - kch > DIM ? DIM : kchs - kch);

//                     if (trans_input_3120) {
//                       I = batches - b > DIM ? DIM : batches - b;
//                     }

//                     uint32_t A_sp_addr = A_sp_addr_start + (kch / DIM) * batches * DS(irows) * DS(icols) + b * DS(irows) * DS(icols) + DS(irow) * DS(icols) + DS(icol);
//                     if (trans_input_3120) {
//                       A_sp_addr = A_sp_addr_start + (b / DIM) * kchs * DS(irows) * DS(icols) + kch * DS(irows) * DS(icols) + DS(irow) * DS(icols) + DS(icol);
//                     }

//                     const int krow_ = wrot180 ? krows - krow - 1 : krow;
//                     const int kcol_ = wrot180 ? kcols - kcol - 1 : kcol;

//                     uint32_t B_sp_addr = B_sp_addr_start + (och / DIM) * krows * kcols * kchs + krow_ * kcols * kchs + kcol_ * kchs + kch;
//                     if (trans_weight_0132) {
//                       B_sp_addr = B_sp_addr_start + (kch / DIM) * krows * kcols * ochs + krow_ * kcols * ochs + kcol_ * ochs + och;
//                     }

//                     const uint32_t pre_sp_addr = new_weights ? B_sp_addr : GARBAGE_ADDR;

//                     // perform matmul
//                     gemmini_extended_preload(pre_sp_addr, C_sp_addr, J, K, J, I);

//                     if (new_weights) {
//                       gemmini_extended_compute_preloaded(A_sp_addr, GARBAGE_ADDR, K, I, J, I);
//                     } else {
//                       gemmini_extended_compute_accumulated(A_sp_addr, GARBAGE_ADDR, K, I, J, I);
//                     }

//                     ocol += ocol_it;
//                     new_weights = false;
//                   }
//                 }
//               }
//             }
//           }
//         }
//       }
//     }

//     #undef DS
//     #undef UNDILATED

//     // mvout output
//     if (output != NULL) {
//       if (no_pool) {
//         gemmini_extended_config_st(out_channels * sizeof(elem_t), act, scale);
//         for (int b = 0; b < batches; b++) {
//           for (int orow = 0; orow < orows; orow++) {
//             for (int ocol = 0; ocol < ocols; ocol += DIM) {
//               const int I = ocols - ocol > DIM ? DIM : ocols - ocol;
//               for (int och = 0; och < ochs; och += DIM) {
//                 const int J = ochs - och > DIM ? DIM : ochs - och;
//                 const uint32_t C_sp_addr = C_sp_addr_start + (och / DIM) * batches * orows * ocols + b * orows * ocols + orow * ocols + ocol;

//                 elem_t * out = output + (b*out_row_dim*out_col_dim + orow*out_col_dim + ocol) * out_channels + och;
//                 if (trans_output_1203) {
//                   out = output + (orow*out_col_dim*batch_size + ocol*batch_size + b) * out_channels + och;
//                 }

//                 gemmini_extended_mvout(out, C_sp_addr, J, I);
//               }
//             }
//           }
//         }
//       } else {
//         printf("Pooling with rectangular convolutions is currently not supported.\n");
//         exit(1);
//       }
//     }
// }