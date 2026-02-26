/**
 * @file web_server.h
 * @brief Simple HTTP status-dashboard server.
 *
 * Wi-Fi AP must be initialized before calling @ref web_server_init.
 */
#ifndef WEB_SERVER_H
#define WEB_SERVER_H

#include <stdint.h>
#include <stdbool.h>
#include "config.h"  // All settings centralized
#include "motor_controller.h"

/**
 * @brief Initialize and start the web server.
 * @param motors  Motor controller used for the status display page.
 * @return true on success.
 */
bool web_server_init(motor_controller_t* motors);

/** @brief Poll for incoming HTTP requests (non-blocking; call each main-loop iteration). */
void web_server_poll(void);

/** @brief Stop the web server and release resources. */
void web_server_stop(void);

/** @brief Return true if the web server is currently running. */
bool web_server_is_running(void);

#endif // WEB_SERVER_H
