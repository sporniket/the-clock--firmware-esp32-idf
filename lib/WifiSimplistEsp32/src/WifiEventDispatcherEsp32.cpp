
// header include
#include "WifiEventDispatcherEsp32.hpp"

WifiEventDispatcherEsp32::~WifiEventDispatcherEsp32() {}
// write code here...

/**
 * @brief The listeners that will receive the events.
 */
static std::vector<WifiStationEsp32 *> listeners;

void WifiEventDispatcherEsp32::install() {
  ESP_LOGI(TAG, "Installing wifi station event handlers...");
  ESP_ERROR_CHECK(esp_event_handler_register(
      WIFI_EVENT, WIFI_EVENT_STA_START,
      (esp_event_handler_t)&handleWifiEventStationStart, NULL));
  ESP_ERROR_CHECK(esp_event_handler_register(
      WIFI_EVENT, WIFI_EVENT_STA_DISCONNECTED,
      (esp_event_handler_t)&handleWifiEventStationDisconnected, NULL));
  ESP_ERROR_CHECK(esp_event_handler_register(
      WIFI_EVENT, WIFI_EVENT_STA_WPS_ER_SUCCESS,
      (esp_event_handler_t)&handleWifiEventStationWpsEnrolleeSuccess, NULL));
  ESP_ERROR_CHECK(esp_event_handler_register(
      WIFI_EVENT, WIFI_EVENT_STA_WPS_ER_FAILED,
      (esp_event_handler_t)&handleWifiEventStationWpsEnrolleeFailure, NULL));
  ESP_ERROR_CHECK(esp_event_handler_register(
      WIFI_EVENT, WIFI_EVENT_STA_WPS_ER_TIMEOUT,
      (esp_event_handler_t)&handleWifiEventStationWpsEnrolleeTimeout, NULL));
  ESP_ERROR_CHECK(esp_event_handler_register(
      IP_EVENT, IP_EVENT_STA_GOT_IP, (esp_event_handler_t)&handleIpEventGotIp,
      NULL));
  ESP_ERROR_CHECK(esp_event_handler_register(
      IP_EVENT, IP_EVENT_STA_LOST_IP, (esp_event_handler_t)&handleIpEventLostIp,
      NULL));
}

void WifiEventDispatcherEsp32::addListener(WifiStationEsp32 *listener) {
  if (nullptr == listener) {
    return;
  }
  for (std::vector<WifiStationEsp32 *>::iterator it = listeners.begin();
       it != listeners.end(); it++) {
    if (listener == *it) {
      return; // already registered.
    }
  }
  listeners.push_back(listener);
}

// ========[ Code generator macros ]========
// generate the handler that will loop through each listener to dispatch the
// event.
#define GENERATE_HANDLER(event_name, handler_name)                             \
  void WifiEventDispatcherEsp32::handler_name(                                 \
      void *arg, esp_event_base_t event_base, int32_t event_id,                \
      void *event_data) {                                                      \
    ESP_LOGI(TAG, event_name);                                                 \
    for (std::vector<WifiStationEsp32 *>::iterator it = listeners.begin();     \
         it != listeners.end(); it++) {                                        \
      (*it)->handler_name(arg, event_base, event_id, event_data);              \
    }                                                                          \
  }

// ========[ Wifi events handlers ]========
GENERATE_HANDLER("WIFI_EVENT_STA_START", handleWifiEventStationStart)
GENERATE_HANDLER("WIFI_EVENT_STA_DISCONNECTED",
                 handleWifiEventStationDisconnected)
GENERATE_HANDLER("WIFI_EVENT_STA_WPS_ER_SUCCESS",
                 handleWifiEventStationWpsEnrolleeSuccess)
GENERATE_HANDLER("WIFI_EVENT_STA_WPS_ER_FAILED",
                 handleWifiEventStationWpsEnrolleeFailure)
GENERATE_HANDLER("WIFI_EVENT_STA_WPS_ER_TIMEOUT",
                 handleWifiEventStationWpsEnrolleeTimeout)

// ========[ IP events handlers ]========
GENERATE_HANDLER("IP_EVENT_STA_GOT_IP", handleIpEventGotIp)
GENERATE_HANDLER("IP_EVENT_STA_LOST_IP", handleIpEventLostIp)
