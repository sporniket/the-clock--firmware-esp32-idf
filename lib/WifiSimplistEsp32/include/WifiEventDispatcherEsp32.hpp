#ifndef WIFI_EVENT_DISPATCHER_ESP32_HPP
#define WIFI_EVENT_DISPATCHER_ESP32_HPP

// standard includes
#include <cstdint>
#include <vector>

// esp32 includes
#include "esp_log.h"
#include "esp_wifi.h"
#include "esp_wps.h"

// project includes
#include "WifiSimplistEsp32Types.hpp"
#include "WifiStationEsp32.hpp"

/** @brief Singleton that have to be registered as Wifi event handlers, and then
 * dispatches those event to listeners.
 */
class WifiEventDispatcherEsp32 {
private:

public:
  virtual ~WifiEventDispatcherEsp32();
  static void installHandlers();
  static void addListener(WifiStationEsp32 *listener);
};

#endif