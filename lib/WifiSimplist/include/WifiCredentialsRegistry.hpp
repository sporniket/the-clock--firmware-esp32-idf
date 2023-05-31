#ifndef WIFI_CREDENTIALS_REGISTRY_HPP
#define WIFI_CREDENTIALS_REGISTRY_HPP

// standard includes
#include <cstdint>
#include <vector>

// esp32 includes

// project includes
#include "WifiSimplistTypes.hpp"

/** @brief Maintains a list of wifi credentials, sorted by a preference rank.
 * The preference criterion is "latest known good (connected to) network". The
 * registry can only enumerate the entries following the ranking order.
 */
class WifiCredentialsRegistry {
private:
  static const uint8_t DEFAULT_MAX_SIZE = 3;
  /**
   * @brief the internal storage of the registry.
   */
  std::vector<WifiCredentials> registry;
  /**
   * @brief The registry will have a limited size, the default value will be
   * `DEFAULT_MAX_SIZE`.
   *
   */
  uint8_t maxSize;

public:
  WifiCredentialsRegistry(
      uint8_t size = WifiCredentialsRegistry::DEFAULT_MAX_SIZE)
      : maxSize(size) {}
  virtual ~WifiCredentialsRegistry();
  // enumeration interface.
  WifiCredentialsRegistry* rewind() ;
  bool hasNext() ;
  WifiCredentials* next() ;
  // update interface
  put(WifiCredentials credentials) ;
};

#endif