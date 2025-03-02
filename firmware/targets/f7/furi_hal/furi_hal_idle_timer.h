#pragma once

#include <stm32wbxx_ll_lptim.h>
#include <stm32wbxx_ll_bus.h>
#include <stm32wbxx_ll_rcc.h>
#include <stdint.h>

// Timer used for tickless idle
#define FURI_HAL_IDLE_TIMER_MAX 0xFFFF
#define FURI_HAL_IDLE_TIMER LPTIM2
#define FURI_HAL_IDLE_TIMER_IRQ LPTIM2_IRQn

static inline void furi_hal_idle_timer_init() {
    // Configure clock source
    LL_RCC_SetLPTIMClockSource(LL_RCC_LPTIM2_CLKSOURCE_LSE);
    // Set interrupt priority and enable them
    NVIC_SetPriority(
        FURI_HAL_IDLE_TIMER_IRQ, NVIC_EncodePriority(NVIC_GetPriorityGrouping(), 15, 0));
    NVIC_EnableIRQ(FURI_HAL_IDLE_TIMER_IRQ);
}

static inline void furi_hal_idle_timer_start(uint32_t count) {
    count--;
    // Enable timer
    LL_LPTIM_Enable(FURI_HAL_IDLE_TIMER);
    while(!LL_LPTIM_IsEnabled(FURI_HAL_IDLE_TIMER))
        ;

    // Enable compare match interrupt
    LL_LPTIM_EnableIT_CMPM(FURI_HAL_IDLE_TIMER);

    // Set compare, autoreload and start counter
    // Include some marging to workaround ARRM behaviour
    LL_LPTIM_SetCompare(FURI_HAL_IDLE_TIMER, count - 3);
    LL_LPTIM_SetAutoReload(FURI_HAL_IDLE_TIMER, count);
    LL_LPTIM_StartCounter(FURI_HAL_IDLE_TIMER, LL_LPTIM_OPERATING_MODE_ONESHOT);
}

static inline void furi_hal_idle_timer_reset() {
    // Hard reset timer
    // THE ONLY RELIABLE WAY to stop it according to errata
    LL_LPTIM_DeInit(FURI_HAL_IDLE_TIMER);
    // Prevent IRQ handler call
    NVIC_ClearPendingIRQ(FURI_HAL_IDLE_TIMER_IRQ);
}

static inline uint32_t furi_hal_idle_timer_get_cnt() {
    uint32_t counter = LL_LPTIM_GetCounter(FURI_HAL_IDLE_TIMER);
    uint32_t counter_shadow = LL_LPTIM_GetCounter(FURI_HAL_IDLE_TIMER);
    while(counter != counter_shadow) {
        counter = counter_shadow;
        counter_shadow = LL_LPTIM_GetCounter(FURI_HAL_IDLE_TIMER);
    }
    return counter;
}
