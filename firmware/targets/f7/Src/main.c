#include <furi.h>
#include <furi_hal.h>
#include <flipper.h>
#include <alt_boot.h>
#include <flipper_boot_led.h>
#include <furi_boot_hal.h>

#define TAG "Main"

int main(void) {
    furi_boot_hal_init();
    // Flipper critical FURI HAL
    //furi_hal_init_critical();
    flipper_boot_led_sequence("W");

#ifdef FURI_RAM_EXEC
    if(false) {
#else
    if(furi_hal_bootloader_get_mode() == FuriHalBootloaderModeDFU) {
        furi_hal_bootloader_set_mode(FuriHalBootloaderModeNormal);
        //flipper_boot_led_sequence("B");
        flipper_boot_dfu_exec();
        furi_hal_power_reset();
    } else if(furi_hal_rtc_is_flag_set(FuriHalRtcFlagExecuteUpdate)) {
#endif
        furi_hal_init_critical();
        //flipper_boot_led_sequence("Y");
        flipper_boot_update_exec();
        // if things go nice, we shouldn't reach this point.
        // But if we do, abandon
        furi_hal_rtc_reset_flag(FuriHalRtcFlagExecuteUpdate);
        furi_hal_rtc_set_flag(FuriHalRtcFlagExecutePostUpdate);
        furi_hal_power_reset();
    } else {
        furi_hal_init_critical();
        furi_hal_clock_init();
        furi_hal_console_init();
        furi_hal_rtc_init();
        // Initialize FURI layer
        furi_init();

        // Flipper FURI HAL
        furi_hal_init();

        // CMSIS initialization
        osKernelInitialize();
        FURI_LOG_I(TAG, "KERNEL OK");

        // Init flipper
        flipper_init();

        // Start kernel
        osKernelStart();
    }
    while(1) {
    }
}

void Error_Handler(void) {
    furi_crash("ErrorHandler");
}

#ifdef USE_FULL_ASSERT
/**
    * @brief  Reports the name of the source file and the source line number
    *         where the assert_param error has occurred.
    * @param  file: pointer to the source file name
    * @param  line: assert_param error line source number
    * @retval None
    */
void assert_failed(uint8_t* file, uint32_t line) {
    furi_crash("HAL assert failed");
}
#endif /* USE_FULL_ASSERT */