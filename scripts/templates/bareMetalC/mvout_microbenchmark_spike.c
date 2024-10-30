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

#define BATCH_SIZE  32
#define INPUT_SIZE  32
#define HIDDEN_SIZE 32

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

  int iterations = 1000;

  // Matrix A, BATCH_SIZE, INPUT_SIZE Setup
	elem_t A[BATCH_SIZE][INPUT_SIZE] = { // Matrix A row
		{28, -67, 33, 44, -46, -58, -55, -2, -81, 116, -47, 47, -125, 7, -51, 110, -59, 47, -74, 74, 23, 11, -17, -16, -85, -98, -82, 32, -117, 116, -38, -53}, // Matrix A row
		{-93, -51, 99, -29, -5, -68, 40, -99, -91, -20, -84, 17, -98, -11, 99, 74, -28, -100, 102, 7, -77, 5, -59, -47, 29, -9, -12, 16, 127, 34, 16, -99}, // Matrix A row
		{105, 122, -55, -6, -63, -14, -47, 119, -112, 0, -111, 18, 30, 80, -38, -25, 80, -60, 66, 81, -10, 103, -56, -112, -100, 98, -92, -105, -20, 43, -55, 12}, // Matrix A row
		{-44, -114, 90, 42, -95, 122, 28, -121, 124, 82, 77, 45, 82, 121, 66, -68, -86, 21, -83, 76, -115, -12, 0, -91, 39, 58, -12, 95, 7, -108, 59, 102}, // Matrix A row
		{-89, 12, -95, -100, 120, -58, 45, -29, 25, -127, -35, 91, -119, -52, -128, -90, 120, 81, -40, 55, 19, -128, -9, 52, 126, -63, -102, 83, -114, 66, -44, -59}, // Matrix A row
		{77, 95, 74, 23, 127, -64, 58, 52, 67, 67, 42, -50, -65, -64, 64, 67, 111, 21, 85, -109, -110, -34, 39, 18, -110, 23, 24, -55, 85, 34, 15, 116}, // Matrix A row
		{65, 22, -77, -47, 38, 53, 9, 42, 122, -16, -38, -84, -112, 13, 14, -83, 82, 78, -42, 73, -48, -45, 69, -116, 1, -106, -12, 14, 105, -47, 114, -42}, // Matrix A row
		{6, 127, -32, 32, 126, -50, 57, -58, -48, -95, -46, 104, 21, -107, -120, 61, 95, -95, -94, -9, -21, 126, 53, 11, 77, 95, 27, -32, 60, 85, -38, -113}, // Matrix A row
		{3, 6, 89, 117, -87, -43, 112, -46, 78, 69, 64, 101, 43, 2, -95, 89, -72, 101, 83, 85, 66, -59, 66, 96, -107, -106, -9, -124, -39, -74, 29, 49}, // Matrix A row
		{101, -8, 77, 74, -4, -95, -22, -71, 51, 72, 121, -3, -40, -8, 88, 74, -35, -49, 3, -80, -109, 97, -63, 100, 66, 107, -35, 123, 26, 110, -119, 45}, // Matrix A row
		{-91, -107, 50, 124, -113, -45, 67, 2, -17, -65, 28, -59, -23, 127, 92, -80, -23, -115, 10, -14, 30, 46, 93, -42, -126, 5, 44, -24, 95, 124, 112, -101}, // Matrix A row
		{-21, 22, -101, -2, -118, -75, -79, 75, 9, -14, -65, -72, 92, -11, 34, 112, 100, 95, -61, -78, -67, 61, 15, -97, 31, 122, 16, -91, -36, -109, -112, 105}, // Matrix A row
		{49, 118, -24, -62, -109, 45, -10, -126, -74, 73, -101, -128, -13, 10, -12, -36, 74, -34, -118, -12, 73, 39, -50, -47, 42, -30, -62, 27, -57, 19, 4, -118}, // Matrix A row
		{25, 92, 64, 17, -8, -118, 96, -110, -123, 28, -94, -29, 46, -119, -79, -64, 11, 73, 16, -108, -124, 71, 19, -53, -7, -71, -27, 16, -32, -73, 26, -64}, // Matrix A row
		{114, 60, -63, -62, 11, -104, -8, -14, 19, 95, 118, -39, 34, 20, -69, 72, 9, -22, -2, -63, -48, 70, 58, -75, -31, -98, -87, 73, -61, -73, -29, 83}, // Matrix A row
		{-110, 69, 33, 113, -100, 97, -2, 46, 77, 56, 61, 7, 102, -100, -123, 118, 114, 41, 64, 1, 91, -91, -61, 103, 92, -21, 94, -7, 96, -39, 95, 107}, // Matrix A row
		{69, -94, -92, 85, -99, 120, 9, -106, 126, 66, 63, -31, 59, -49, -111, 74, 61, -73, 119, -18, -13, 5, -120, -38, 52, -49, -87, -106, 90, 116, -57, 81}, // Matrix A row
		{10, -121, 48, -116, -94, -60, 14, -44, -110, -33, -117, -81, 36, -108, 61, 14, -120, -79, 113, -121, -24, 111, -85, -52, -113, 96, 58, -97, 18, -25, 3, 38}, // Matrix A row
		{103, 106, -1, 81, 21, -113, 93, 99, 58, 8, 121, 28, 31, 90, -65, -65, 23, 32, 118, -80, 6, -21, 100, -103, 86, -66, 118, 111, 39, -108, 101, 109}, // Matrix A row
		{-25, 42, -127, 50, 5, -100, -90, -18, 100, -38, -90, -75, -3, -29, -15, 56, 43, -66, 62, -100, -22, 53, 22, -84, 103, 35, 29, 113, 62, 63, -123, 25}, // Matrix A row
		{-94, 79, -58, -72, 52, 38, -85, -97, 111, -28, -54, -79, 80, 111, 105, -58, -69, -19, 55, -78, 109, 71, 88, -75, -106, 90, 123, 64, 112, 100, 119, -32}, // Matrix A row
		{-96, -126, 55, 101, -89, 35, 101, 18, -91, -121, -39, -17, -10, 103, -84, -29, -41, -71, -97, 111, 92, 87, -59, -41, -33, -101, -70, -52, -106, 78, 57, -126}, // Matrix A row
		{15, -85, 53, -13, 53, -23, -37, 94, 22, -13, -27, 98, 63, 20, -56, -45, 125, -38, 15, -44, -74, 53, 93, 8, -76, -102, 14, -13, 108, -119, -36, 25}, // Matrix A row
		{-105, -87, -56, -80, 125, -126, 60, 103, -64, -6, 71, -124, -74, -105, -29, -16, 16, -32, 23, -108, -68, -36, -47, 59, -95, -111, -15, 11, 56, -75, 96, 28}, // Matrix A row
		{-14, -83, 41, 58, 87, 120, 67, 20, 10, -32, 58, -124, -91, -11, 110, 70, -51, 51, -126, 125, -28, 8, 63, 120, 15, 18, 10, 61, 39, -10, 25, -94}, // Matrix A row
		{66, -82, 103, -53, -24, 65, -65, -75, 44, 104, -101, 87, -78, 95, 47, -56, -49, 76, 71, -64, 89, 16, -128, -93, 47, 18, -10, -127, 99, 103, -8, -55}, // Matrix A row
		{-53, 34, -116, 35, 48, 94, -99, 14, -11, 62, 64, 124, 60, 83, 39, -109, 33, 77, 84, 114, 54, 8, -88, -49, -40, -30, 95, 22, 4, -104, -57, -32}, // Matrix A row
		{-34, 88, -109, 2, -34, 86, -52, -11, -101, -62, 123, 55, 2, 105, -36, -72, 2, -64, -104, 107, -112, -81, 44, 51, 51, 90, -74, 120, -125, -128, -50, 13}, // Matrix A row
		{42, -63, 18, -60, 77, -123, -101, 67, -90, -47, 94, 89, -108, 19, -32, 125, -69, -60, 83, 73, -36, -11, -70, 21, -8, -3, -76, -68, 58, 9, -123, -24}, // Matrix A row
		{-24, -32, -125, 39, -14, 118, -87, -12, -64, -10, 85, 27, -121, -47, 2, 95, -90, 20, -73, 88, 76, -45, 80, -87, 127, 29, -52, -45, 40, -42, 78, -19}, // Matrix A row
		{-73, 106, -1, 79, 49, 38, 7, -108, -92, 51, 113, 1, -124, 112, 106, -46, -3, 123, 106, 75, 24, -103, 47, -31, 9, 47, -33, -119, 22, -28, 42, -115}, // Matrix A row
		{51, 10, 48, 41, -97, -66, -83, 56, 85, -22, -41, -92, 47, 2, -110, 60, -59, 89, 10, -79, -48, -70, -68, -8, 19, 75, -103, -81, -17, -38, 55, -110} // Matrix A row
	}; // Matrix A row

  // Matrix B, INPUT_SIZE, HIDDEN_SIZE Setup
	elem_t B[INPUT_SIZE][HIDDEN_SIZE] = { // Matrix B row
		{-37, 69, -72, 111, 119, -100, -29, -80, -42, 7, 110, -51, -28, -70, 54, -87, 90, 24, -120, 15, -126, -46, 10, 28, 12, -106, 108, 125, -102, -96, 1, -35}, // Matrix B row
		{-6, 33, -122, 94, -31, 31, 1, 16, -86, -87, -81, 83, -32, -83, 13, -22, 74, -42, 9, -41, 13, -50, -73, -47, 16, 79, -88, 104, -6, 60, -73, 91}, // Matrix B row
		{123, -10, 101, -102, -103, 8, -80, 84, 40, 127, -9, -104, -17, 26, 95, -104, -29, 69, -3, -113, -31, -39, -119, 90, -26, -39, -80, -74, 120, -51, -61, 93}, // Matrix B row
		{-12, 32, -51, 50, -47, 103, -33, -93, 28, -25, -19, 123, -40, 84, 50, -53, 46, -87, 1, 45, -42, -122, 35, 13, -73, -61, 12, 120, -58, -74, -74, 88}, // Matrix B row
		{114, -71, -29, 108, -43, 22, 55, -76, -45, 22, 115, -43, 46, -64, -11, 2, -6, 18, 118, 3, 47, -67, -66, 14, 104, -59, 73, -75, -62, 60, 98, -48}, // Matrix B row
		{7, -57, 31, 115, 118, 90, -93, -112, 110, 82, -1, -16, -35, -50, 60, -71, 17, -78, -45, 9, -100, -50, -102, -113, 92, -32, -23, 26, 57, -117, 83, -75}, // Matrix B row
		{-36, -53, 94, 63, -71, -96, 121, -60, -27, 71, -67, 66, -99, -44, 42, 77, -43, 102, 19, -45, 98, 119, -22, 106, 82, -96, -66, 80, -75, 27, 97, 52}, // Matrix B row
		{-3, -48, 65, 56, -122, -75, 32, 19, 102, 76, -10, -90, -107, -2, -123, 25, -68, 28, 125, 89, -69, -88, -74, -115, -109, -85, 2, 35, 110, 73, -123, -5}, // Matrix B row
		{-23, -12, -97, 90, -101, 71, 58, -74, -27, 34, 9, -111, 11, -16, 4, -47, 46, 123, -29, -50, -6, -101, -14, -78, 115, 77, -46, 36, 59, 32, -86, 15}, // Matrix B row
		{8, -5, -120, -84, -14, 40, 48, 119, -26, -69, -72, -71, 1, 27, 50, 92, -117, -40, -126, -114, 103, -17, 104, -128, -53, 80, 72, 121, -106, 48, 58, 62}, // Matrix B row
		{-122, -23, 101, 12, 124, -46, -95, -85, 124, -55, 48, -17, 42, 43, 40, 105, 69, 113, 54, -89, -127, 18, 106, 78, -18, 111, 120, -35, -14, 22, -51, 33}, // Matrix B row
		{-72, 90, -64, -121, -11, 1, -42, 56, -70, -111, -111, 68, -75, -86, 32, -125, -67, -61, -11, -75, 92, 48, 71, 40, -91, -8, 23, 68, 94, -115, -75, -82}, // Matrix B row
		{-77, -42, -84, 7, -84, 80, -46, 5, 51, -94, 62, 125, 76, -118, -91, -100, 101, -107, 89, -72, -116, 51, -113, 120, 123, 44, 54, 61, -118, -77, 4, 88}, // Matrix B row
		{48, 119, -20, 3, 103, 92, 55, -95, -113, -25, 23, 123, 91, 38, -14, 104, 101, -26, -74, 39, -71, -31, -21, -31, 25, 15, 84, 111, -72, -28, 107, -26}, // Matrix B row
		{-83, -8, 118, -27, -29, 16, 81, -64, 69, 88, 96, 91, -84, -41, -1, 127, -35, -83, -102, 45, -41, -107, 102, -113, -6, -94, 120, -114, -20, 107, 107, 93}, // Matrix B row
		{54, -123, -125, 48, 17, 53, -55, 91, 56, 36, 67, -61, -99, 57, -94, 25, 72, 117, 18, 20, 65, -16, -26, 111, -104, 55, -86, -30, 96, 67, 43, 22}, // Matrix B row
		{-47, 60, 41, 62, -54, 40, 111, 47, 36, -70, -106, -31, -114, 108, -120, -83, -88, 79, 121, 46, 28, -88, 80, -82, -115, 66, 98, 126, 124, -91, 86, 54}, // Matrix B row
		{91, 111, 121, -11, 74, -109, 119, 13, 108, -57, 51, -10, 25, 82, -14, -59, -102, 74, 63, -21, -76, -75, -113, 98, -22, 37, 56, 96, 116, 65, 31, -34}, // Matrix B row
		{32, 13, -95, -47, 45, -111, 28, -89, 57, -85, 42, -42, 37, -74, -81, -25, -52, 11, 27, -75, 32, -81, -117, 10, -3, 47, -59, -90, -88, -95, 3, 67}, // Matrix B row
		{9, 47, 117, 47, 48, -50, 125, 81, -107, 63, -23, 68, -23, 1, 99, -104, -125, 79, -121, -80, 15, 55, -84, -107, 90, 17, -35, 13, 61, 110, 44, 117}, // Matrix B row
		{-63, -10, -84, 60, 100, 118, -31, 112, -112, -75, -98, -88, 97, 12, -80, -123, -65, -7, 68, 84, 5, -117, -83, -111, -86, -97, -91, -50, -61, 100, -80, 54}, // Matrix B row
		{-119, 27, -98, 42, -100, 115, 97, 57, 94, -64, 72, -1, -109, -55, -117, 52, 58, 37, -57, 88, -3, 117, 100, 10, 81, -43, 127, -108, -56, 8, 84, -90}, // Matrix B row
		{-9, -40, -92, -99, -31, -90, 63, -62, -54, -60, 63, 97, 15, 36, 117, -27, 14, -38, 41, -24, -27, 15, -47, -10, 112, 4, 22, 115, -119, -26, -117, 68}, // Matrix B row
		{38, -9, -4, -98, 68, 73, -16, 23, 63, 110, -72, -57, 122, -74, 78, 97, -9, -11, -3, -35, 75, -73, 88, -76, 13, 85, -112, 67, -90, 67, 69, -40}, // Matrix B row
		{-14, -69, 50, -122, -12, 51, 37, 112, 70, 120, -22, -35, 100, 87, 127, -126, -32, 39, -13, -14, -76, -58, -75, -96, 75, 124, -34, -33, -127, -85, 54, -31}, // Matrix B row
		{-58, -2, 120, 15, 56, -14, -78, 64, 1, 14, 36, -57, 71, -27, 29, 44, -20, 74, -39, -119, -53, 36, -74, -99, -23, 4, 15, 113, 74, 18, -94, -40}, // Matrix B row
		{-88, 115, -105, 49, 64, 122, 6, 41, -108, 34, 34, 1, -92, 63, 67, 18, 32, 43, 75, 29, 85, -95, 73, -71, 13, -19, -100, 104, 77, 93, 65, 73}, // Matrix B row
		{27, 112, 99, -67, 18, 54, -77, 24, -126, -54, -98, 106, -82, -7, 53, 84, -113, 3, 0, 93, 98, 112, -110, -94, 121, 71, 68, 24, 122, 119, -56, -11}, // Matrix B row
		{106, 23, 48, -36, -65, 17, -124, 20, -94, -6, 21, -15, 54, 22, 59, -88, -123, 11, 26, 19, -68, -103, 11, -27, -45, -22, 47, -16, -110, -17, -45, 65}, // Matrix B row
		{-56, 61, 29, -117, 81, 36, -61, 93, 67, -81, -114, -56, 91, 44, 57, 42, 94, -79, 49, -73, 33, 29, 16, -45, 48, 123, -29, -103, -115, -23, 8, 95}, // Matrix B row
		{106, -6, 64, -73, -105, -51, 24, 96, 107, -50, -31, 117, -65, -86, -70, -35, 62, -125, 54, 127, 36, 71, -8, 49, 15, 108, 1, -24, 46, -56, -75, 46}, // Matrix B row
		{-25, 74, 42, 22, 94, -29, -87, -113, 41, -6, 125, 23, -110, 42, -88, -126, 127, -68, 81, 94, 87, -76, -22, 36, -92, 75, -30, -13, 7, 2, -78, -31} // Matrix B row
	}; // Matrix B row

  elem_t C[BATCH_SIZE][HIDDEN_SIZE];

  size_t I = BATCH_SIZE / DIM;
  size_t J = HIDDEN_SIZE / DIM;
  size_t K = INPUT_SIZE / DIM;

  size_t A_row_stride = INPUT_SIZE;
  size_t B_row_stride = HIDDEN_SIZE;
  size_t C_row_stride = HIDDEN_SIZE;

  const uint32_t A_sp_addr_start = 0;
  const uint32_t B_sp_addr_start = BANK_NUM * BANK_ROWS - K * J * DIM;
  const uint32_t C_sp_addr_start = 3 << (ADDR_LEN-2);
  const int A_blocks = K <= MAX_BLOCK_LEN ? K : MAX_BLOCK_LEN;
  const int B_blocks = J <= MAX_BLOCK_LEN ? J : MAX_BLOCK_LEN;
  const int C_blocks = J <= MAX_BLOCK_LEN ? J : MAX_BLOCK_LEN;

  // Gemmini instructions start
  gemmini_fence();

  // Clear TLB
  gemmini_flush(0);

  // Config Setup
  gemmini_extended3_config_ld(INPUT_SIZE * sizeof(elem_t), MVIN_SCALE_IDENTITY, false, 0);
  gemmini_extended3_config_ld(HIDDEN_SIZE * sizeof(elem_t), MVIN_SCALE_IDENTITY, false, 1);
  gemmini_config_ex(WS, NO_ACTIVATION, 0);
  gemmini_config_st(HIDDEN_SIZE * sizeof(elem_t));

  // Multiply matrices
	for (size_t k = 0; k < K; k++) {
		for (size_t j = 0; j < J; j++) {
			for (size_t i = 0; i < I; i++) {
				const uint32_t A_sp_addr = A_sp_addr_start + (i*K + k)*DIM;
				const uint32_t B_sp_addr = B_sp_addr_start + (k*J + j)*DIM;
		    	const uint32_t C_sp_addr = C_sp_addr_start + (i*J + j)*DIM;
				// Mvin A
				if (j == 0 && k % A_blocks == 0) {
					const elem_t * const A_dram_addr = A + (i*A_row_stride + k)*DIM;
					const size_t blocks = k + A_blocks <= K ? A_blocks : K-k;
					const size_t cols = blocks * DIM;
					const size_t rows = DIM;
					gemmini_extended_mvin(A_dram_addr, A_sp_addr, cols, rows);
          printf("gemmini_extended_mvin(A + %p, %p, %u, %u);\n", (i*A_row_stride + k)*DIM, A_sp_addr, cols, rows);
				}
				// Mvin B
				if (i == 0 && j % B_blocks == 0) {
					const elem_t * const B_dram_addr = B + (k*B_row_stride + j)*DIM;
					const size_t blocks = j + B_blocks <= J ? B_blocks : J-j;
					const size_t cols = blocks * DIM;
					const size_t rows = DIM;
					gemmini_extended_mvin2(B_dram_addr, B_sp_addr, cols, rows);
          printf("gemmini_extended_mvin2(B + %p, %p, %u, %u);\n", (k*B_row_stride+j)*DIM, B_sp_addr, cols, rows);
				}
 		    // Compute
        uint32_t pre_sp_addr = i == 0 ? B_sp_addr : GARBAGE_ADDR;
        uint32_t out_sp_addr = C_sp_addr;
        const size_t A_cols = DIM;
        const size_t A_rows = DIM;
        const size_t B_cols = DIM;
        const size_t B_rows = DIM;
        const size_t C_cols = DIM;
        const size_t C_rows = DIM;
        gemmini_extended_preload(pre_sp_addr, out_sp_addr, B_cols, B_rows, C_cols, C_rows);
        printf("gemmini_extended_preload(%p, 0x%x, %u, %u, %u, %u);\n", pre_sp_addr, out_sp_addr, B_cols, B_rows, C_cols, C_rows);
        if (i == 0) { // First iteration
          gemmini_extended_compute_preloaded(A_sp_addr, GARBAGE_ADDR, A_cols, A_rows, DIM, DIM);
          printf("gemmini_extended_compute_preloaded(%p, 0x%x, %u, %u, %u, %u);\n", A_sp_addr, GARBAGE_ADDR, A_cols, A_rows, DIM, DIM);
        } else { // All other iterations
          gemmini_extended_compute_accumulated(A_sp_addr, GARBAGE_ADDR, A_cols, A_rows, DIM, DIM);
          printf("gemmini_extended_compute_accumulated(%p, 0x%x, %u, %u, %u, %u);\n", A_sp_addr, GARBAGE_ADDR, A_cols, A_rows, DIM, DIM);
        }
			}
		}
	}

  gemmini_fence();
  // Gemmini instructions end

  // Setup end
  setup_end = read_cycles();

  // Microbenchmark start
  start = read_cycles();

  // Main microbenchmark code
  for(int iteration = 0; iteration < iterations; iteration++) {
    for (size_t k = 0; k < K; k++) {
      for (size_t j = 0; j < J; j++) {
        for (size_t i = 0; i < I; i++) {
		      const uint32_t C_sp_addr = C_sp_addr_start + (i*J + j)*DIM;
 		      if (k == K-1) {
            // Move-out C
            if (j == J-1 || j % C_blocks == C_blocks-1) {
              const size_t rounded_j = (j / C_blocks) * C_blocks;
              const uint32_t rounded_C_sp_addr = C_sp_addr_start + (i*J + rounded_j)*DIM;
              void * const C_dram_addr = (int8_t*)C + (i*C_row_stride + rounded_j)*DIM*sizeof(elem_t);
              const size_t blocks = rounded_j + C_blocks <= J ? C_blocks : J-rounded_j;
              const size_t cols = blocks * DIM;
              const size_t rows = DIM;
              gemmini_extended_mvout(C_dram_addr, rounded_C_sp_addr, cols, rows);
              printf("gemmini_extended_mvout(C + %p, 0x%x, %u, %u);\n", (i*C_row_stride+rounded_j)*DIM*sizeof(elem_t), rounded_C_sp_addr, cols, rows);
            }
          }
        }
      }
    }
  }

  gemmini_fence();
  // Gemmini instructions end

  // Microbenchmark end
  end = read_cycles();

  setup_cycles = setup_end - setup_start;
  benchmark_cycles = end - start;
  printf("Setup cycles taken: %u\n", setup_cycles);
  printf("Cycles taken: %u\n", benchmark_cycles);

  exit(0);
}