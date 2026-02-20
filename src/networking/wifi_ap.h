/**
 * @file wifi_ap.h
 * @brief Wi-Fi access point driver for the web dashboard hotspot.
 */
#ifndef WIFI_AP_H
#define WIFI_AP_H

#include <stdint.h>
#include <stdbool.h>
#include "config.h"  // All settings centralized

/** @brief Initialize and start the Wi-Fi access point. @return true on success. */
bool wifi_ap_init(void);

/** @brief Return true if the AP is active. */
bool wifi_ap_is_active(void);

/** @brief Return the AP's IPv4 address as a dotted-decimal string. */
const char* wifi_ap_get_ip(void);

/** @brief Return the number of connected clients (platform accuracy may vary). */
int wifi_ap_get_client_count(void);

/** @brief Stop the Wi-Fi access point. */
void wifi_ap_stop(void);

#endif // WIFI_AP_H
