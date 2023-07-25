// Copyright 2023 David SPORN
// ---
// This file is part of 'the clock by sporniket -- ESP32/IDF firmware'.
// ---
// 'ESP32/IDF workspace for IIC' is free software: you can redistribute it
// and/or modify it under the terms of the GNU General Public License as
// published by the Free Software Foundation, either version 3 of the License,
// or (at your option) any later version.

// 'ESP32/IDF workspace for IIC' is distributed in the hope that it will be
// useful, but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General
// Public License for more details.

// You should have received a copy of the GNU General Public License along
// with 'ESP32/IDF workspace for IIC'. If not, see
// <https://www.gnu.org/licenses/>. 

// esp32 includes
#include "esp_log.h"
#include "nvs.h"
#include "nvs_flash.h"

// sdk config
#include "sdkconfig.h"

// project includes
#include "NetworkTimeKeeperEsp32.hpp"

extern "C" {
void app_main(void);
}

NetworkTimeKeeperEsp32 *networkTimeKeeper;


void app_main(void) {
  // setup
  networkTimeKeeper = new NetworkTimeKeeperEsp32() ;

  // and voila
}