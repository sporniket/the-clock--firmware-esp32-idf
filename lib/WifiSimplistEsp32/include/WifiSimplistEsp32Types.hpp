// Copyright 2023 David SPORN
// ---
// This file is part of 'Wifi Simplist for ESP32'.
// ---
// 'Wifi Simplist for ESP32' is free software: you can redistribute it and/or
// modify it under the terms of the GNU General Public License as published
// by the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.

// 'Wifi Simplist for ESP32' is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General
// Public License for more details.

// You should have received a copy of the GNU General Public License along
// with 'Wifi Simplist for ESP32'. If not, see <https://www.gnu.org/licenses/>. 
#ifndef WIFI_SIMPLIST_ESP32TYPES_HPP
#define WIFI_SIMPLIST_ESP32TYPES_HPP

// standard includes
#include <cstdint>

// esp32 includes
#include "esp_wifi.h"
#include "esp_wps.h"

// project includes

/**
 * @brief Interface of a class that handle all the events related with a Wifi
 * station.
 *
 */
class WifiStationEsp32EventHandler {
public:
  // ========[ Wifi events handlers ]========
  /**
   * @brief Event handler for WIFI_EVENT_STA_START.
   *
   * @param arg see Esp32 documentation.
   * @param event_base see Esp32 documentation.
   * @param event_id see Esp32 documentation.
   * @param event_data the provided instance of WifiStationEsp32.
   */
  virtual void handleWifiEventStationStart(void *event_data) = 0;

  /**
   * @brief Event handler for WIFI_EVENT_STA_CONNECTED.
   *
   * @param arg see Esp32 documentation.
   * @param event_base see Esp32 documentation.
   * @param event_id see Esp32 documentation.
   * @param event_data the provided instance of WifiStationEsp32.
   */
  virtual void handleWifiEventStationConnected(void *event_data) = 0;

  /**
   * @brief Event handler for WIFI_EVENT_STA_DISCONNECTED.
   *
   * @param arg see Esp32 documentation.
   * @param event_base see Esp32 documentation.
   * @param event_id see Esp32 documentation.
   * @param event_data the provided instance of WifiStationEsp32.
   */
  virtual void handleWifiEventStationDisconnected(void *event_data) = 0;

  /**
   * @brief Event handler for WIFI_EVENT_STA_WPS_ER_SUCCESS.
   *
   * @param arg see Esp32 documentation.
   * @param event_base see Esp32 documentation.
   * @param event_id see Esp32 documentation.
   * @param event_data the provided instance of WifiStationEsp32.
   */
  virtual void handleWifiEventStationWpsEnrolleeSuccess(void *event_data) = 0;

  /**
   * @brief Event handler for WIFI_EVENT_STA_WPS_ER_FAILED.
   *
   * @param arg see Esp32 documentation.
   * @param event_base see Esp32 documentation.
   * @param event_id see Esp32 documentation.
   * @param event_data the provided instance of WifiStationEsp32.
   */
  virtual void handleWifiEventStationWpsEnrolleeFailure(void *event_data) = 0;

  /**
   * @brief Event handler for WIFI_EVENT_STA_WPS_ER_TIMEOUT.
   *
   * @param arg see Esp32 documentation.
   * @param event_base see Esp32 documentation.
   * @param event_id see Esp32 documentation.
   * @param event_data the provided instance of WifiStationEsp32.
   */
  virtual void handleWifiEventStationWpsEnrolleeTimeout(void *event_data) = 0;

  // ========[ IP events handlers ]========
  /**
   * @brief Event handler for IP_EVENT_STA_GOT_IP.
   *
   * @param arg see Esp32 documentation.
   * @param event_base see Esp32 documentation.
   * @param event_id see Esp32 documentation.
   * @param event_data the provided instance of WifiStationEsp32.
   */
  virtual void handleIpEventGotIp(void *event_data) = 0;

  /**
   * @brief Event handler for IP_EVENT_STA_LOST_IP.
   *
   * @param arg see Esp32 documentation.
   * @param event_base see Esp32 documentation.
   * @param event_id see Esp32 documentation.
   * @param event_data the provided instance of WifiStationEsp32.
   */
  virtual void handleIpEventLostIp(void *event_data) = 0;
};

#endif