#ifndef NETWORK_STATION_HPP
#define NETWORK_STATION_HPP

// standard includes
#include <cstdint>
#include <vector>

// esp32 includes
#include "esp_wifi.h"

// project includes

// ################ SCRATCHPAD [start] ##################
const uint8_t SIZE_OF_WIFI_SSID = 32;
typedef struct {
  //@brief : flag to know whether password is a manual password or precomputed
  // psk (maybe )
  bool isWps;
  //@brief : ssid of the wifi access point
  uint8_t ssid[SIZE_OF_WIFI_SSID];
  //@brief : human readable password
  uint8_t password[64];
} WifiCredentials;

/**
 * @brief Registry of known wifi credentials, to be queried in sequence.
 *
 */
class RegistryOfWifiCredentials {
private:
  std::vector<WifiCredentials *> registry;
  std::vector<WifiCredentials *>::iterator iteratorOnCredentials;
  bool isSameSsid(uint8_t *query, uint8_t *reference) {
    for (uint8_t i = 0; i < SIZE_OF_WIFI_SSID; i++) {
      uint8_t queryValue = *query;
      if ((*query) != (*reference))
        return false;
      if (0 == (*query))
        break;
    }
    return true;
  }

public:
  virtual ~RegistryOfWifiCredentials();
  bool isEmpty() { return registry.empty(); };
  std::size_t size() { return registry.size(); };
  void resetIterator() { iteratorOnCredentials = registry.begin(); };
  bool hasNext() { return iteratorOnCredentials != registry.end(); };
  WifiCredentials *getNextCredentials() {
    WifiCredentials *result;
    if (hasNext) {
      result = *iteratorOnCredentials;
      iteratorOnCredentials++;
    }
    return nullptr;
  };
  /**
   * @brief Registers a new entry, as the first entry.
   *
   * @param entry the entry to registers.
   */
  void registerCredentials(WifiCredentials *entry) {
    if (nullptr == entry)
      return;
    // remove entry with same ssid (MUST be single)
    for (std::vector<WifiCredentials *>::iterator it = registry.begin();
         it != registry.end(); it++) {
      if (isSameSsid(entry->ssid, (*it)->ssid)) {
        registry.erase(it);
      }
    }
    registry.insert(registry.begin(), entry);
    resetIterator();
  };
};

class WifiStationMonitor {
  // general handlers to call the subclass handlers
  // ========[ Wifi events handlers ]========
  /** @brief Handler for Wifi event : 'Station start'
   */
  static void handleWifiEventStationStart(void *arg,
                                          esp_event_base_t event_base,
                                          int32_t event_id, void *event_data);

  /** @brief Handler for Wifi event : 'Station disconnected'
   */
  static void handleWifiEventStationDisconnected(void *arg,
                                                 esp_event_base_t event_base,
                                                 int32_t event_id,
                                                 void *event_data);

  /** @brief Handler for Wifi event : 'Station WPS enrollee success'
   */
  static void
  handleWifiEventStationWpsEnrolleeSuccess(void *arg,
                                           esp_event_base_t event_base,
                                           int32_t event_id, void *event_data);

  /** @brief Handler for Wifi event : 'Station WPS enrollee failure'
   */
  static void
  handleWifiEventStationWpsEnrolleeFailure(void *arg,
                                           esp_event_base_t event_base,
                                           int32_t event_id, void *event_data);

  /** @brief Handler for Wifi event : 'Station WPS enrollee timeout'
   */
  static void
  handleWifiEventStationWpsEnrolleeTimeout(void *arg,
                                           esp_event_base_t event_base,
                                           int32_t event_id, void *event_data);

  // ========[ IP events handlers ]========
  /** @brief Handler for IP event : 'got ip'
   */
  static void handleIpEventGotIp(void *arg, esp_event_base_t event_base,
                                 int32_t event_id, void *event_data);

  /** @brief Handler for IP event : 'lost ip'
   */
  static void handleIpEventLostIp(void *arg, esp_event_base_t event_base,
                                  int32_t event_id, void *event_data);
public:
  virtual ~WifiStationMonitor();
  esp_err_t start();
  // event handlers to be implemented by subclasses.
  // ========[ Wifi events handlers ]========
  /** @brief Handler for Wifi event : 'Station start'
   */
  virtual void onWifiEventStationStart(esp_event_base_t event_base,
                                       int32_t event_id, void *event_data) = 0;

  /** @brief Handler for Wifi event : 'Station disconnected'
   */
  virtual void onWifiEventStationDisconnected(esp_event_base_t event_base,
                                              int32_t event_id,
                                              void *event_data) = 0;

  /** @brief Handler for Wifi event : 'Station WPS enrollee success'
   */
  virtual void onWifiEventStationWpsEnrolleeSuccess(esp_event_base_t event_base,
                                                    int32_t event_id,
                                                    void *event_data) = 0;

  /** @brief Handler for Wifi event : 'Station WPS enrollee failure'
   */
  virtual void onWifiEventStationWpsEnrolleeFailure(esp_event_base_t event_base,
                                                    int32_t event_id,
                                                    void *event_data) = 0;

  /** @brief Handler for Wifi event : 'Station WPS enrollee timeout'
   */
  virtual void onWifiEventStationWpsEnrolleeTimeout(esp_event_base_t event_base,
                                                    int32_t event_id,
                                                    void *event_data) = 0;

  // ========[ IP events handlers ]========
  /** @brief Handler for IP event : 'got ip'
   */
  virtual void onIpEventGotIp(esp_event_base_t event_base, int32_t event_id,
                              void *event_data) = 0;

  /** @brief Handler for IP event : 'lost ip'
   */
  virtual void onIpEventLostIp(esp_event_base_t event_base, int32_t event_id,
                               void *event_data) = 0;

};
// ################ SCRATCHPAD [end] ##################
// ################ TYPES [start] ##################
typedef enum { GOT_IP, LOST_IP } NetworkStationEventType;
class NetworkStation; // forward declaration

typedef struct {
  NetworkStationEventType type;
  NetworkStation *source;
} NetworkStationEvent;

/**
 * @brief Interface for being notified of network station events.
 */
class NetworkStationListener {
public:
  virtual ~NetworkStationListener();
  virtual void onNetworkStationEvent(NetworkStationEvent *event) = 0;
};

// ################ TYPES [end] ##################

/** @brief What the class is for.
 */
class NetworkStation {
private:
  std::vector<NetworkStationListener *> listeners;

protected:
  void notify(NetworkStationEvent *event) {
    if (listeners.empty()) {
      return;
    }
    for (std::vector<NetworkStationListener *>::iterator it = listeners.begin();
         it != listeners.end(); it++) {
      (*it)->onNetworkStationEvent(event);
    }
  };

public:
  virtual ~NetworkStation();
  NetworkStation *withListener(NetworkStationListener *listener) {
    listeners.push_back(listener);
    return this;
  };
};

#endif