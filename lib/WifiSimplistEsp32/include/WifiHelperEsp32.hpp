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
#ifndef WIFI_HELPER_ESP32_HPP
#define WIFI_HELPER_ESP32_HPP

// standard includes
#include <cstdint>

// esp32 includes

// project includes
#include "WifiCredentialsRegistryDaoUsingNvs.hpp"
#include "WifiStationEsp32.hpp"

/** @brief What the class is for.
 */
class WifiHelperEsp32 {
private:
public:
  virtual ~WifiHelperEsp32();

  /**
   * @brief Helper to create and run a Wifi Station
   *
   * @param storageName Name of nvs storage to save wifi credentials
   * @param listener1 mandatory HostConfigurationEventListener
   * @param listener2 optionnal HostConfigurationEventListener
   * @param listener3 optionnal HostConfigurationEventListener
   * @param listener4 optionnal HostConfigurationEventListener
   * @return WifiStationEsp32* the station, that has been started.
   */
  static WifiStationEsp32 *
  setupAndRunStation(char *storageName,
                     HostConfigurationEventListener *listener1,
                     HostConfigurationEventListener *listener2 = nullptr,
                     HostConfigurationEventListener *listener3 = nullptr,
                     HostConfigurationEventListener *listener4 = nullptr);
};

#endif