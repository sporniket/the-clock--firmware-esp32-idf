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
#ifndef WIFI_CREDENTIALS_HPP
#define WIFI_CREDENTIALS_HPP

// standard includes
#include <algorithm>
#include <cstdint>
#include <cstring>
#include <iterator>
#include <ranges>

// esp32 includes

// project includes
#include "WifiSimplistTypes.hpp"

/**
 * @brief Description of a wifi credentials item to be put in a registry of
 * credentials. A credentials is ranked, 0 being the best.
 */
class WifiCredentials {
private:
  /**
   * @brief A preference ranking, to be used as a sorting key.
   */
  uint8_t rank;
  /**
   * @brief The wifi network ssid.
   */
  uint8_t ssid[MAX_LENGTH_OF_SSID];
  /**
   * @brief What kind of key is stored, some implementation distinguish between
   * password and pre-shared key.
   */
  WifiKeyType keyType;
  /**
   * @brief The key to authenticate on the wifi network.
   */
  uint8_t key[MAX_LENGTH_OF_KEYPASS];
  static void copyAndNullTerminate(uint8_t *to, const uint8_t *from,
                                   uint8_t size) {
    memcpy(to, from, size);
    to[size - 1] = 0;
  }

public:
  /**
   * @brief Comparator by rank.
   *
   * @param wc1 the left hand side
   * @param wc2 the right hand side.
   * @return true if the left hand side is before the right hand side.
   */
  static bool compareByRank(WifiCredentials *wc1, WifiCredentials *wc2) {
    return wc1->rank < wc2->rank;
  }

  virtual ~WifiCredentials();
  WifiCredentials(uint8_t *credentialSsid, uint8_t *credentialKey,
                  WifiKeyType credentialKeyType, uint8_t credentialRank = 0);
  WifiCredentials(WifiCredentials &wc);

  /**
   * @brief Compare the SSID.
   *
   * @param other the other credentials to compare.
   * @return true if both SSIDs are the same.
   */
  bool isSameSsid(WifiCredentials *other);

  /**
   * @brief Make this credential one-step better ranked.
   */
  void rankUp() { --rank; };

  /**
   * @brief Make this credential one-step worse ranked.
   */
  void rankDown() { ++rank; };
  /**
   * @brief Make this credential the best ranked one.
   */
  void rankFirst() { rank = 0; };

  /**
   * @brief Get the Ssid.
   *
   * @return uint8_t* const the ssid.
   */
  uint8_t *const getSsid() { return ssid; }
  /**
   * @brief Get the Key.
   *
   * @return uint8_t* const the key.
   */
  uint8_t *const getKey() { return key; }
  /**
   * @brief Get the Key type.
   *
   * @return const WifiKeyType the type.
   */
  const WifiKeyType getKeyType() { return keyType; }

  /**
   * @brief Change the type of key.
   *
   * @param newKeyType the new type of key.
   */
  void setKeyType(WifiKeyType newKeyType) { keyType = newKeyType; }

  /**
   * @brief Set the new key.
   *
   * @param newKey the new key value to copy from.
   */
  void setKey(const uint8_t *newKey) {
    copyAndNullTerminate(key, newKey, MAX_LENGTH_OF_KEYPASS);
  }
};

#endif