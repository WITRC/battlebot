// =============================================================================
// Web Server Implementation
// Simple HTTP server using lwIP for status dashboard
// =============================================================================

#include "web_server.h"
#include "wifi_ap.h"
#include <stdio.h>
#include <string.h>
#include "pico/stdlib.h"
#include "lwip/tcp.h"

// State
static struct tcp_pcb* g_server_pcb = NULL;
static motor_controller_t* g_motors = NULL;
static bool g_running = false;

// Buffer for building HTTP responses
#define RESPONSE_BUFFER_SIZE 2048
static char g_response_buffer[RESPONSE_BUFFER_SIZE];

// Forward declarations
static err_t http_accept(void* arg, struct tcp_pcb* newpcb, err_t err);
static err_t http_recv(void* arg, struct tcp_pcb* tpcb, struct pbuf* p, err_t err);
static void http_close(struct tcp_pcb* tpcb);

// =============================================================================
// HTML Generation
// =============================================================================

static int generate_status_page(char* buffer, int max_len) {
    // init values
    int left;
    int right;
    weapon;
    bool armed;
    float voltage;
    int battery_percent;
    float temp;

    // get motor data
    if (g_motors != NULL) {
        motor_controller_get_status(g_motors, &left, &right, &weapon, &armed);
        left = g_motors->left_speed;
        right = g_motors->right_speed;
        weapon = g_motors->weapon_speed;

        armed = g_motors->weapon_armed;
    }

    // response template
    return snprintf(buffer, max_len,
        "HTTP/1.1 200 OK\r\n"
        "Content-Type: text/html\r\n"
        "Connection: close\r\n"
        "Refresh: 2\r\n"
        "\r\n"
        "<!DOCTYPE html><html><head>"
        "<meta name='viewport' content='width=device-width,initial-scale=1'>"
        "<style>"
        "body{font-family:monospace;background:#1a1a2e;color:#eee;padding:20px;text-align:center;}"
        ".box{background:#16213e;padding:15px;margin:10px auto;border-radius:8px;max-width:400px;border:1px solid #e94560;}"
        ".armed{color:#ff4444;font-weight:bold;font-size:1.5em;}"
        ".safe{color:#44ff44;font-weight:bold;font-size:1.5em;}"
        "</style></head><body>"
        "<h1>%s</h1>"
        "<div class='box'>"
        "<div>STATUS</div>"
        "<div class='%s'>%s</div>"
        "</div>"
        "<div class='box'>"
        "<div>MOTORS</div>"
        "<div>L: %+d%% | R: %+d%%</div>"
        "<div>WEAPON: %d%%</div>"
        "</div>"
        "<div class='box'>"
        "<div>SYSTEM</div>"
        "<div>Battery: %.2fV (%d%%)</div>"
        "<div>Temp: %.1f&deg;C</div>"
        "</div>"
        "</body></html>",
        ROBOT_NAME,                            // %s (H1)
        armed ? "armed" : "safe",              // %s (CSS class)
        armed ? "SYSTEM ARMED" : "SYSTEM SAFE",// %s (Status text)
        left, right, weapon,                   // %d, %d, %d
        voltage, battery_percent,              // %.2f, %d
        temp                                   // %.1f
    );
}

static int generate_404(char* buffer, int max_len) {
    return snprintf(buffer, max_len,
        "HTTP/1.1 404 Not Found\r\n"
        "Content-Type: text/html\r\n"
        "Connection: close\r\n"
        "\r\n"
        "<html><body><h1>404 Not Found</h1></body></html>"
    );
}

// =============================================================================
// TCP Callbacks
// =============================================================================

static err_t http_recv(void* arg, struct tcp_pcb* tpcb, struct pbuf* p, err_t err) {
    if (p == NULL) {
        http_close(tpcb);
        return ERR_OK;
    }

    printf("Web Server: Request received! Size: %d bytes\n", p->tot_len);

    // Tell lwIP we processed the data
    tcp_recved(tpcb, p->tot_len);

    // Generate the page
    int response_len = generate_status_page(g_response_buffer, RESPONSE_BUFFER_SIZE);

    // Write and Force Push
    err_t write_err = tcp_write(tpcb, g_response_buffer, response_len, TCP_WRITE_FLAG_COPY);
    if (write_err != ERR_OK) {
        printf("Web Server: tcp_write failed with error %d\n", write_err);
    }
    
    tcp_output(tpcb);

    pbuf_free(p);
    
    // Note: If the page is very simple, we close immediately. 
    // If it's complex, we'd wait for the sentinel callback.
    http_close(tpcb);

    return ERR_OK;
}

static void http_err(void* arg, err_t err) {
    // Error occurred, connection will be freed automatically
    (void)arg;
    (void)err;
}

static err_t http_accept(void* arg, struct tcp_pcb* newpcb, err_t err) {
    if (err != ERR_OK || newpcb == NULL) {
        return ERR_VAL;
    }

    // Set up callbacks for this connection
    tcp_recv(newpcb, http_recv);
    tcp_err(newpcb, http_err);

    return ERR_OK;
}

static void http_close(struct tcp_pcb* tpcb) {
    tcp_recv(tpcb, NULL);
    tcp_err(tpcb, NULL);
    tcp_close(tpcb);
}

// =============================================================================
// Public API
// =============================================================================

bool web_server_init(motor_controller_t* motors) {
    g_motors = motors;

    printf("Starting web server on port %d...\n", WEB_SERVER_PORT);

    // Create new TCP PCB
    g_server_pcb = tcp_new();
    if (!g_server_pcb) {
        printf("Failed to create TCP PCB\n");
        return false;
    }

    // Bind to port 80
    err_t err = tcp_bind(g_server_pcb, IP_ADDR_ANY, WEB_SERVER_PORT);
    if (err != ERR_OK) {
        printf("Failed to bind to port %d\n", WEB_SERVER_PORT);
        tcp_close(g_server_pcb);
        g_server_pcb = NULL;
        return false;
    }

    // Start listening
    g_server_pcb = tcp_listen(g_server_pcb);
    if (!g_server_pcb) {
        printf("Failed to listen\n");
        return false;
    }

    // Set accept callback
    tcp_accept(g_server_pcb, http_accept);

    g_running = true;
    printf("Web server ready at http://%s/\n", wifi_ap_get_ip());

    return true;
}

void web_server_poll(void) {
    // lwIP handles callbacks, nothing to poll explicitly
    // The threadsafe_background arch handles this
}

void web_server_stop(void) {
    if (g_server_pcb) {
        tcp_close(g_server_pcb);
        g_server_pcb = NULL;
    }
    g_running = false;
    printf("Web server stopped\n");
}

bool web_server_is_running(void) {
    return g_running;
}
