#include <furi_hal_clock.h>
#include <foo.h>

#include <stm32f4xx_ll_pwr.h>
#include <stm32f4xx_ll_rcc.h>
#include <stm32f4xx_ll_utils.h>
#include <stm32f4xx_ll_cortex.h>

#define TAG "FuriHalClock"

#define CPU_CLOCK_EARLY_HZ 4000000
#define CPU_CLOCK_HSI16_HZ 16000000
#define CPU_CLOCK_HSE_HZ   8000000
#define CPU_CLOCK_PLL_HZ   180000000

#define TICK_INT_PRIORITY   15U
#define HS_CLOCK_IS_READY() (LL_RCC_HSE_IsReady() && LL_RCC_HSI_IsReady())
#define LS_CLOCK_IS_READY() (LL_RCC_LSE_IsReady() && LL_RCC_LSI1_IsReady())

void furi_hal_clock_init_early(void) {
    LL_SetSystemCoreClock(CPU_CLOCK_EARLY_HZ);
    LL_Init1msTick(CPU_CLOCK_PLL_HZ);
}

void furi_hal_clock_deinit_early(void) {
}

void furi_hal_clock_init(void) {
  LL_FLASH_SetLatency(LL_FLASH_LATENCY_5);
  while(LL_FLASH_GetLatency()!= LL_FLASH_LATENCY_5)
  {
  }
  LL_PWR_SetRegulVoltageScaling(LL_PWR_REGU_VOLTAGE_SCALE1);
  LL_PWR_EnableOverDriveMode();
  LL_RCC_HSE_Enable();

   /* Wait till HSE is ready */
  while(LL_RCC_HSE_IsReady() != 1)
  {

  }
  LL_RCC_PLL_ConfigDomain_SYS(LL_RCC_PLLSOURCE_HSE, LL_RCC_PLLM_DIV_4, 180, LL_RCC_PLLP_DIV_2);
  LL_RCC_PLL_Enable();

   /* Wait till PLL is ready */
  while(LL_RCC_PLL_IsReady() != 1)
  {

  }
  while (LL_PWR_IsActiveFlag_VOS() == 0)
  {
  }
  LL_RCC_SetAHBPrescaler(LL_RCC_SYSCLK_DIV_1);
  LL_RCC_SetAPB1Prescaler(LL_RCC_APB1_DIV_4);
  LL_RCC_SetAPB2Prescaler(LL_RCC_APB2_DIV_2);
  LL_RCC_SetSysClkSource(LL_RCC_SYS_CLKSOURCE_PLL);

   /* Wait till System clock is ready */
  while(LL_RCC_GetSysClkSource() != LL_RCC_SYS_CLKSOURCE_STATUS_PLL)
  {

  }
  LL_Init1msTick(CPU_CLOCK_PLL_HZ);
  LL_SetSystemCoreClock(CPU_CLOCK_PLL_HZ);
  LL_RCC_SetTIMPrescaler(LL_RCC_TIM_PRESCALER_TWICE);
}

// void furi_hal_clock_switch_hse2hsi(void) {
//     LL_RCC_HSI_Enable();

//     while(!LL_RCC_HSI_IsReady())
//         ;

//     LL_RCC_SetSysClkSource(LL_RCC_SYS_CLKSOURCE_HSI);
//     furi_check(LL_RCC_GetSMPSClockSelection() == LL_RCC_SMPS_CLKSOURCE_HSI);

//     while(LL_RCC_GetSysClkSource() != LL_RCC_SYS_CLKSOURCE_STATUS_HSI)
//         ;

//     LL_FLASH_SetLatency(LL_FLASH_LATENCY_0);
//     while(LL_FLASH_GetLatency() != LL_FLASH_LATENCY_0)
//         ;
// }

// void furi_hal_clock_switch_hsi2hse(void) {
// #ifdef FURI_HAL_CLOCK_TRACK_STARTUP
//     uint32_t clock_start_time = DWT->CYCCNT;
// #endif

//     LL_RCC_HSE_Enable();
//     while(!LL_RCC_HSE_IsReady())
//         ;

//     LL_FLASH_SetLatency(LL_FLASH_LATENCY_1);
//     while(LL_FLASH_GetLatency() != LL_FLASH_LATENCY_1)
//         ;

//     LL_RCC_SetSysClkSource(LL_RCC_SYS_CLKSOURCE_HSE);

//     while(LL_RCC_GetSysClkSource() != LL_RCC_SYS_CLKSOURCE_STATUS_HSE)
//         ;

// #ifdef FURI_HAL_CLOCK_TRACK_STARTUP
//     uint32_t total = DWT->CYCCNT - clock_start_time;
//     if(total > (20 * 0x148)) {
//         furi_crash("Slow HSE/PLL startup");
//     }
// #endif
// }

// bool furi_hal_clock_switch_hse2pll(void) {
//     furi_check(LL_RCC_GetSysClkSource() == LL_RCC_SYS_CLKSOURCE_STATUS_HSE);

//     LL_RCC_PLL_Enable();
//     LL_RCC_PLLSAI1_Enable();

//     while(!LL_RCC_PLL_IsReady())
//         ;
//     while(!LL_RCC_PLLSAI1_IsReady())
//         ;

//     // This API returns garbage if stack version < 1.20.0
//     SHCI_C2_SetSystemClock(SET_SYSTEM_CLOCK_HSE_TO_PLL);
//     // So we'll check results by asking hardware directly
//     if(LL_RCC_GetSysClkSource() != LL_RCC_SYS_CLKSOURCE_STATUS_PLL) {
//         return false;
//     }

//     LL_SetSystemCoreClock(CPU_CLOCK_PLL_HZ);
//     SysTick->LOAD = (uint32_t)((SystemCoreClock / 1000) - 1UL);

//     return true;
// }

// bool furi_hal_clock_switch_pll2hse(void) {
//     furi_check(LL_RCC_GetSysClkSource() == LL_RCC_SYS_CLKSOURCE_STATUS_PLL);

//     LL_RCC_HSE_Enable();
//     while(!LL_RCC_HSE_IsReady())
//         ;

//     // This API returns garbage if stack version < 1.20.0
//     SHCI_C2_SetSystemClock(SET_SYSTEM_CLOCK_PLL_ON_TO_HSE);
//     // So we'll check results by asking hardware directly
//     if(LL_RCC_GetSysClkSource() != LL_RCC_SYS_CLKSOURCE_STATUS_HSE) {
//         return false;
//     }

//     LL_SetSystemCoreClock(CPU_CLOCK_HSE_HZ);
//     SysTick->LOAD = (uint32_t)((SystemCoreClock / 1000) - 1UL);

//     return true;
// }

// void furi_hal_clock_suspend_tick(void) {
//     CLEAR_BIT(SysTick->CTRL, SysTick_CTRL_ENABLE_Msk);
// }

// void furi_hal_clock_resume_tick(void) {
//     SET_BIT(SysTick->CTRL, SysTick_CTRL_ENABLE_Msk);
// }

// void furi_hal_clock_mco_enable(FuriHalClockMcoSourceId source, FuriHalClockMcoDivisorId div) {
//     if(source == FuriHalClockMcoLse) {
//         LL_RCC_ConfigMCO(LL_RCC_MCO1SOURCE_LSE, div);
//     } else if(source == FuriHalClockMcoSysclk) {
//         LL_RCC_ConfigMCO(LL_RCC_MCO1SOURCE_SYSCLK, div);
//     } else {
//         LL_RCC_MSI_Enable();
//         while(LL_RCC_MSI_IsReady() != 1)
//             ;
//         switch(source) {
//         case FuriHalClockMcoMsi100k:
//             LL_RCC_MSI_SetRange(LL_RCC_MSIRANGE_0);
//             break;
//         case FuriHalClockMcoMsi200k:
//             LL_RCC_MSI_SetRange(LL_RCC_MSIRANGE_1);
//             break;
//         case FuriHalClockMcoMsi400k:
//             LL_RCC_MSI_SetRange(LL_RCC_MSIRANGE_2);
//             break;
//         case FuriHalClockMcoMsi800k:
//             LL_RCC_MSI_SetRange(LL_RCC_MSIRANGE_3);
//             break;
//         case FuriHalClockMcoMsi1m:
//             LL_RCC_MSI_SetRange(LL_RCC_MSIRANGE_4);
//             break;
//         case FuriHalClockMcoMsi2m:
//             LL_RCC_MSI_SetRange(LL_RCC_MSIRANGE_5);
//             break;
//         case FuriHalClockMcoMsi4m:
//             LL_RCC_MSI_SetRange(LL_RCC_MSIRANGE_6);
//             break;
//         case FuriHalClockMcoMsi8m:
//             LL_RCC_MSI_SetRange(LL_RCC_MSIRANGE_7);
//             break;
//         case FuriHalClockMcoMsi16m:
//             LL_RCC_MSI_SetRange(LL_RCC_MSIRANGE_8);
//             break;
//         case FuriHalClockMcoMsi24m:
//             LL_RCC_MSI_SetRange(LL_RCC_MSIRANGE_9);
//             break;
//         case FuriHalClockMcoMsi32m:
//             LL_RCC_MSI_SetRange(LL_RCC_MSIRANGE_10);
//             break;
//         case FuriHalClockMcoMsi48m:
//             LL_RCC_MSI_SetRange(LL_RCC_MSIRANGE_11);
//             break;
//         default:
//             break;
//         }
//         LL_RCC_ConfigMCO(LL_RCC_MCO1SOURCE_MSI, div);
//     }
// }

// void furi_hal_clock_mco_disable(void) {
//     LL_RCC_ConfigMCO(LL_RCC_MCO1SOURCE_NOCLOCK, FuriHalClockMcoDiv1);
//     LL_RCC_MSI_Disable();
//     while(LL_RCC_MSI_IsReady() != 0)
//         ;
// }
