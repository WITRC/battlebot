/**
 * @file web_dashboard_page.h
 * @brief Static dashboard page asset.
 *
 * Exposes the prebuilt HTTP response body for the root dashboard page.
 */
#ifndef WEB_DASHBOARD_PAGE_H
#define WEB_DASHBOARD_PAGE_H

#include <stddef.h>

/** @brief Return the raw dashboard HTTP response stored in flash. */
const char* web_dashboard_page_data(void);

/** @brief Return the exact byte size of the dashboard HTTP response. */
size_t web_dashboard_page_size(void);

#endif // WEB_DASHBOARD_PAGE_H
