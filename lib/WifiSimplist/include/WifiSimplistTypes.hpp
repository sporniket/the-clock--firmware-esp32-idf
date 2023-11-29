// Copyright 2023 David SPORN
// ---
// This file is part of 'Wifi Simplist'.
// ---
// 'Wifi Simplist' is free software: you can redistribute it and/or
// modify it under the terms of the GNU General Public License as published
// by the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.

// 'Wifi Simplist' is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General
// Public License for more details.

// You should have received a copy of the GNU General Public License along
// with 'Wifi Simplist'. If not, see <https://www.gnu.org/licenses/>. 
#ifndef WIFI_SIMPLIST_TYPES_HPP
#define WIFI_SIMPLIST_TYPES_HPP

// standard includes
#include <cstdint>

// esp32 includes

// project includes

// write code here

//**@brief Max length of SSID stored as C-String.
const uint8_t MAX_LENGTH_OF_SSID = 33;

//**@brief Max length of wifi password/psk as C-String.
const uint8_t MAX_LENGTH_OF_KEYPASS = 64;

/**
 * @brief Type of the wifi key/password
 */
enum WifiKeyType {
  //**@brief human typable password.
  PASSWORD,

  //**@brief Pre Shared Key (psk) format.
  PRESHAREDKEY
};

#include "WifiStationTypes.hpp"

#endif