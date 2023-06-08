#ifndef WIFI_CREDENTIALS_REGISTRY_DAO_USING_NVS_HPP
#define WIFI_CREDENTIALS_REGISTRY_DAO_USING_NVS_HPP

// standard includes
#include <cstdint>
#include <cstdio>
#include <memory>

// esp32 includes
#include <esp_log.h>
#include <nvs.h>
#include <nvs_flash.h>
#include <nvs_handle.hpp>

// project includes
#include "WifiSimplist.hpp"

/** @brief Dao for wifi credentials registry, that use the non volatile storage.
 *
 * The designator is used as namespace. The NVS **MUST** have been initialized
 * beforehand.
 */
class WifiCredentialsRegistryDaoUsingNvs : public WifiCredentialsRegistryDao {
private:
  void logErrorGetItem(const char *tag, esp_err_t err, char* keyname);
  void logErrorSetItem(const char *tag, esp_err_t err, char* keyname);
  void logErrorCommit(const char *tag, esp_err_t err, char* keyname);

public:
  virtual ~WifiCredentialsRegistryDaoUsingNvs();

  /**
   * @brief Load entries and put them into the provided registry.
   *
   * @param recipient the registry to update.
   *
   * @return true when all went well.
   */
  virtual bool loadInto(WifiCredentialsRegistry *recipient);

  /**
   * @brief Extract the entries of the provided registry and save them.
   *
   * @param source
   *
   * @return true when all went well.
   */
  virtual bool saveFrom(WifiCredentialsRegistry *const source);
};

#endif