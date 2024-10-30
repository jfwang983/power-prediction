/*
package chipyard

import org.chipsalliance.cde.config.Config
import gemmini.{GemminiCustomConfig, GemminiCustomConfigs}

class CustomGemminiSoCConfig extends Config(
  new gemmini.GemminiCustomConfig ++

  // No L2 Cache
  new chipyard.config.WithBroadcastManager ++

  // // Set your custom L2 configs
  // new chipyard.config.WithL2TLBs(512) ++

  // new freechips.rocketchip.subsystem.WithInclusiveCache(
  //   nWays = 8,
  //   capacityKB = 512,
  //   outerLatencyCycles = 40,
  //   subBankingFactor = 4
  // ) ++

  // Set the number of CPUs you want to create
  new chipyard.CustomGemmminiCPUConfigs.CustomCPU(1) ++

  new chipyard.config.WithSystemBusWidth(GemminiCustomConfigs.customConfig.dma_buswidth) ++
  new chipyard.config.WithTileFrequency(100) ++    // 10ns period --> 100 MHz freq
  new chipyard.config.WithSystemBusFrequency(100) ++
  new chipyard.config.WithMemoryBusFrequency(100) ++   
  new chipyard.config.WithPeripheryBusFrequency(100) ++
  new chipyard.config.AbstractConfig
)
*/