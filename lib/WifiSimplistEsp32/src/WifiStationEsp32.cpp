// Copyright 2023 David SPORN
// ---
// This file is part of 'Wifi Simplist for ESP32'.
// ---
// 'Wifi Simplist for ESP32' is free software: you can redistribute it and/or
// modify it under the terms of the GNU General Public License as published
// by the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.

// 'Wifi Simplist for ESP32' is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General
// Public License for more details.

// You should have received a copy of the GNU General Public License along
// with 'Wifi Simplist for ESP32'. If not, see <https://www.gnu.org/licenses/>. 

// header include
#include "WifiStationEsp32.hpp"

WifiStationEsp32::~WifiStationEsp32() {}
// write code here...

// static
void WifiStationEsp32::install() {
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
  changeStateToInstalled();
}

// ========[ state management ]========
bool WifiStationEsp32::changeStateToConnected() {
  bool canSwitch = true;
  if (TRYING_KNOWN_ACCESS_POINTS != state && TRYING_WPS != state) {
    ESP_LOGW(TAG, "Not in a state that can be changed to 'connected'.");
    canSwitch = false;
  }
  if (canSwitch) {
    // TODO update known APs registry and save it

    state = CONNECTED;
  }
  return canSwitch;
}
bool WifiStationEsp32::changeStateToNotConnectedAndIdle() {
  bool canSwitch = true;
  if (BEFORE_INIT == state) {
    ESP_LOGW(TAG, "Require to have been installed first.");
    canSwitch = false;
  }
  if (canSwitch) {
    ESP_ERROR_CHECK(esp_wifi_wps_disable());

    state = NOT_CONNECTED_AND_IDLE;
  }
  return canSwitch;
}
bool WifiStationEsp32::changeStateToTryingWps() {
  bool canSwitch = true;
  if (DONE_TRYING_KNOWN_ACCESS_POINTS != state) {
    ESP_LOGW(TAG, "Not in a state that can be changed to 'trying wps.");
    canSwitch = false;
  }
  if (canSwitch) {
    startWps();

    state = TRYING_WPS;
  }
  return canSwitch;
}
bool WifiStationEsp32::changeStateToDoneTryingKnownAccessPoints() {
  bool canSwitch = true;
  if (TRYING_KNOWN_ACCESS_POINTS != state) {
    ESP_LOGW(TAG, "Not in a state that can be changed to 'done trying known "
                  "access points'.");
    canSwitch = false;
  }
  if (canSwitch) {
    state = DONE_TRYING_KNOWN_ACCESS_POINTS;
  }
  return canSwitch;
}
bool WifiStationEsp32::changeStateToTryingKnownAccessPoints() {
  bool canSwitch = true;
  if (INSTALLED != state && NOT_CONNECTED_AND_IDLE != state) {
    ESP_LOGW(TAG, "Not in a state that can be changed to 'trying known "
                  "access points'.");
    canSwitch = false;
  }
  if (canSwitch) {
    ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_STA, &wifi_config_known_ap));
    wcreg.rewind();

    state = TRYING_KNOWN_ACCESS_POINTS;
  }
  return canSwitch;
}
bool WifiStationEsp32::changeStateToInstalled() {
  bool canSwitch = true;
  if (state != READY_TO_INSTALL) {
    ESP_LOGW(TAG, "Not in a state that can be changed to 'installed'.");
    canSwitch = false;
  }
  if (canSwitch) {
    state = INSTALLED;
  }
  return canSwitch;
}
bool WifiStationEsp32::changeStateToReadyToInstall() {
  bool canSwitch = true;
  if (state != BEFORE_INIT) {
    ESP_LOGW(TAG, "Not in a state that can be changed to 'ready to install'.");
    canSwitch = false;
  }
  if (nullptr == wcregdao) {
    ESP_LOGW(TAG, "Cannot be ready to install until wcregdao is defined.");
    canSwitch = false;
  }
  if (hostConfigurationListeners.empty()) {
    ESP_LOGW(TAG, "Cannot be ready to install until at least one host "
                  "configuration listener is registered.");
    canSwitch = false;
  }
  if (canSwitch) {
    // TODO restore known APs registry using dao

    state = READY_TO_INSTALL;
  }
  return canSwitch;
}

bool WifiStationEsp32::tryNextKnownAccessPoints() {
  if (!changeStateToTryingKnownAccessPoints()) {
    return false;
  }
  if (!wcreg.hasNext()) {
    changeStateToDoneTryingKnownAccessPoints();
    return false;
  }
  WifiCredentials *wc = wcreg.next();
  ESP_LOGI(TAG, "Connecting to known SSID: %s", wc->getSsid());
  setupCredentials(wifi_config_known_ap, wc->getSsid(), wc->getKey());
  esp_wifi_connect();
  return true;
}

bool WifiStationEsp32::tryNextWpsAccessPoint() {
  if (0 < wpsCredentialsCount) {
    ++wpsCredentialsCurrent;
    if (wpsCredentialsCurrent < wpsCredentialsCount) {
      ESP_LOGI(TAG, "Tried all access points");
      wpsCredentialsCount = 0; // drop all the scanned access points
      changeStateToNotConnectedAndIdle();
      return false;
    }
    ESP_LOGI(TAG, "Connecting to SSID: %s",
             wpsCredentials[wpsCredentialsCurrent].sta.ssid);
    ESP_ERROR_CHECK(esp_wifi_set_config(
        WIFI_IF_STA, &wpsCredentials[wpsCredentialsCurrent]));
    tryAgainConnect();
  } else {
    ESP_LOGI(TAG, "There was no WPS access points.");
    changeStateToNotConnectedAndIdle();
  }
}

void WifiStationEsp32::notifyGotHostConfiguration(
    HostConfigurationDescription *desc) {
  for (std::unordered_set<HostConfigurationEventListener *>::iterator iter =
           hostConfigurationListeners.begin();
       iter != hostConfigurationListeners.end(); iter++) {
    HostConfigurationEventListener *listener = *iter;
    listener->onGotConfiguration(desc);
  }
}

void WifiStationEsp32::notifyLostHostConfiguration() {
  for (std::unordered_set<HostConfigurationEventListener *>::iterator iter =
           hostConfigurationListeners.begin();
       iter != hostConfigurationListeners.end(); iter++) {
    HostConfigurationEventListener *listener = *iter;
    listener->onLostConfiguration();
  }
}

void WifiStationEsp32::init() {
  if (!wcregdao->loadInto(&wcreg)) {
    ESP_LOGW(TAG,
             "Could not restore saved credentials -or nothing to restore-.");
  }
  changeStateToReadyToInstall();
}

void WifiStationEsp32::tryToConnect() {
  if (READY_TO_INSTALL == state) {
    install();
  }
  if (WifiStationLifecycleState::NOT_CONNECTED_AND_IDLE == state ||
      WifiStationLifecycleState::INSTALLED) {
    changeStateToTryingKnownAccessPoints();

    if (!tryNextKnownAccessPoints()) {
      ESP_LOGI(TAG, "No known access point, switch to wps...");
      changeStateToTryingWps();
    }
  }
}

// ========[ Wifi events handlers ]========
void WifiStationEsp32::handleWifiEventStationStart(void *arg,
                                                   esp_event_base_t event_base,
                                                   int32_t event_id,
                                                   void *event_data) {
  ESP_LOGI(TAG, "WIFI_EVENT_STA_START");
}

void WifiStationEsp32::handleWifiEventStationDisconnected(
    void *arg, esp_event_base_t event_base, int32_t event_id,
    void *event_data) {
  ESP_LOGI(TAG, "WIFI_EVENT_STA_DISCONNECTED");
  // select next step according to state
  if (TRYING_KNOWN_ACCESS_POINTS == state) {
    ESP_LOGI(TAG, "Failed to connect to a known access point.");
    if (!tryNextKnownAccessPoints()) {
      ESP_LOGI(TAG, "No known access point any more, switch to wps...");
      changeStateToTryingWps();
    }
  } else if (TRYING_WPS == state) {
    if (remainingRetriesForAccessPoint > 0) {
      tryAgainConnect();
    } else if (0 < wpsCredentialsCount &&
               wpsCredentialsCurrent < wpsCredentialsCount) {
      // TODO to rewrite
      tryNextWpsAccessPoint();
      tryAgainConnect();
    } else {
      ESP_LOGI(TAG, "Failed to connect to an available access point.");
      changeStateToNotConnectedAndIdle();
    }
  } else {
    ESP_LOGW(TAG, "UNKNOWN STATE, failed to connect anyway, go back to idle.");
    changeStateToNotConnectedAndIdle();
  }
}

void WifiStationEsp32::handleWifiEventStationWpsEnrolleeSuccess(
    void *arg, esp_event_base_t event_base, int32_t event_id,
    void *event_data) {
  ESP_LOGI(TAG, "WIFI_EVENT_STA_WPS_ER_SUCCESS");
  wifi_event_sta_wps_er_success_t *evt =
      (wifi_event_sta_wps_er_success_t *)event_data;
  if (evt) {
    // Multiple Access point, cache the credentials reported by the event.
    wpsCredentialsCount = evt->ap_cred_cnt;
    for (int i = 0; i < wpsCredentialsCount; i++) {
      memcpy(wpsCredentials[i].sta.ssid, evt->ap_cred[i].ssid,
             sizeof(evt->ap_cred[i].ssid));
      memcpy(wpsCredentials[i].sta.password, evt->ap_cred[i].passphrase,
             sizeof(evt->ap_cred[i].passphrase));
    }
    useWpsCredentials(wpsCredentialsCurrent);
  } else {
    ESP_LOGI(TAG, "Only one access point, ready to connect.");
  }
  ESP_ERROR_CHECK(esp_wifi_wps_disable());
  esp_wifi_connect();
}

void WifiStationEsp32::handleWifiEventStationWpsEnrolleeFailure(
    void *arg, esp_event_base_t event_base, int32_t event_id,
    void *event_data) {
  ESP_LOGI(TAG, "WIFI_EVENT_STA_WPS_ER_FAILED");
  if (0 == remainingRetriesForWps) {
    ESP_LOGI(TAG, "Still cannot connect using WPS, gives up.");
    changeStateToNotConnectedAndIdle();
  } else {
    tryAgainWps();
  }
}

void WifiStationEsp32::handleWifiEventStationWpsEnrolleeTimeout(
    void *arg, esp_event_base_t event_base, int32_t event_id,
    void *event_data) {
  ESP_LOGI(TAG, "WIFI_EVENT_STA_WPS_ER_TIMEOUT");
  if (0 == remainingRetriesForWps) {
    ESP_LOGI(TAG, "Still cannot connect using WPS, gives up.");
    changeStateToNotConnectedAndIdle();
  } else {
    tryAgainWps();
  }
}

// ========[ IP events handlers ]========
//#define esp_ip4_addr_get_byte(ipaddr, idx) (((const
// uint8_t*)(&(ipaddr)->addr))[idx])

void WifiStationEsp32::handleIpEventGotIp(void *arg,
                                          esp_event_base_t event_base,
                                          int32_t event_id, void *event_data) {
  ip_event_got_ip_t *event = (ip_event_got_ip_t *)event_data;
  ESP_LOGI(TAG, "got ip: " IPSTR, IP2STR(&event->ip_info.ip));
  ESP_LOGI(TAG, "got network mask: " IPSTR, IP2STR(&event->ip_info.netmask));
  ESP_LOGI(TAG, "got gateway: " IPSTR, IP2STR(&event->ip_info.gw));

  changeStateToConnected() ;

  // copy ip address
  uint32_t currentIpv4 = event->ip_info.ip.addr;
  uint8_t *ipBytes = (uint8_t *)&(event->ip_info.ip.addr);
  HostConfigurationDescription desc = {
      .ipAddressFormat = IPV4,
      .ipAddress = {.v4 = {esp_ip4_addr_get_byte(&event->ip_info.ip, 0),
                           esp_ip4_addr_get_byte(&event->ip_info.ip, 1),
                           esp_ip4_addr_get_byte(&event->ip_info.ip, 2),
                           esp_ip4_addr_get_byte(&event->ip_info.ip, 3)}},
      .subNetworkMask =
          {.v4 = {esp_ip4_addr_get_byte(&event->ip_info.netmask, 0),
                  esp_ip4_addr_get_byte(&event->ip_info.netmask, 1),
                  esp_ip4_addr_get_byte(&event->ip_info.netmask, 2),
                  esp_ip4_addr_get_byte(&event->ip_info.netmask, 3)}},
      .defaultGateway = {.v4 = {esp_ip4_addr_get_byte(&event->ip_info.gw, 0),
                                esp_ip4_addr_get_byte(&event->ip_info.gw, 1),
                                esp_ip4_addr_get_byte(&event->ip_info.gw, 2),
                                esp_ip4_addr_get_byte(&event->ip_info.gw, 3)}}};
  notifyGotHostConfiguration(&desc);
}
