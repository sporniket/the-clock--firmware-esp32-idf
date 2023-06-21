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
#ifndef WIFI_STATION_ESP32_HPP
#define WIFI_STATION_ESP32_HPP

// standard includes
#include <cstdint>
#include <cstring>
#include <unordered_set>

// esp32 includes
#include "esp_log.h"
#include "esp_wifi.h"
#include "esp_wps.h"

// project includes
#include "InternetSimplist.hpp"
#include "WifiSimplist.hpp"

/** @brief Model of a Wifi Station on Esp32 platform.
 *
 * Typical startup sequence :
 * ```cpp
 * WifiStationEsp32* station = new WifiStationEsp32()
 *     -> withWifiCredentialsRegistryDao(theDao)
 *     -> withHostConfigurationEventListener(theListener) ;
 * station.init() ;
 * station.tryToConnect() ;
 * ```
 *
 * Typical status sequence, **once `tryToConnect()` has been called** :
 * ```
 * if(station.isConnected()) {
 *   // behavior for up and running, e.g. switch on a status led
 * } else if (station.isTryingToConnect()) {
 *   // behavior for waiting for connection, e.g. blinking a status led
 * } else {
 *   // behavior when down and idle, e.g. try to connect again when pushing a
 * button
 * }
 * ```
 */
class WifiStationEsp32 {
private:
  static constexpr char *TAG = (char *)"WifiStationEsp32";
  /**
   * @brief The registry of credentials, to be loaded on startup, and saved when
   * something change it.
   */
  WifiCredentialsRegistry wcreg;
  /**
   * @brief The DAO for a registry of wifi credentials to be used for loading
   * and saving the registry.
   */
  WifiCredentialsRegistryDao *wcregdao;

  /**
   * @brief The list of registered listeners of host configuration events.
   *
   */
  std::unordered_set<HostConfigurationEventListener *>
      hostConfigurationListeners;

  /** @brief Number of attempt when connecting to an access point.
   */
  const int MAX_RETRY = 10;

  /**
   * @brief Reset value for wpsCredentialsCurrent.
   */
  const int RESET_WPS_CREDENTIALS_CURRENT =
      -1; // because `tryNextWpsAccessPoint()` starts by incrementing
          // wpsCredentialsCurrent.

  /**
   * @brief Internal state in a modelized lifecycle.
   */
  WifiStationLifecycleState state = WifiStationLifecycleState::CREATED;
  /**
   * @brief Keep track of the remaining number of attempts to connecting to an
   * access point.
   */
  uint8_t remainingRetriesForAccessPoint;
  /**
   * @brief Keep track of the remaining number of attempts to use wps to scan
   * and get a connection to an access point.
   */
  uint8_t remainingRetriesForWps;

  /** @brief storage of the mac address.
   */
  uint8_t macAddress[6];

  /** @brief storage of the wps configuration.
   */
  esp_wps_config_t config WPS_CONFIG_INIT_DEFAULT(WPS_TYPE_PBC);

  /** @brief storage of all the credentials we got.
   */
  wifi_config_t wpsCredentials[MAX_WPS_AP_CRED];

  /** @brief number of credentials in wpsCredentials.
   */
  int wpsCredentialsCount = 0;

  /** @brief Current access point to connect to.
   */
  int wpsCredentialsCurrent = 0;

  /**
   * @brief Wifi config to be used with known access points
   */
  wifi_config_t wifi_config_known_ap = {
      .sta =
          {
              .ssid = "",
              .password = "",
              .threshold = {.authmode = WIFI_AUTH_WPA2_PSK},
              .sae_pwe_h2e = WPA3_SAE_PWE_UNSPECIFIED,
          },
  };

  /** @brief true when it got an host configuration.
   */
  bool isConnected{false};

  /**
   * @brief Current configuration (if `isConnected` is `true`).
   */
  HostConfigurationDescription hostConfiguration;

  static void setupCredentials(wifi_config_t &cfg, uint8_t *ssid,
                               uint8_t *pass) {
    memcpy(cfg.sta.ssid, ssid, MAX_SSID_LEN);
    ;
    memcpy(cfg.sta.password, pass, MAX_PASSPHRASE_LEN);
    ;
  }

  /**
   * @brief Try to use the next known access point credentials to setup the wifi
   * configuration.
   *
   * @return true when there is a connection being tried.
   */
  bool tryNextKnownAccessPoints();

  /**
   * @brief For a WPS access point, try multiple times as long as it is within
   * the accepted number of retries.
   */
  void tryAgainConnect() {
    esp_wifi_connect();
    if (0 < remainingRetriesForAccessPoint) {
      --remainingRetriesForAccessPoint;
    }
  }

  /**
   * @brief Try to use the next WPS access point that has been scanned.
   *
   * @return true when there is a connection being tried.
   */
  bool tryNextWpsAccessPoint();

  void startWps() {
    ESP_ERROR_CHECK(esp_wifi_wps_enable(&config));
    ESP_ERROR_CHECK(esp_wifi_wps_start(0));
    if (0 < remainingRetriesForWps) {
      --remainingRetriesForWps;
    }
  }

  void tryAgainWps() {
    ESP_ERROR_CHECK(esp_wifi_wps_disable());
    startWps();
  }

  /**
   * @brief Notify the registered listeners of host configuration events that it
   * got an IP address.
   *
   * @param desc the host configuration description.
   */
  void notifyGotHostConfiguration(HostConfigurationDescription *desc);

  /**
   * @brief Notify the registered listeners of host configuration events that it
   * lost the IP address.
   */
  void notifyLostHostConfiguration();

  // ========[ Wifi events handlers ]========
  /**
   * @brief Event handler for WIFI_EVENT_STA_START.
   *
   * @param arg see Esp32 documentation.
   * @param event_base see Esp32 documentation.
   * @param event_id see Esp32 documentation.
   * @param event_data the provided instance of WifiStationEsp32.
   */
  void handleWifiEventStationStart(void *arg, esp_event_base_t event_base,
                                   int32_t event_id, void *event_data);

  /**
   * @brief Event handler for WIFI_EVENT_STA_DISCONNECTED.
   *
   * @param arg see Esp32 documentation.
   * @param event_base see Esp32 documentation.
   * @param event_id see Esp32 documentation.
   * @param event_data the provided instance of WifiStationEsp32.
   */
  void handleWifiEventStationDisconnected(void *arg,
                                          esp_event_base_t event_base,
                                          int32_t event_id, void *event_data);

  /**
   * @brief Event handler for WIFI_EVENT_STA_WPS_ER_SUCCESS.
   *
   * @param arg see Esp32 documentation.
   * @param event_base see Esp32 documentation.
   * @param event_id see Esp32 documentation.
   * @param event_data the provided instance of WifiStationEsp32.
   */
  void handleWifiEventStationWpsEnrolleeSuccess(void *arg,
                                                esp_event_base_t event_base,
                                                int32_t event_id,
                                                void *event_data);

  /**
   * @brief Event handler for WIFI_EVENT_STA_WPS_ER_FAILED.
   *
   * @param arg see Esp32 documentation.
   * @param event_base see Esp32 documentation.
   * @param event_id see Esp32 documentation.
   * @param event_data the provided instance of WifiStationEsp32.
   */
  void handleWifiEventStationWpsEnrolleeFailure(void *arg,
                                                esp_event_base_t event_base,
                                                int32_t event_id,
                                                void *event_data);

  /**
   * @brief Event handler for WIFI_EVENT_STA_WPS_ER_TIMEOUT.
   *
   * @param arg see Esp32 documentation.
   * @param event_base see Esp32 documentation.
   * @param event_id see Esp32 documentation.
   * @param event_data the provided instance of WifiStationEsp32.
   */
  void handleWifiEventStationWpsEnrolleeTimeout(void *arg,
                                                esp_event_base_t event_base,
                                                int32_t event_id,
                                                void *event_data);

  // ========[ IP events handlers ]========
  /**
   * @brief Event handler for IP_EVENT_STA_GOT_IP.
   *
   * @param arg see Esp32 documentation.
   * @param event_base see Esp32 documentation.
   * @param event_id see Esp32 documentation.
   * @param event_data the provided instance of WifiStationEsp32.
   */
  void handleIpEventGotIp(void *arg, esp_event_base_t event_base,
                          int32_t event_id, void *event_data);

  /**
   * @brief Event handler for IP_EVENT_STA_LOST_IP.
   *
   * @param arg see Esp32 documentation.
   * @param event_base see Esp32 documentation.
   * @param event_id see Esp32 documentation.
   * @param event_data the provided instance of WifiStationEsp32.
   */
  void handleIpEventLostIp(void *arg, esp_event_base_t event_base,
                           int32_t event_id, void *event_data) {
    notifyLostHostConfiguration();
    changeStateToNotConnectedAndIdle();
  }

  // ========[ state management ]========
  /**
   * @brief Try to change the current state to the target state.
   *
   * @return true when the state has been changed.
   */
  bool changeStateToConnected();

  /**
   * @brief Try to change the current state to the target state.
   *
   * @return true when the state has been changed.
   */
  bool changeStateToNotConnectedAndIdle();

  /**
   * @brief Try to change the current state to the target state.
   *
   * @return true when the state has been changed.
   */
  bool changeStateToTryingWps();

  /**
   * @brief Try to change the current state to the target state.
   *
   * @return true when the state has been changed.
   */
  bool changeStateToDoneTryingKnownAccessPoints();

  /**
   * @brief Try to change the current state to the target state.
   *
   * @return true when the state has been changed.
   */
  bool changeStateToTryingKnownAccessPoints();

  /**
   * @brief Try to change the current state to the target state.
   *
   * @return true when the state has been changed.
   */
  bool changeStateToInstalled();

  /**
   * @brief Try to change the current state to the target state.
   *
   * @return true when the state has been changed.
   */
  bool changeStateToReadyToInstall();

  /**
   * @brief Try to change the current state to the target state.
   *
   * @return true when the state has been changed.
   */
  bool changeStateToReadyToInit();

public:
  virtual ~WifiStationEsp32();
  WifiStationEsp32 *
  withWifiCredentialsRegistryDao(WifiCredentialsRegistryDao *dao) {
    wcregdao = dao;
    changeStateToReadyToInit();
    return this;
  }
  WifiStationEsp32 *
  withHostConfigurationEventListener(HostConfigurationEventListener *listener) {
    if (0 == hostConfigurationListeners.count(listener)) {
      hostConfigurationListeners.insert(listener);
    }
    changeStateToReadyToInit();
    return this;
  }

  /**
   * @brief Install the wifi station event handlers.
   */
  void install();

  /**
   * @brief Initialise the station, should be called **after** having set all
   * the collaborative objects (e.g. call to
   * `withWifiCredentialsRegistryDao()`).
   */
  void init();

  /**
   * @brief Registers itself as handler for various Esp32-IdF wifi calls, then
   * try to connect.
   */
  void tryToConnect();

  /**
   * @brief Tells whether the station is in a connected state ('up and
   * running').
   *
   * @return true when a connection to an access point has been obtained.
   */
  bool isConnected() { return CONNECTED == state; }

  /**
   * @brief Tells whether the station is trying to get a connection.
   *
   * @return true when it tries to connect to an access point.
   */
  bool isTryingToConnect() {
    return TRYING_KNOWN_ACCESS_POINTS == state ||
           DONE_TRYING_KNOWN_ACCESS_POINTS == state || TRYING_WPS == state;
  }
};

#endif