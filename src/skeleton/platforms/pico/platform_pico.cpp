//
// Created by cpasjuste on 30/05/23.
//

#include <pico/time.h>
#include <pico/stdlib.h>
#include <hardware/vreg.h>
#include <hardware/watchdog.h>
#include "platform.h"
#include "platform_pico.h"

using namespace mb;

PicoPlatform::PicoPlatform(bool useDoubleBufferDisplay) : Platform() {
    // overclock - the value for VCO set here is meant for least power consumption
#if MB_GB
    vreg_set_voltage(VREG_VOLTAGE_1_15);
    sleep_ms(2);
    set_sys_clock_khz(300000, true);
    sleep_ms(2);
#else
    const unsigned vco = 1596 * 1000 * 1000; // 266MHz
    const unsigned div1 = 6, div2 = 1;
    vreg_set_voltage(VREG_VOLTAGE_DEFAULT);
    sleep_ms(2);
    set_sys_clock_pll(vco, div1, div2);
    sleep_ms(2);
#endif

    // initialise USB serial connection for debugging
    stdio_init_all();
#ifndef NDEBUG
#ifndef LIB_PICO_STDIO_UART
    // wait for usb serial
    while (!stdio_usb_connected()) { sleep_ms(100); }
#endif
#endif
    printf("\r\nPicoPlatform: pico\r\n");

    p_display = useDoubleBufferDisplay ? (PicoDisplay *) new PicoDisplayBuffered() : new PicoDisplay();
    p_audio = new PicoAudio();
    p_input = new PicoInput();
    p_io = new PicoIo();
}

void PicoPlatform::reboot(const Platform::RebootTarget &target) {
    // set up watchdog scratch registers so that the bootloader
    // knows what to do after the reset
    switch (target) {
        case RebootTarget::Ui:
            watchdog_hw->scratch[0] = FLASH_MAGIC_UI;
            break;
        case RebootTarget::Gb:
            watchdog_hw->scratch[0] = FLASH_MAGIC_GB;
            break;
        case RebootTarget::Nes:
            watchdog_hw->scratch[0] = FLASH_MAGIC_NES;
            break;
        default:
            break;
    }

    watchdog_reboot(0x00000000, 0x00000000, 50);

    // wait for the reset
    while (true) tight_loop_contents();
}

PicoPlatform::~PicoPlatform() {
    printf("~PicoPlatform()\n");
}
