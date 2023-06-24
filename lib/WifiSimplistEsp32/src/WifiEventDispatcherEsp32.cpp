
// header include
#include "WifiEventDispatcherEsp32.hpp"

WifiEventDispatcherEsp32::~WifiEventDispatcherEsp32() {}
// write code here...

static constexpr char *TAG = (char *)"WifiEventDispatcherEsp32";

/**
 * @brief The listeners that will receive the events.
 */
static std::vector<WifiStationEsp32 *> listeners;

static void handleWifiEventStationStart(void *arg, esp_event_base_t event_base,
                                        int32_t event_id, void *event_data);

static void handleWifiEventStationDisconnected(void *arg, esp_event_base_t event_base,
                                        int32_t event_id, void *event_data);

static void handleWifiEventStationWpsEnrolleeSuccess(void *arg, esp_event_base_t event_base,
                                        int32_t event_id, void *event_data);

static void handleWifiEventStationWpsEnrolleeFailure(void *arg, esp_event_base_t event_base,
                                        int32_t event_id, void *event_data);

static void handleWifiEventStationWpsEnrolleeTimeout(void *arg, esp_event_base_t event_base,
                                        int32_t event_id, void *event_data);

static void handleIpEventGotIp(void *arg, esp_event_base_t event_base,
                                        int32_t event_id, void *event_data);

static void handleIpEventLostIp(void *arg, esp_event_base_t event_base,
                                        int32_t event_id, void *event_data);

void WifiEventDispatcherEsp32::installHandlers() {
  ESP_LOGI(TAG, "Installing wifi station event handlers...");
  // setup station mode
  ESP_ERROR_CHECK(esp_netif_init());
  ESP_ERROR_CHECK(esp_event_loop_create_default());
  esp_netif_t *sta_netif = esp_netif_create_default_wifi_sta();
  assert(sta_netif);

  wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
  ESP_ERROR_CHECK(esp_wifi_init(&cfg));

  // install event handlers...
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
  static void handler_name(void *arg, esp_event_base_t event_base,             \
                           int32_t event_id, void *event_data) {               \
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

// ========[ --- ]========
