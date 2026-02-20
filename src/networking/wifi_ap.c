/*
 * Wi-Fi Access Point Implementation
 * Based on pico-examples access_point
*/

#include "wifi_ap.h"
#include <stdio.h>
#include <string.h>
#include "pico/stdlib.h"
#include "pico/cyw43_arch.h"
#include "lwip/netif.h"

static bool g_ap_active = false;

bool wifi_ap_init(void) {
    printf("Starting Wi-Fi Access Point...\n");
    printf("  SSID: %s\n", WIFI_AP_SSID);
    printf("  Password: %s\n", WIFI_AP_PASSWORD);

    // Enable AP mode - this configures the netif automatically
    cyw43_arch_enable_ap_mode(WIFI_AP_SSID, WIFI_AP_PASSWORD, CYW43_AUTH_WPA2_AES_PSK);

    // Small delay to let AP mode fully initialize
    sleep_ms(100);

    // Use CYW43's default AP address (192.168.4.1)
    // Must use ip_addr_t (not ip4_addr_t) for proper lwIP compatibility
    ip_addr_t gw, mask;
    IP4_ADDR(ip_2_ip4(&gw), 192, 168, 4, 1);
    IP4_ADDR(ip_2_ip4(&mask), 255, 255, 255, 0);
    IP_SET_TYPE_VAL(gw, IPADDR_TYPE_V4);
    IP_SET_TYPE_VAL(mask, IPADDR_TYPE_V4);

    printf("  Gateway: %s\n", ipaddr_ntoa(&gw));

    // Debug: show network interface state
    struct netif *nif = netif_list;
    printf("  Network interfaces:\n");
    while (nif != NULL) {
        printf("    - %c%c%d: %s (flags=0x%02x)\n",
               nif->name[0], nif->name[1], nif->num,
               ip4addr_ntoa(netif_ip4_addr(nif)),
               nif->flags);
        nif = nif->next;
    }

    g_ap_active = true;

    printf("Wi-Fi AP ready!\n");
    printf("  Connect to: %s\n", WIFI_AP_SSID);
    printf("  Browse to: http://%s/\n", WIFI_AP_IP);

    return true;
}

bool wifi_ap_is_active(void) {
    return g_ap_active;
}

const char* wifi_ap_get_ip(void) {
    return WIFI_AP_IP;
}

int wifi_ap_get_client_count(void) {
    return 0;
}

void wifi_ap_stop(void) {
    if (g_ap_active) {
        cyw43_arch_disable_ap_mode();
        g_ap_active = false;
        printf("Wi-Fi AP stopped\n");
    }
}
