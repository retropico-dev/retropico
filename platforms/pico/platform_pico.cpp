//
// Created by cpasjuste on 30/05/23.
//

#include <pico/time.h>
#include <pico/stdlib.h>
#include <hardware/vreg.h>
#include <class/cdc/cdc_device.h>
#include "platform_pico.h"
#include "display_ssd1306.h"
#include "audio_pico.h"
#include "io_pico.h"

using namespace mb;

PicoPlatform::PicoPlatform() : Platform() {
    /* Overclock. */
    {
        /* The value for VCO set here is meant for least power
         * consumption. */
        const unsigned vco = 792000000; /* 264MHz/132MHz */
        const unsigned div1 = 3, div2 = 1;
        //vreg_set_voltage(VREG_VOLTAGE_1_15);
        vreg_set_voltage(VREG_VOLTAGE_DEFAULT);
        sleep_ms(2);
        set_sys_clock_pll(vco, div1, div2);
        sleep_ms(2);
    }

    /* Initialise USB serial connection for debugging. */
    stdio_init_all();
#ifndef NDEBUG
    while (!tud_cdc_connected()) { sleep_ms(100); }
#endif

    /* Initialise GPIO pins. */
#if 0
    gpio_set_function(GPIO_CS, GPIO_FUNC_SIO);
    gpio_set_function(GPIO_CLK, GPIO_FUNC_SPI);
    gpio_set_function(GPIO_SDA, GPIO_FUNC_SPI);
    gpio_set_function(GPIO_RS, GPIO_FUNC_SIO);
    gpio_set_function(GPIO_RST, GPIO_FUNC_SIO);

    gpio_set_dir(GPIO_CS, true);
    gpio_set_dir(GPIO_RS, true);
    gpio_set_dir(GPIO_RST, true);
    gpio_set_slew_rate(GPIO_CLK, GPIO_SLEW_RATE_FAST);
    gpio_set_slew_rate(GPIO_SDA, GPIO_SLEW_RATE_FAST);

    /* Set SPI clock to use high frequency. */
    clock_configure(clk_peri, 0,
                    CLOCKS_CLK_PERI_CTRL_AUXSRC_VALUE_CLK_SYS,
                    125 * 1000 * 1000, 125 * 1000 * 1000);
    spi_init(spi0, 16 * 1000 * 1000);
    spi_set_format(spi0, 16, SPI_CPOL_0, SPI_CPHA_0, SPI_MSB_FIRST);
#endif

    p_display = new SSD1306Display();
    p_audio = new PicoAudio();
    // TODO
    p_input = new Input();
    // TODO
    p_io = new PicoIo();
}

PicoPlatform::~PicoPlatform() {
    printf("~PicoPlatform()\n");
    reset_usb_boot(0, 0);
}
