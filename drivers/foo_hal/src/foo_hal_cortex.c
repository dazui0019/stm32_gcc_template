#include <foo_hal_cortex.h>
#include <common_defines.h>
#include <check.h>
#include "stm32f4xx_ll_rcc.h"
#include "stm32f4xx_ll_bus.h"
#include "stm32f4xx_ll_system.h"
#include "stm32f4xx_ll_exti.h"
#include "stm32f4xx_ll_cortex.h"
#include "stm32f4xx_ll_utils.h"
#include "stm32f4xx_ll_pwr.h"
#include "stm32f4xx_ll_dma.h"
#include "stm32f4xx_ll_gpio.h"

#include <stm32f4xx.h>

#define FOO_HAL_CORTEX_INSTRUCTIONS_PER_MICROSECOND (SystemCoreClock / 1000000)

void foo_hal_cortex_init_early(void) {
    // CoreDebug->DEMCR |= (CoreDebug_DEMCR_TRCENA_Msk | CoreDebug_DEMCR_MON_EN_Msk);
    // DWT->CTRL |= DWT_CTRL_CYCCNTENA_Msk;
    // DWT->CYCCNT = 0U;

    // /* Enable instruction prefetch */
    // SET_BIT(FLASH->ACR, FLASH_ACR_PRFTEN);
    /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
    LL_APB2_GRP1_EnableClock(LL_APB2_GRP1_PERIPH_SYSCFG);
    LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_PWR);

    /* System interrupt init*/
    NVIC_SetPriorityGrouping((uint32_t)0x00000003);

    /* SysTick_IRQn interrupt configuration */
    NVIC_SetPriority(SysTick_IRQn, NVIC_EncodePriority(NVIC_GetPriorityGrouping(),15, 0));
}

void foo_hal_cortex_delay_us(uint32_t microseconds) {
    foo_check(microseconds < (UINT32_MAX / FOO_HAL_CORTEX_INSTRUCTIONS_PER_MICROSECOND));

    uint32_t start = DWT->CYCCNT;
    uint32_t time_ticks = FOO_HAL_CORTEX_INSTRUCTIONS_PER_MICROSECOND * microseconds;

    while((DWT->CYCCNT - start) < time_ticks) {
    };
}

uint32_t foo_hal_cortex_instructions_per_microsecond(void) {
    return FOO_HAL_CORTEX_INSTRUCTIONS_PER_MICROSECOND;
}

FOO_WARN_UNUSED FooHalCortexTimer foo_hal_cortex_timer_get(uint32_t timeout_us) {
    foo_check(timeout_us < (UINT32_MAX / FOO_HAL_CORTEX_INSTRUCTIONS_PER_MICROSECOND));

    FooHalCortexTimer cortex_timer = {0};
    cortex_timer.start = DWT->CYCCNT;
    cortex_timer.value = FOO_HAL_CORTEX_INSTRUCTIONS_PER_MICROSECOND * timeout_us;
    return cortex_timer;
}

bool foo_hal_cortex_timer_is_expired(FooHalCortexTimer cortex_timer) {
    return !((DWT->CYCCNT - cortex_timer.start) < cortex_timer.value);
}

void foo_hal_cortex_timer_wait(FooHalCortexTimer cortex_timer) {
    while(!foo_hal_cortex_timer_is_expired(cortex_timer))
        ;
}

// Duck ST
#undef COMP0
#undef COMP1
#undef COMP2
#undef COMP3

void foo_hal_cortex_comp_enable(
    FooHalCortexComp comp,
    FooHalCortexCompFunction function,
    uint32_t value,
    uint32_t mask,
    FooHalCortexCompSize size) {
    uint32_t function_reg = (uint32_t)function | ((uint32_t)size << 10);

    switch(comp) {
    case FooHalCortexComp0:
        (DWT->COMP0) = value;
        (DWT->MASK0) = mask;
        (DWT->FUNCTION0) = function_reg;
        break;
    case FooHalCortexComp1:
        (DWT->COMP1) = value;
        (DWT->MASK1) = mask;
        (DWT->FUNCTION1) = function_reg;
        break;
    case FooHalCortexComp2:
        (DWT->COMP2) = value;
        (DWT->MASK2) = mask;
        (DWT->FUNCTION2) = function_reg;
        break;
    case FooHalCortexComp3:
        (DWT->COMP3) = value;
        (DWT->MASK3) = mask;
        (DWT->FUNCTION3) = function_reg;
        break;
    default:
        foo_crash("Invalid parameter");
    }
}

void foo_hal_cortex_comp_reset(FooHalCortexComp comp) {
    switch(comp) {
    case FooHalCortexComp0:
        (DWT->COMP0) = 0;
        (DWT->MASK0) = 0;
        (DWT->FUNCTION0) = 0;
        break;
    case FooHalCortexComp1:
        (DWT->COMP1) = 0;
        (DWT->MASK1) = 0;
        (DWT->FUNCTION1) = 0;
        break;
    case FooHalCortexComp2:
        (DWT->COMP2) = 0;
        (DWT->MASK2) = 0;
        (DWT->FUNCTION2) = 0;
        break;
    case FooHalCortexComp3:
        (DWT->COMP3) = 0;
        (DWT->MASK3) = 0;
        (DWT->FUNCTION3) = 0;
        break;
    default:
        foo_crash("Invalid parameter");
    }
}
