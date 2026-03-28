#pragma once
#include "stdbool.h"
#include <stm32f4xx_ll_gpio.h>
#include <stm32f4xx_ll_system.h>
#include <stm32f4xx_ll_exti.h>
#include "check.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Number of gpio on one port
 */
#define GPIO_NUMBER (16U)

/**
 * Interrupt callback prototype
 */
typedef void (*GpioExtiCallback)(void* ctx);

/**
 * Gpio interrupt type
 */
typedef struct {
    GpioExtiCallback callback;
    void* context;
} GpioInterrupt;

/**
 * Gpio modes
 */
typedef enum {
    GpioModeInput,
    GpioModeOutputPushPull,
    GpioModeOutputOpenDrain,
    GpioModeAltFunctionPushPull,
    GpioModeAltFunctionOpenDrain,
    GpioModeAnalog,
    GpioModeInterruptRise,
    GpioModeInterruptFall,
    GpioModeInterruptRiseFall,
    GpioModeEventRise,
    GpioModeEventFall,
    GpioModeEventRiseFall,
} GpioMode;

/**
 * Gpio pull modes
 */
typedef enum {
    GpioPullNo,
    GpioPullUp,
    GpioPullDown,
} GpioPull;

/**
 * Gpio speed modes
 */
typedef enum {
    GpioSpeedLow,
    GpioSpeedMedium,
    GpioSpeedHigh,
    GpioSpeedVeryHigh,
} GpioSpeed;

/**
 * Gpio alternate functions
 *
 * Aligned with STM32F446xx official GPIO_AFx_* definitions.
 */
typedef enum {
    GpioAltFn0RTC_50Hz = 0, /*!< RTC_50Hz Alternate Function mapping */
    GpioAltFn0MCO = 0, /*!< MCO (MCO1 and MCO2) Alternate Function mapping */
    GpioAltFn0TAMPER = 0, /*!< TAMPER (TAMPER_1 and TAMPER_2) Alternate Function mapping */
    GpioAltFn0SWJ = 0, /*!< SWJ (SWD and JTAG) Alternate Function mapping */
    GpioAltFn0TRACE = 0, /*!< TRACE Alternate Function mapping */

    GpioAltFn1TIM1 = 1, /*!< TIM1 Alternate Function mapping */
    GpioAltFn1TIM2 = 1, /*!< TIM2 Alternate Function mapping */

    GpioAltFn2TIM3 = 2, /*!< TIM3 Alternate Function mapping */
    GpioAltFn2TIM4 = 2, /*!< TIM4 Alternate Function mapping */
    GpioAltFn2TIM5 = 2, /*!< TIM5 Alternate Function mapping */

    GpioAltFn3TIM8 = 3, /*!< TIM8 Alternate Function mapping */
    GpioAltFn3TIM9 = 3, /*!< TIM9 Alternate Function mapping */
    GpioAltFn3TIM10 = 3, /*!< TIM10 Alternate Function mapping */
    GpioAltFn3TIM11 = 3, /*!< TIM11 Alternate Function mapping */
    GpioAltFn3CEC = 3, /*!< CEC Alternate Function mapping */

    GpioAltFn4I2C1 = 4, /*!< I2C1 Alternate Function mapping */
    GpioAltFn4I2C2 = 4, /*!< I2C2 Alternate Function mapping */
    GpioAltFn4I2C3 = 4, /*!< I2C3 Alternate Function mapping */
    GpioAltFn4FMPI2C1 = 4, /*!< FMPI2C1 Alternate Function mapping */
    GpioAltFn4CEC = 4, /*!< CEC Alternate Function mapping */

    GpioAltFn5SPI1 = 5, /*!< SPI1/I2S1 Alternate Function mapping */
    GpioAltFn5SPI2 = 5, /*!< SPI2/I2S2 Alternate Function mapping */
    GpioAltFn5SPI3 = 5, /*!< SPI3/I2S3 Alternate Function mapping */
    GpioAltFn5SPI4 = 5, /*!< SPI4 Alternate Function mapping */

    GpioAltFn6SPI2 = 6, /*!< SPI2/I2S2 Alternate Function mapping */
    GpioAltFn6SPI3 = 6, /*!< SPI3/I2S3 Alternate Function mapping */
    GpioAltFn6SPI4 = 6, /*!< SPI4 Alternate Function mapping */
    GpioAltFn6SAI1 = 6, /*!< SAI1 Alternate Function mapping */

    GpioAltFn7USART1 = 7, /*!< USART1 Alternate Function mapping */
    GpioAltFn7USART2 = 7, /*!< USART2 Alternate Function mapping */
    GpioAltFn7USART3 = 7, /*!< USART3 Alternate Function mapping */
    GpioAltFn7UART5 = 7, /*!< UART5 Alternate Function mapping */
    GpioAltFn7SPI2 = 7, /*!< SPI2/I2S2 Alternate Function mapping */
    GpioAltFn7SPI3 = 7, /*!< SPI3/I2S3 Alternate Function mapping */
    GpioAltFn7SPDIFRX = 7, /*!< SPDIFRX Alternate Function mapping */

    GpioAltFn8UART4 = 8, /*!< UART4 Alternate Function mapping */
    GpioAltFn8UART5 = 8, /*!< UART5 Alternate Function mapping */
    GpioAltFn8USART6 = 8, /*!< USART6 Alternate Function mapping */
    GpioAltFn8SPDIFRX = 8, /*!< SPDIFRX Alternate Function mapping */
    GpioAltFn8SAI2 = 8, /*!< SAI2 Alternate Function mapping */

    GpioAltFn9CAN1 = 9, /*!< CAN1 Alternate Function mapping */
    GpioAltFn9CAN2 = 9, /*!< CAN2 Alternate Function mapping */
    GpioAltFn9TIM12 = 9, /*!< TIM12 Alternate Function mapping */
    GpioAltFn9TIM13 = 9, /*!< TIM13 Alternate Function mapping */
    GpioAltFn9TIM14 = 9, /*!< TIM14 Alternate Function mapping */
    GpioAltFn9QSPI = 9, /*!< QSPI Alternate Function mapping */

    GpioAltFn10OTG_FS = 10, /*!< OTG_FS Alternate Function mapping */
    GpioAltFn10OTG_HS = 10, /*!< OTG_HS Alternate Function mapping */
    GpioAltFn10SAI2 = 10, /*!< SAI2 Alternate Function mapping */
    GpioAltFn10QSPI = 10, /*!< QSPI Alternate Function mapping */

    GpioAltFn11ETH = 11, /*!< ETHERNET Alternate Function mapping */

    GpioAltFn12FMC = 12, /*!< FMC Alternate Function mapping */
    GpioAltFn12OTG_HS_FS = 12, /*!< OTG HS configured in FS Alternate Function mapping */
    GpioAltFn12SDIO = 12, /*!< SDIO Alternate Function mapping */

    GpioAltFn13DCMI = 13, /*!< DCMI Alternate Function mapping */

    GpioAltFn15EVENTOUT = 15, /*!< EVENTOUT Alternate Function mapping */

    GpioAltFnUnused = 16, /*!< Dummy value, not a hardware AF */
} GpioAltFn;

/**
 * Gpio structure
 */
typedef struct {
    GPIO_TypeDef* port;
    uint16_t pin;
} GpioPin;

/**
 * GPIO initialization function, simple version
 * @param gpio  GpioPin
 * @param mode  GpioMode
 */
void foo_hal_gpio_init_simple(const GpioPin* gpio, const GpioMode mode);

/**
 * GPIO initialization function, normal version
 * @param gpio  GpioPin
 * @param mode  GpioMode
 * @param pull  GpioPull
 * @param speed GpioSpeed
 */
void foo_hal_gpio_init(
    const GpioPin* gpio,
    const GpioMode mode,
    const GpioPull pull,
    const GpioSpeed speed);

/**
 * GPIO initialization function, extended version
 * @param gpio  GpioPin
 * @param mode  GpioMode
 * @param pull  GpioPull
 * @param speed GpioSpeed
 * @param alt_fn GpioAltFn
 */
void foo_hal_gpio_init_ex(
    const GpioPin* gpio,
    const GpioMode mode,
    const GpioPull pull,
    const GpioSpeed speed,
    const GpioAltFn alt_fn);

/**
 * Add and enable interrupt
 * @param gpio GpioPin
 * @param cb   GpioExtiCallback
 * @param ctx  context for callback
 */
void foo_hal_gpio_add_int_callback(const GpioPin* gpio, GpioExtiCallback cb, void* ctx);

/**
 * Enable interrupt
 * @param gpio GpioPin
 */
void foo_hal_gpio_enable_int_callback(const GpioPin* gpio);

/**
 * Disable interrupt
 * @param gpio GpioPin
 */
void foo_hal_gpio_disable_int_callback(const GpioPin* gpio);

/**
 * Remove interrupt
 * @param gpio GpioPin
 */
void foo_hal_gpio_remove_int_callback(const GpioPin* gpio);

/**
 * GPIO write pin
 * @param gpio  GpioPin
 * @param state true / false
 */
static inline void foo_hal_gpio_write(const GpioPin* gpio, const bool state) {
    // writing to BSSR is an atomic operation
    if(state == true) {
        gpio->port->BSRR = gpio->pin;
    } else {
        gpio->port->BSRR = (uint32_t)gpio->pin << GPIO_NUMBER;
    }
}

/**
 * GPIO read pin
 * @param port GPIO port
 * @param pin pin mask
 * @return true / false
 */
static inline void
    foo_hal_gpio_write_port_pin(GPIO_TypeDef* port, uint16_t pin, const bool state) {
    // writing to BSSR is an atomic operation
    if(state == true) {
        port->BSRR = pin;
    } else {
        port->BSRR = pin << GPIO_NUMBER;
    }
}

/**
 * GPIO read pin
 * @param gpio GpioPin
 * @return true / false
 */
static inline bool foo_hal_gpio_read(const GpioPin* gpio) {
    if((gpio->port->IDR & gpio->pin) != 0x00U) {
        return true;
    } else {
        return false;
    }
}

/**
 * GPIO read pin
 * @param port GPIO port
 * @param pin pin mask
 * @return true / false
 */
static inline bool foo_hal_gpio_read_port_pin(GPIO_TypeDef* port, uint16_t pin) {
    if((port->IDR & pin) != 0x00U) {
        return true;
    } else {
        return false;
    }
}

#ifdef __cplusplus
}
#endif
