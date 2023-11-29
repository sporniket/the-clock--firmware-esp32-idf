
// header include
#include "WifiEventDispatcherEsp32.hpp"

WifiEventDispatcherEsp32::~WifiEventDispatcherEsp32() {}
// write code here...

static constexpr char *TAG = (char *)"WifiEventDispatcherEsp32";

/**
 * @brief The listeners that will receive the events.
 */
static std::vector<WifiStationEsp32EventHandler *> listeners;

// ========[ Code generator macros ]========
// generate the handler that will loop through each listener to dispatch the
// event.
#define DISPATCH(event, event_name, handler_name)                              \
  case event:                                                                  \
    ESP_LOGI(TAG, event_name);                                                 \
    for (std::vector<WifiStationEsp32EventHandler *>::iterator it =            \
             listeners.begin();                                                \
         it != listeners.end(); it++) {                                        \
      (*it)->handler_name(event_data);                                         \
    }                                                                          \
    ESP_LOGI(TAG, "DONE dispatch");                                            \
    break;

// ========[ Event dispatcher ]========
static void dispatchEvents(void *arg, esp_event_base_t event_base,
                           int32_t event_id, void *event_data) {
  if (event_base == WIFI_EVENT) {
    switch (event_id) {
      DISPATCH(WIFI_EVENT_STA_START, "WIFI_EVENT_STA_START",
               handleWifiEventStationStart)
      DISPATCH(WIFI_EVENT_STA_CONNECTED, "WIFI_EVENT_STA_CONNECTED",
               handleWifiEventStationConnected)
      DISPATCH(WIFI_EVENT_STA_DISCONNECTED, "WIFI_EVENT_STA_DISCONNECTED",
               handleWifiEventStationDisconnected)
      DISPATCH(WIFI_EVENT_STA_WPS_ER_SUCCESS, "WIFI_EVENT_STA_WPS_ER_SUCCESS",
               handleWifiEventStationWpsEnrolleeSuccess)
      DISPATCH(WIFI_EVENT_STA_WPS_ER_FAILED, "WIFI_EVENT_STA_WPS_ER_FAILED",
               handleWifiEventStationWpsEnrolleeFailure)
      DISPATCH(WIFI_EVENT_STA_WPS_ER_TIMEOUT, "WIFI_EVENT_STA_WPS_ER_TIMEOUT",
               handleWifiEventStationWpsEnrolleeTimeout)
    }
  } else if (event_base == IP_EVENT) {
    switch (event_id) {
      DISPATCH(IP_EVENT_STA_GOT_IP, "IP_EVENT_STA_GOT_IP", handleIpEventGotIp)
      DISPATCH(IP_EVENT_STA_LOST_IP, "IP_EVENT_STA_LOST_IP",
               handleIpEventLostIp)
    }
  }
}

void WifiEventDispatcherEsp32::installHandlers() {
  ESP_LOGI(TAG, "Installing wifi station event handlers...");
  // setup station mode
  ESP_ERROR_CHECK(esp_netif_init());
  ESP_ERROR_CHECK(esp_event_loop_create_default());

  // install event handlers...
  ESP_ERROR_CHECK(
      esp_event_handler_register(WIFI_EVENT, WIFI_EVENT_STA_START,
                                 (esp_event_handler_t)&dispatchEvents, NULL));
  ESP_ERROR_CHECK(
      esp_event_handler_register(WIFI_EVENT, WIFI_EVENT_STA_DISCONNECTED,
                                 (esp_event_handler_t)&dispatchEvents, NULL));
  ESP_ERROR_CHECK(
      esp_event_handler_register(WIFI_EVENT, WIFI_EVENT_STA_WPS_ER_SUCCESS,
                                 (esp_event_handler_t)&dispatchEvents, NULL));
  ESP_ERROR_CHECK(
      esp_event_handler_register(WIFI_EVENT, WIFI_EVENT_STA_WPS_ER_FAILED,
                                 (esp_event_handler_t)&dispatchEvents, NULL));
  ESP_ERROR_CHECK(
      esp_event_handler_register(WIFI_EVENT, WIFI_EVENT_STA_WPS_ER_TIMEOUT,
                                 (esp_event_handler_t)&dispatchEvents, NULL));
  ESP_ERROR_CHECK(
      esp_event_handler_register(IP_EVENT, IP_EVENT_STA_GOT_IP,
                                 (esp_event_handler_t)&dispatchEvents, NULL));
  ESP_ERROR_CHECK(
      esp_event_handler_register(IP_EVENT, IP_EVENT_STA_LOST_IP,
                                 (esp_event_handler_t)&dispatchEvents, NULL));
}

void WifiEventDispatcherEsp32::addListener(
    WifiStationEsp32EventHandler *listener) {
  if (nullptr == listener) {
    return;
  }
  for (std::vector<WifiStationEsp32EventHandler *>::iterator it =
           listeners.begin();
       it != listeners.end(); it++) {
    if (listener == *it) {
      return; // already registered.
    }
  }
  listeners.push_back(listener);
}
