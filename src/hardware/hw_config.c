/**
 * @file hw_config.c
 * @brief FatFS SD card hardware configuration for SPI0.
 *
 * Required by the no-OS-FatFS-SD-SDIO-SPI-RPi-Pico library.
 * Defines which SPI peripheral and GPIOs are used for the SD card.
 */

#include "hw_config.h"

/* SPI bus configuration */
static spi_t spi = {
    .hw_inst = spi0,
    .sck_gpio  = 18,  // GP18 — SPI0 SCK
    .mosi_gpio = 19,  // GP19 — SPI0 TX
    .miso_gpio = 16,  // GP16 — SPI0 RX
    .baud_rate = 12500 * 1000,  // 12.5 MHz
};

/* SPI interface for the SD card */
static sd_spi_if_t spi_if = {
    .spi = &spi,
    .ss_gpio = 17,  // GP17 — SPI0 CS
};

/* SD card slot configuration */
static sd_card_t sd_card = {
    .type = SD_IF_SPI,
    .spi_if_p = &spi_if,
};

/* Library callbacks — do not rename */
size_t sd_get_num(void) { return 1; }

sd_card_t *sd_get_by_num(size_t num) {
    if (num == 0) {
        return &sd_card;
    }
    return NULL;
}
