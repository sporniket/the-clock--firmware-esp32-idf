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

// header include
#include "WifiStationEsp32.hpp"

WifiStationEsp32::~WifiStationEsp32() {}
// write code here...

void WifiStationEsp32::notifyGotHostConfiguration(
    HostConfigurationDescription *desc) {
  for (std::unordered_set<HostConfigurationEventListener *>::iterator iter =
           hostConfigurationListeners.begin();
       iter != hostConfigurationListeners.end(); iter++) {
    HostConfigurationEventListener *listener = *iter;
    listener->onGotConfiguration(desc);
  }
}

void WifiStationEsp32::notifyLostHostConfiguration() {
  for (std::unordered_set<HostConfigurationEventListener *>::iterator iter =
           hostConfigurationListeners.begin();
       iter != hostConfigurationListeners.end(); iter++) {
    HostConfigurationEventListener *listener = *iter;
    listener->onLostConfiguration();
  }
}

void WifiStationEsp32::init() {}

void WifiStationEsp32::start() {}

void WifiStationEsp32::stop() {}

void WifiStationEsp32::tryWps() {}

bool WifiStationEsp32::isTryingWps() { return false; }
