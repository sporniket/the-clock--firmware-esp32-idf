// Copyright 2023 David SPORN
// ---
// This file is part of 'Internet Simplist'.
// ---
// 'Internet Simplist' is free software: you can redistribute it and/or
// modify it under the terms of the GNU General Public License as published
// by the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.

// 'Internet Simplist' is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General
// Public License for more details.

// You should have received a copy of the GNU General Public License along
// with 'Internet Simplist'. If not, see <https://www.gnu.org/licenses/>. 
#ifndef INTERNET_SIMPLIST_TYPES_HPP
#define INTERNET_SIMPLIST_TYPES_HPP

// standard includes
#include <cstdint>

// esp32 includes

// project includes

// write code here

//**@brief An IPv4 address will be modeled as an array of 4 values
const uint8_t SIZE_OF_IPV4 = 4;
//**@brief An IPv6 address will be modeled as an array of 8 values
const uint8_t SIZE_OF_IPV6 = 8;

/**
 * @brief Storage of an Internet Protocol address, to support either version (v4 or v6).
 */
typedef union {
    uint8_t v4[SIZE_OF_IPV4] ;
    uint16_t v6[SIZE_OF_IPV6] ;
} IpAddress;

/**
 * @brief Required to decide which kind of address is stored in a `IpAddress`.
 */
enum IpAddressFormat {
  //**@brief To use the `v4` field of an `IpAddress`.
  IPV4,

  //**@brief To use the `v6` field of an `IpAddress`.
  IPV6
};

/**
 * @brief Basic description of an host configuration.
 * 
 */
typedef struct {
  IpAddressFormat ipAddressFormat;
  IpAddress ipAddress;
  IpAddress subNetworkMask;
  IpAddress defaultGateway ;
} HostConfigurationDescription ;

#endif