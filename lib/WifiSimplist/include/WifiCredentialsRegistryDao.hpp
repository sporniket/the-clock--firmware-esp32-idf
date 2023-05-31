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
   * @brief Load entries and put them into the provided registry.
   *
   * @param recipient the registry to update.
   */
  virtual loadInto(const WifiCredentialsRegistry *recipient) = 0;

  /**
   * @brief Extract the entries of the provided registry and save them.
   *
   * @param source
   */
  virtual saveFrom(const WifiCredentialsRegistry *const source) = 0;
};

#endif