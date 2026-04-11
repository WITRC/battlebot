/**
 * @file sd_storage.h
 * @brief SD card driver — mount/unmount, read/write files via FatFS.
 */
#ifndef SD_STORAGE_H
#define SD_STORAGE_H

#include <stdbool.h>
#include <stddef.h>

/** @brief Initialize SPI and mount the SD card. Returns true on success. */
bool sd_card_init(void);

/** @brief Unmount the SD card. */
void sd_card_deinit(void);

/** @brief Check whether the SD card is currently mounted. */
bool sd_card_is_mounted(void);

/**
 * @brief Append a line of text to a file.
 * Creates the file if it doesn't exist.
 * @return true on success.
 */
bool sd_card_append(const char *filename, const char *text);

/**
 * @brief Write data to a file, overwriting any existing content.
 * Creates the file if it doesn't exist.
 * @return true on success.
 */
bool sd_card_write(const char *filename, const char *text);

/**
 * @brief Read up to @p buf_size - 1 bytes from a file into @p buf.
 * Null-terminates the result.
 * @return Number of bytes read, or -1 on error.
 */
int sd_card_read(const char *filename, char *buf, size_t buf_size);

#endif // SD_STORAGE_H
