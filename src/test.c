#include <stdio.h>
#include "pico/stdlib.h"
#include "pico/cyw43_arch.h"
#include "lwip/tcp.h"

#define SSID "Pico2W"
#define PASS "12345678"

static err_t http_recv(void *arg, struct tcp_pcb *tpcb, struct pbuf *p, err_t err) {
    if (!p) {
        tcp_close(tpcb);
        return ERR_OK;
    }

    tcp_recved(tpcb, p->tot_len);

    const char html[] =
        "HTTP/1.1 200 OK\r\n"
        "Content-Type: text/html\r\n\r\n"
        "<html><body><h1>Pico 2W AP</h1><p>It works.</p></body></html>";

    tcp_write(tpcb, html, sizeof(html) - 1, TCP_WRITE_FLAG_COPY);
    tcp_output(tpcb);
    pbuf_free(p);

    return ERR_OK;
}

static err_t http_accept(void *arg, struct tcp_pcb *newpcb, err_t err) {
    tcp_recv(newpcb, http_recv);
    return ERR_OK;
}

int main() {
    stdio_init_all();
    sleep_ms(2000);

    if (cyw43_arch_init()) {
        printf("WiFi init failed\n");
        return 1;
    }

    cyw43_arch_enable_ap_mode(SSID, PASS, CYW43_AUTH_WPA2_AES_PSK);

    printf("AP running\n");
    printf("Connect to %s\n", SSID);
    printf("Go to http://192.168.4.1\n");

    struct tcp_pcb *pcb = tcp_new();
    tcp_bind(pcb, IP_ADDR_ANY, 80);
    pcb = tcp_listen(pcb);
    tcp_accept(pcb, http_accept);

    while (1) {
        tight_loop_contents();
    }
}
