/**
 * @file foo_hal_cortex.h
 * ARM Cortex HAL
 */

#pragma once

#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

/** Cortex timer provides high precision low level expiring timer */
typedef struct {
    uint32_t start;
    uint32_t value;
} FooHalCortexTimer;

/** Early init stage for cortex
 */
void foo_hal_cortex_init_early(void);

/** Microseconds delay
 *
 * @param[in]  microseconds  The microseconds to wait
 */
void foo_hal_cortex_delay_us(uint32_t microseconds);

/** Get instructions per microsecond count
 *
 * @return     instructions per microsecond count
 */
uint32_t foo_hal_cortex_instructions_per_microsecond(void);

/** Get Timer
 *
 * @param[in]  timeout_us  The expire timeout in us
 *
 * @return     The FooHalCortexTimer
 */
FooHalCortexTimer foo_hal_cortex_timer_get(uint32_t timeout_us);

/** Check if timer expired
 *
 * @param[in]  cortex_timer  The FooHalCortexTimer
 *
 * @return     true if expired
 */
bool foo_hal_cortex_timer_is_expired(FooHalCortexTimer cortex_timer);

/** Wait for timer expire
 *
 * @param[in]  cortex_timer  The FooHalCortexTimer
 */
void foo_hal_cortex_timer_wait(FooHalCortexTimer cortex_timer);

typedef enum {
    FooHalCortexComp0,
    FooHalCortexComp1,
    FooHalCortexComp2,
    FooHalCortexComp3,
} FooHalCortexComp;

typedef enum {
    FooHalCortexCompSizeWord = 0b10,
    FooHalCortexCompSizeHalfWord = 0b01,
    FooHalCortexCompSizeByte = 0b00,
} FooHalCortexCompSize;

typedef enum {
    FooHalCortexCompFunctionPC = 0b100,
    FooHalCortexCompFunctionRead = 0b101,
    FooHalCortexCompFunctionWrite = 0b110,
    FooHalCortexCompFunctionReadWrite = 0b110,
} FooHalCortexCompFunction;

/** Enable DWT comparator
 * 
 * Allows to programmatically set instruction/data breakpoints.
 * 
 * More details on how it works can be found in armv7m official documentation:
 * https://developer.arm.com/documentation/ddi0403/d/Debug-Architecture/ARMv7-M-Debug/The-Data-Watchpoint-and-Trace-unit/The-DWT-comparators
 * https://developer.arm.com/documentation/ddi0403/d/Debug-Architecture/ARMv7-M-Debug/The-Data-Watchpoint-and-Trace-unit/Comparator-Function-registers--DWT-FUNCTIONn
 *
 * @param[in]  comp      The Comparator
 * @param[in]  function  The Comparator Function to use
 * @param[in]  value     The value
 * @param[in]  mask      The mask
 * @param[in]  size      The size
 */
void foo_hal_cortex_comp_enable(
    FooHalCortexComp comp,
    FooHalCortexCompFunction function,
    uint32_t value,
    uint32_t mask,
    FooHalCortexCompSize size);

/** Reset DWT comparator
 *
 * @param[in]  comp  The Comparator
 */
void foo_hal_cortex_comp_reset(FooHalCortexComp comp);

#ifdef __cplusplus
}
#endif
