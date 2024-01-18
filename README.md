# Setup
1. This repo assumes miniconda3 is located in ``/scratch/{user}/miniconda3``
2. Initialization
    ```bash
    ./init_all.sh
    ```
    * Runs ``init.sh`` to initialize all repositories
    * Runs ``init_rtl.sh`` to initialize Joules with Gemmini RTL (only needed for first-time setup and new Gemmini configs)

# Customizations
* Clock period in ``power-mappings-chipyard/vlsi/custom.yml``
    ```yaml
    # Clock
    vlsi.inputs.clocks: [
      {name: clock_uncore_clock, period: 16.0ns, uncertainty: 0.1ns}
    ]
    ```
    * Default is 16.0ns
* Gemmini architectural configurations in ``power-mappings-chipyard/generators/gemmini/configs/GemminiCustomConfigs.scala``
    ```scala
    val baselineInferenceConfig = defaultConfig.copy(
      acc_singleported = true, // Add this
      acc_sub_banks = 2, // Add this
      has_training_convs = false,
    )
    ```
    * ``acc_singleported`` and ``acc_sub_banks`` are used for compatability with SKY130 SRAMs; do not change these
* SoC Clock frequency in ``power-mappings-chipyard/generators/chipyard/src/main/scala/config/GemminiSoCConfigs.scala``
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
    * Change clock frequency dependent on clock period specified in ``power-mappings-chipyard/vlsi/custom.yml``
* Binary and config in ``power-mappings-chipyard/vlsi/custom.mk``
   ```make
   BINARY             ?= $(RISCV)/riscv64-unknown-elf/share/riscv-tests/benchmarks/towers.riscv # Workload
   CONFIG             ?= CustomGemminiSoCConfig
   ```
* CustomGemminiSoCConfig is the default Gemmini config compatible with SKY130 SRAMs
* Binaries must be baremetal C binaries

# Using Joules
* New RTL
    1. Run RTL Simulation
       ```bash
       make buildfile # use flag -B to rebuild everything
       make sim-rtl-debug CONFIG=CustomGemminiSoCConfig BINARY={binary_name} LOADMEM={binary_name}
       ```
    2. Set RTL Simulation ``.fsdb`` file for Joules to analyze and name the name the power report
       ```make
       power.inputs:
           level: "rtl"
           waveforms: []
           report_configs:
               - waveform_path: output/chipyard.harness.TestHarness.CustomGemminiSoCConfig/{binary_name}.fsdb
                 inst: /ChipTop/system/tile_prci_domain/tile_reset_domain_tile/gemmini # Can be specific module
                 report_name: {report_name}
                 output_formats:
                     - report
       ```
    3. Run Joules
       ```bash
       make power-rtl
       ```
* Used/Already-analyzed RTL
    1. Run RTL Simulation
       ```bash
       make buildfile # use flag -B to rebuild everything
       make redo-sim-rtl-debug CONFIG=CustomGemminiSoCConfig BINARY={binary_name} LOADMEM={binary_name} args="--only_step run_simulation"
       ```
    2. Set RTL Simulation ``.fsdb`` file for Joules to analyze and name the name the power report
       ```make
       power.inputs:
           level: "rtl"
           waveforms: []
           report_configs:
               - waveform_path: output/chipyard.harness.TestHarness.CustomGemminiSoCConfig/{binary_name}.fsdb
                 inst: /ChipTop/system/tile_prci_domain/tile_reset_domain_tile/gemmini # Can be specific module
                 report_name: {report_name}
                 output_formats:
                     - report
       ```
    3. Run Joules
       ```bash
       make redo-power-rtl args="--only_step report_power"
       ```

# Automating Joules
The following scripts are designed to produce Joules reports for different mappings of a workload for Gemmini:
* ``scripts/run.sh``: Calls other scripts to generate binaries for mappings, run RTL simulation for binaries, and run Joules for waveforms
* ``scripts/create_and_simulate_binaries.sh``: Generates unique mappings and creates baremetal C binaries per mapping
* ``scripts/sample_extract.py``: Sorts mappings by EDP
* ``power-mappings-chipyard/vlsi/run_rtl_sim.sh``: Runs RTL Sim for baremetal C binary
* ``power-mappings-chipyard/vlsi/run_joules.py``: Runs Joules for RTL Sim waveform

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
