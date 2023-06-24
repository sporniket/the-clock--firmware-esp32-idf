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
  static constexpr char *TAG = (char *)"WifiEventDispatcherEsp32";

  // ========[ Wifi events handlers ]========
  /**
   * @brief Event handler for WIFI_EVENT_STA_START.
   *
   * @param arg see Esp32 documentation.
   * @param event_base see Esp32 documentation.
   * @param event_id see Esp32 documentation.
   * @param event_data the provided instance of WifiStationEsp32.
   */
  static void handleWifiEventStationStart(void *arg,
                                          esp_event_base_t event_base,
                                          int32_t event_id, void *event_data);

  /**
   * @brief Event handler for WIFI_EVENT_STA_DISCONNECTED.
   *
   * @param arg see Esp32 documentation.
   * @param event_base see Esp32 documentation.
   * @param event_id see Esp32 documentation.
   * @param event_data the provided instance of WifiStationEsp32.
   */
  static void handleWifiEventStationDisconnected(void *arg,
                                                 esp_event_base_t event_base,
                                                 int32_t event_id,
                                                 void *event_data);

  /**
   * @brief Event handler for WIFI_EVENT_STA_WPS_ER_SUCCESS.
   *
   * @param arg see Esp32 documentation.
   * @param event_base see Esp32 documentation.
   * @param event_id see Esp32 documentation.
   * @param event_data the provided instance of WifiStationEsp32.
   */
  static void
  handleWifiEventStationWpsEnrolleeSuccess(void *arg,
                                           esp_event_base_t event_base,
                                           int32_t event_id, void *event_data);

  /**
   * @brief Event handler for WIFI_EVENT_STA_WPS_ER_FAILED.
   *
   * @param arg see Esp32 documentation.
   * @param event_base see Esp32 documentation.
   * @param event_id see Esp32 documentation.
   * @param event_data the provided instance of WifiStationEsp32.
   */
  static void
  handleWifiEventStationWpsEnrolleeFailure(void *arg,
                                           esp_event_base_t event_base,
                                           int32_t event_id, void *event_data);

  /**
   * @brief Event handler for WIFI_EVENT_STA_WPS_ER_TIMEOUT.
   *
   * @param arg see Esp32 documentation.
   * @param event_base see Esp32 documentation.
   * @param event_id see Esp32 documentation.
   * @param event_data the provided instance of WifiStationEsp32.
   */
  static void
  handleWifiEventStationWpsEnrolleeTimeout(void *arg,
                                           esp_event_base_t event_base,
                                           int32_t event_id, void *event_data);

  // ========[ IP events handlers ]========
  /**
   * @brief Event handler for IP_EVENT_STA_GOT_IP.
   *
   * @param arg see Esp32 documentation.
   * @param event_base see Esp32 documentation.
   * @param event_id see Esp32 documentation.
   * @param event_data the provided instance of WifiStationEsp32.
   */
  static void handleIpEventGotIp(void *arg, esp_event_base_t event_base,
                                 int32_t event_id, void *event_data);

  /**
   * @brief Event handler for IP_EVENT_STA_LOST_IP.
   *
   * @param arg see Esp32 documentation.
   * @param event_base see Esp32 documentation.
   * @param event_id see Esp32 documentation.
   * @param event_data the provided instance of WifiStationEsp32.
   */
  static void handleIpEventLostIp(void *arg, esp_event_base_t event_base,
                                  int32_t event_id, void *event_data);

public:
  virtual ~WifiEventDispatcherEsp32();
  static void install();
  static void addListener(WifiStationEsp32 *listener);
};

#endif