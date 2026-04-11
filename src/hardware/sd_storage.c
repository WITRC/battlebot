/**
 * @file sd_storage.c
 * @brief SD card driver implementation using FatFS over SPI0.
 */

#include "sd_storage.h"

#include <stdio.h>
#include <string.h>

#include "f_util.h"
#include "ff.h"
#include "hw_config.h"

static FATFS fatfs;
static bool mounted = false;

bool sd_card_init(void) {
    if (mounted) {
        return true;
    }

    sd_card_t *sd = sd_get_by_num(0);
    if (!sd) {
        printf("[SD] No SD card configured\n");
        return false;
    }

    FRESULT fr = f_mount(&fatfs, "", 1);
    if (fr != FR_OK) {
        printf("[SD] Mount failed: %s (%d)\n", FRESULT_str(fr), fr);
        return false;
    }

    mounted = true;
    printf("[SD] Card mounted\n");
    return true;
}

void sd_card_deinit(void) {
    if (!mounted) {
        return;
    }

    f_unmount("");
    mounted = false;
    printf("[SD] Card unmounted\n");
}

bool sd_card_is_mounted(void) {
    return mounted;
}

bool sd_card_append(const char *filename, const char *text) {
    if (!mounted) {
        printf("[SD] Not mounted\n");
        return false;
    }

    FIL fil;
    FRESULT fr = f_open(&fil, filename, FA_OPEN_APPEND | FA_WRITE);
    if (fr != FR_OK) {
        printf("[SD] Open failed: %s (%d)\n", FRESULT_str(fr), fr);
        return false;
    }

    int written = f_printf(&fil, "%s", text);
    f_close(&fil);

    if (written < 0) {
        printf("[SD] Write failed\n");
        return false;
    }

    return true;
}

bool sd_card_write(const char *filename, const char *text) {
    if (!mounted) {
        printf("[SD] Not mounted\n");
        return false;
    }

    FIL fil;
    FRESULT fr = f_open(&fil, filename, FA_CREATE_ALWAYS | FA_WRITE);
    if (fr != FR_OK) {
        printf("[SD] Open failed: %s (%d)\n", FRESULT_str(fr), fr);
        return false;
    }

    int written = f_printf(&fil, "%s", text);
    f_close(&fil);

    if (written < 0) {
        printf("[SD] Write failed\n");
        return false;
    }

    return true;
}

int sd_card_read(const char *filename, char *buf, size_t buf_size) {
    if (!mounted) {
        printf("[SD] Not mounted\n");
        return -1;
    }

    FIL fil;
    FRESULT fr = f_open(&fil, filename, FA_READ);
    if (fr != FR_OK) {
        printf("[SD] Open failed: %s (%d)\n", FRESULT_str(fr), fr);
        return -1;
    }

    UINT bytes_read = 0;
    fr = f_read(&fil, buf, buf_size - 1, &bytes_read);
    f_close(&fil);

    if (fr != FR_OK) {
        printf("[SD] Read failed: %s (%d)\n", FRESULT_str(fr), fr);
        return -1;
    }

    buf[bytes_read] = '\0';
    return (int)bytes_read;
}
