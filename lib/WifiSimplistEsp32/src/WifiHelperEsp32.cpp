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
#include "WifiHelperEsp32.hpp"

WifiHelperEsp32::~WifiHelperEsp32() {}
// write code here...

WifiStationEsp32 *
WifiHelperEsp32::setupAndRunStation(char *storageName,
                                    HostConfigurationEventListener *listener1,
                                    HostConfigurationEventListener *listener2,
                                    HostConfigurationEventListener *listener3,
                                    HostConfigurationEventListener *listener4) {
  // do stuff
  WifiStationEsp32 *station =
      (new WifiStationEsp32()) //
          ->withWifiCredentialsRegistryDao(
              (new WifiCredentialsRegistryDaoUsingNvs())
                  ->withDesignator(storageName))          //
          ->withHostConfigurationEventListener(listener1) //
          ->withHostConfigurationEventListener(listener2) //
          ->withHostConfigurationEventListener(listener3) //
          ->withHostConfigurationEventListener(listener4) //
      ;
  station->init();
  station->install();
  station->tryToConnect();
  return station;
}