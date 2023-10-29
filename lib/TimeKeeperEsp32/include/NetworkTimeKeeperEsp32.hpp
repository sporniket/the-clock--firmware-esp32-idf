#ifndef NETWORK_TIME_KEEPER_ESP32_HPP
#define NETWORK_TIME_KEEPER_ESP32_HPP

// standard includes
#include <cstdint>
#include <time.h>
#include <sys/time.h>

// esp32 includes
#include "esp_log.h"
#include "esp_netif_sntp.h"
#include "esp_sntp.h"
#include "esp_wifi.h"
#include "esp_wps.h"
#include "lwip/ip_addr.h"

// project includes
#include "HostConfigurationEventListener.hpp"

/** @brief Synchronize time using SNTP.
 * 
 * For now, I want the option of using both DHCP and SNTP.
 */
class NetworkTimeKeeperEsp32 : public HostConfigurationEventListener {
private:
  esp_sntp_config_t config;

public:
  NetworkTimeKeeperEsp32(char* defaultSntpTimeServer);
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