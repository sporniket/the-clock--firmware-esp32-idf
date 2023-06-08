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

// header include
#include "WifiCredentials.hpp"

WifiCredentials::~WifiCredentials() {}
// write code here...

bool WifiCredentials::isSameSsid(WifiCredentials *other) {
  return std::equal(std::begin(ssid), std::end(ssid), std::begin(other->ssid),
                    std::end(other->ssid));
}

WifiCredentials::WifiCredentials(uint8_t credentialSsid[MAX_LENGTH_OF_SSID],
                                 uint8_t credentialKey[MAX_LENGTH_OF_KEYPASS],
                                 WifiKeyType credentialKeyType,
                                 uint8_t credentialRank)
    : keyType(credentialKeyType), rank(credentialRank) {
  copyAndNullTerminate(ssid, credentialSsid, MAX_LENGTH_OF_SSID);
  copyAndNullTerminate(key, credentialKey, MAX_LENGTH_OF_KEYPASS);
}

WifiCredentials::WifiCredentials(WifiCredentials &wc)
    : keyType(wc.keyType), rank(wc.rank) {
  copyAndNullTerminate(ssid, wc.ssid, MAX_LENGTH_OF_SSID);
  copyAndNullTerminate(key, wc.key, MAX_LENGTH_OF_KEYPASS);
}
