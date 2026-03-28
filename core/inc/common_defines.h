#pragma once

#include "core_defines.h"
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#define FOO_NORETURN [[noreturn]]
#else
#include <stdnoreturn.h>
#define FOO_NORETURN noreturn
#endif

#include <cmsis_compiler.h>

#ifndef FOO_WARN_UNUSED
#define FOO_WARN_UNUSED __attribute__((warn_unused_result))
#endif

#ifndef FOO_DEPRECATED
#define FOO_DEPRECATED __attribute__((deprecated))
#endif

#ifndef FOO_WEAK
#define FOO_WEAK __attribute__((weak))
#endif

#ifndef FOO_PACKED
#define FOO_PACKED __attribute__((packed))
#endif

#ifndef FOO_ALWAYS_INLINE
#define FOO_ALWAYS_INLINE __attribute__((always_inline)) inline
#endif

#ifndef FOO_IS_IRQ_MASKED
#define FOO_IS_IRQ_MASKED() (__get_PRIMASK() != 0U)
#endif

#ifndef FOO_IS_IRQ_MODE
#define FOO_IS_IRQ_MODE() (__get_IPSR() != 0U)
#endif

#ifndef FOO_IS_ISR
#define FOO_IS_ISR() (FOO_IS_IRQ_MODE() || FOO_IS_IRQ_MASKED())
#endif

typedef struct {
    uint32_t isrm;
    bool from_isr;
    bool kernel_running;
} __FooCriticalInfo;

__FooCriticalInfo __foo_critical_enter(void);

void __foo_critical_exit(__FooCriticalInfo info);

#ifndef FOO_CRITICAL_ENTER
#define FOO_CRITICAL_ENTER() __FooCriticalInfo __foo_critical_info = __foo_critical_enter();
#endif

#ifndef FOO_CRITICAL_EXIT
#define FOO_CRITICAL_EXIT() __foo_critical_exit(__foo_critical_info);
#endif

#ifndef FOO_CHECK_RETURN
#define FOO_CHECK_RETURN __attribute__((__warn_unused_result__))
#endif

#ifndef FOO_NAKED
#define FOO_NAKED __attribute__((naked))
#endif

#ifndef FOO_DEFAULT
#define FOO_DEFAULT(x) __attribute__((weak, alias(x)))
#endif

#ifdef __cplusplus
}
#endif
