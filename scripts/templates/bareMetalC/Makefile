include $(abs_top_srcdir)/Makefrag

tests =  simple  matmul_1  matmul_2  matmul_3  matmul_4  matmul_5  matmul_6  matmul_7  matmul_8  matmul_9  matmul_10 \
matmul_1_spike  matmul_2_spike  matmul_3_spike  matmul_4_spike  matmul_5_spike  matmul_6_spike  matmul_7_spike  matmul_8_spike  matmul_9_spike  matmul_10_spike \
mlp_1  mlp_2  mlp_3  mlp_4  mlp_5  mlp_6  mlp_1_spike  mlp_2_spike  mlp_3_spike  mlp_4_spike  mlp_5_spike  mlp_6_spike \
mvin_microbenchmark  mvout_microbenchmark  preload_and_compute_microbenchmark \
mvin_microbenchmark_spike  mvout_microbenchmark_spike  preload_and_compute_microbenchmark_spike \
mvin_microbenchmark_basic  mvout_microbenchmark_basic  preload_and_compute_microbenchmark_basic \
mvin_microbenchmark_basic_spike  mvout_microbenchmark_basic_spike  preload_and_compute_microbenchmark_basic_spike \
mvin_microbenchmark_original  mvout_microbenchmark_original  preload_and_compute_microbenchmark_original \
mvin_microbenchmark_original_spike  mvout_microbenchmark_original_spike  preload_and_compute_microbenchmark_original_spike \
conv_1 conv_2 conv_3 conv_4 conv_5 conv_6 \
conv_1_spike conv_2_spike conv_3_spike conv_4_spike conv_5_spike conv_6_spike

tests_baremetal = $(tests:=-baremetal)

ifeq ($(findstring spike,$(RUNNER)),spike)
# Currently don't support conv or conv-with-pool on spike
runs_baremetal = $(addsuffix .run,$(filter-out conv-baremetal conv_with_pool-baremetal,$(tests_baremetal)))
else
# Don't run very long benchmarks for RTL sim
runs_baremetal = $(addsuffix .run,$(filter-out tiled_matmul_cpu-baremetal tiled_matmul_option-baremetal,$(tests_baremetal)))
endif

ifdef BAREMETAL_ONLY
	tests_linux =
	tests_pk =
else
	tests_linux = $(tests:=-linux)
	tests_pk = $(tests:=-pk)
endif

BENCH_COMMON = $(abs_top_srcdir)/riscv-tests/benchmarks/common
GEMMINI_HEADERS = $(abs_top_srcdir)/include/gemmini.h $(abs_top_srcdir)/include/gemmini_params.h $(abs_top_srcdir)/include/gemmini_testutils.h

CFLAGS := $(CFLAGS) \
	-DPREALLOCATE=1 \
	-DMULTITHREAD=1 \
	-mcmodel=medany \
	-std=gnu99 \
	-O2 \
	-ffast-math \
	-fno-common \
	-fno-builtin-printf \
	-fno-tree-loop-distribute-patterns \
	-march=rv64gc -Wa,-march=rv64gc \
	-lm \
	-lgcc \
	-I$(abs_top_srcdir)/riscv-tests \
	-I$(abs_top_srcdir)/riscv-tests/env \
	-I$(abs_top_srcdir) \
	-I$(BENCH_COMMON) \
	-DID_STRING=$(ID_STRING) \
	-DPRINT_TILE=0 \

CFLAGS_PK := \
	$(CFLAGS) \
	-static \
	-DBAREMETAL=1 \

CFLAGS_BAREMETAL := \
	$(CFLAGS) \
	-nostdlib \
	-nostartfiles \
	-static \
	-T $(BENCH_COMMON)/test.ld \
	-DBAREMETAL=1 \

all: $(tests_baremetal) $(tests_linux) $(tests_pk)

vpath %.c $(src_dir)

%-baremetal: %.c $(GEMMINI_HEADERS)
	$(CC_BAREMETAL) $(CFLAGS_BAREMETAL) $< $(LFLAGS) -o $@ \
		$(wildcard $(BENCH_COMMON)/*.c) $(wildcard $(BENCH_COMMON)/*.S) $(LIBS)

%-linux: %.c $(GEMMINI_HEADERS)
	$(CC_LINUX) $(CFLAGS) $< $(LFLAGS) -o $@

%-pk: %.c $(GEMMINI_HEADERS)
	$(CC_LINUX) $(CFLAGS_PK) $< $(LFLAGS) -o $@

run-baremetal: $(runs_baremetal)

%-baremetal.run: %-baremetal
	$(RUNNER)$(abs_top_srcdir)/build/bareMetalC/$^

junk += $(tests_baremetal) $(tests_linux) $(tests_pk)

