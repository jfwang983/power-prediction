# Setup
1. Initialize repositories
    ```bash
    ./init.sh
    ```
2. Set clock period in power-mappings-chipyard/vlsi/custom.yml
    ```yaml
    # Clock
    vlsi.inputs.clocks: [
      {name: clock_uncore_clock, period: 16.0ns, uncertainty: 0.1ns}
    ]
    ```

4. Modify Gemmini config in power-mappings-chipyard/generators/gemmini/src/main/scala/gemmini/CustomConfigs.scala
    ```scala
    val baselineInferenceConfig = defaultConfig.copy(
      acc_singleported = true, // Add this
      acc_sub_banks = 2, // Add this
      has_training_convs = false,
    )
    ```
5. Add clock frequency to SoC config in power-mappings-chipyard/generators/gemmini/src/main/scala/gemmini/CustomSoCConfigs.scala
    ```scala
    // Set the number of CPUs you want to create
    new chipyard.CustomGemmminiCPUConfigs.CustomCPU(1) ++

    new chipyard.config.WithSystemBusWidth(GemminiCustomConfigs.customConfig.dma_buswidth) ++
    // 16ns period --> 62.5 MHz freq
    new chipyard.config.WithTileFrequency(62.5) ++ // Add this
    new chipyard.config.WithSystemBusFrequency(62.5) ++ // Add this
    new chipyard.config.WithMemoryBusFrequency(62.5) ++ // Add this
    new chipyard.config.WithPeripheryBusFrequency(62.5) ++ // Add this
    new chipyard.config.AbstractConfig
    ```
6. Generate Gemmini RTL from configs
    ```bash
    cd ~power-mappings-chipyard/generators/gemmini
    ./scripts/setup-paths.sh
    cd software/gemmini-rocc-tests
    ./build.sh
    ```
7. Check config and binary in power-mappings-chipyard/vlsi/custom.mk
   ```make
   BINARY             ?= $(RISCV)/riscv64-unknown-elf/share/riscv-tests/benchmarks/towers.riscv # Workload
   CONFIG             ?= CustomGemminiSoCConfig # Must be this config
   ```

8. Create Joules power report
* First time or new RTL
    ```bash
    cd ~power-mappings-chipyard/vlsi
    make buildfile # use flag -B to rebuild everything
    make sim-rtl-debug
    make power-rtl
    ```
*  No RTL change
    ```bash
    cd ~power-mappings-chipyard/vlsi
    make redo-sim-rtl-debug args="--only_step run_simulation"
    make redo-power-rtl args="--only_step report_power"
    ```

# Automate Joules flow
The following scripts are designed to produce Joules reports for different mappings of a workload for Gemmini:
* `scripts/run.sh`: Calls other scripts to generate binaries for mappings, run RTL simulation for binaries, and run Joules for waveforms
* `scripts/create_and_simulate_binaries.sh`: Generates unique mappings and creates baremetal C binaries per mapping
* `scripts/sample_extract.py`: Sorts mappings by EDP
* `power-mappings-chipyard/vlsi/run_rtl_sim.sh`: Runs RTL Sim for baremetal C binary
* `power-mappings-chipyard/vlsi/run_joules.py`: Runs Joules for RTL Sim waveform

1. Generate new workload power data
   ```bash
   ./scripts/run.sh {workload_name} -regen
   ```

2. Generate more workload power data
   ```bash
   ./scripts/run.sh {workload_name} -append
   ```

# Dataset Generator
WIP
