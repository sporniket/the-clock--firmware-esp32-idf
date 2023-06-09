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

void WifiStationEsp32::init() {}

void WifiStationEsp32::start() {}

void WifiStationEsp32::stop() {}

void WifiStationEsp32::tryWps() {}

bool WifiStationEsp32::isTryingWps() { return false; }

// ========[ Wifi events handlers ]========
void WifiStationEsp32::handleWifiEventStationStart(void *arg,
                                                   esp_event_base_t event_base,
                                                   int32_t event_id,
                                                   void *event_data) {
  // do stuff
}

void WifiStationEsp32::handleWifiEventStationDisconnected(
    void *arg, esp_event_base_t event_base, int32_t event_id,
    void *event_data) {
  // do stuff
}

void WifiStationEsp32::handleWifiEventStationWpsEnrolleeSuccess(
    void *arg, esp_event_base_t event_base, int32_t event_id,
    void *event_data) {
  // do stuff
}

void WifiStationEsp32::handleWifiEventStationWpsEnrolleeFailure(
    void *arg, esp_event_base_t event_base, int32_t event_id,
    void *event_data) {
  // do stuff
}

void WifiStationEsp32::handleWifiEventStationWpsEnrolleeTimeout(
    void *arg, esp_event_base_t event_base, int32_t event_id,
    void *event_data) {
  // do stuff
}

// ========[ IP events handlers ]========
//#define esp_ip4_addr_get_byte(ipaddr, idx) (((const
// uint8_t*)(&(ipaddr)->addr))[idx])

void WifiStationEsp32::handleIpEventGotIp(void *arg,
                                          esp_event_base_t event_base,
                                          int32_t event_id, void *event_data) {
  ip_event_got_ip_t *event = (ip_event_got_ip_t *)event_data;

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
