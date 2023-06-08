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
#ifndef WIFI_CREDENTIALS_REGISTRY_DAO_HPP
#define WIFI_CREDENTIALS_REGISTRY_DAO_HPP

// standard includes
#include <cstdint>
#include <string>

// esp32 includes

// project includes
#include "WifiCredentialsRegistry.hpp"

/** @brief An interface to save and load a registry of wifi credentials.
 */
class WifiCredentialsRegistryDao {
private:
  std::string designator;

public:
  virtual ~WifiCredentialsRegistryDao();
  /**
   * @brief Setup the designator. The usage of the designator depends on the
   * actual implementation. E.g. as a prefix, as a file name,...
   *
   * @param value the new value of the designator
   * @return WifiCredentialsRegistryDao* the dao, to be able to fluently chain
   * with a load or save, or to instanciate and setup.
   */
  WifiCredentialsRegistryDao *withDesignator(std::string value) {
    designator = value;
    return this;
  }

  /**
   * @brief Get the Designator.
   * 
   * @return std::string* the current designator.
   */
  std::string* getDesignator() {return &designator ;}

  /**
   * @brief Load entries and put them into the provided registry.
   *
   * @param recipient the registry to update.
   * 
   * @return true when all went well.
   */
  virtual bool loadInto(WifiCredentialsRegistry *recipient) = 0;

  /**
   * @brief Extract the entries of the provided registry and save them.
   *
   * @param source
   * 
   * @return true when all went well.
   */
  virtual bool saveFrom(WifiCredentialsRegistry *const source) = 0;
};

#endif