#ifndef NETWORK_TIME_KEEPER_ESP32_HPP
#define NETWORK_TIME_KEEPER_ESP32_HPP

// standard includes
#include <cstdint>

// esp32 includes
#include <esp_sntp.h>

// project includes
#include "HostConfigurationEventListener.hpp"

/** @brief What the class is for.
 */
class NetworkTimeKeeperEsp32 : public HostConfigurationEventListener {
private:
public:
  virtual ~NetworkTimeKeeperEsp32();

  /**
   * @brief Event received when obtaining a host configuration.
   *
   * @param configuration the configuration (ip address, ...).
   */
  virtual void
  onGotConfiguration(HostConfigurationDescription *configuration);

  /**
   * @brief Previously received configuration is now invalid (destroyed).
   *
   */
  virtual void onLostConfiguration();
};

#endif