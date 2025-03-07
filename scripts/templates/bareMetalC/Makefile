include $(abs_top_srcdir)/Makefrag

tests = mvin_microbenchmark_mult_addr_basic_1_col	mvin_microbenchmark_mult_addr_basic_2_col	mvin_microbenchmark_mult_addr_basic_4_col	mvin_microbenchmark_mult_addr_basic_8_col \
mvin_microbenchmark_mult_addr_basic_16_col	mvin_microbenchmark_mult_addr_basic_32_col	mvin_microbenchmark_mult_addr_basic_64_col \
preload_and_compute_microbenchmark_10_iter_basic	preload_and_compute_microbenchmark_100_iter_basic	preload_and_compute_microbenchmark_1000_iter_basic \
preload_and_compute_microbenchmark_addr_acc_basic	preload_and_compute_microbenchmark_addr_overwrite_basic \
mvout_microbenchmark_mult_addr_basic_1_col	mvout_microbenchmark_mult_addr_basic_2_col	mvout_microbenchmark_mult_addr_basic_4_col	mvout_microbenchmark_mult_addr_basic_8_col \
mvout_microbenchmark_mult_addr_basic_16_col	mvout_microbenchmark_mult_addr_basic_32_col	mvout_microbenchmark_mult_addr_basic_64_col \
simple

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

