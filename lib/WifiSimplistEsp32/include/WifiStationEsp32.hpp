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
#include <unordered_set>

// esp32 includes

// project includes
#include "InternetSimplist.hpp"
#include "WifiSimplist.hpp"

/** @brief Model of a Wifi Station on Esp32 platform.
 */
class WifiStationEsp32 {
private:
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

public:
  virtual ~WifiStationEsp32();
  WifiStationEsp32 *
  withWifiCredentialsRegistryDao(WifiCredentialsRegistryDao *dao) {
    wcregdao = dao;
    return this;
  }
  WifiStationEsp32 *
  withHostConfigurationEventListener(HostConfigurationEventListener *listener) {
    if (0 == hostConfigurationListeners.count(listener)) {
      hostConfigurationListeners.insert(listener);
    }
    return this;
  }

  /**
   * @brief Initialise the station, should be called **after** having set all
   * the collaborative objects (e.g. call to
   * `withWifiCredentialsRegistryDao()`).
   */
  void init();

  /**
   * @brief Registers itself as handler for various Esp32-IdF wifi calls.
   */
  void start();

  /**
   * @brief Deregisters itself as handler for various Esp32-IdF wifi calls.
   *
   */
  void stop();

  /**
   * @brief Start to scan and find an access point with an activated push-button
   * WPS, do nothing if it is already doing so.
   */
  void tryWps();

  /**
   * @brief Query about currently attempting to find and connect to an activated
   * push-button WPS access point.
   *
   * @return true When trying to find a WPS access point
   */
  bool isTryingWps();
};

#endif