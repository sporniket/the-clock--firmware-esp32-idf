
// header include
#include "NetworkTimeKeeperEsp32.hpp"

static constexpr char *TAG = (char *)"NetworkTimeKeeperEsp32";

#define INET6_ADDRSTRLEN 48

NetworkTimeKeeperEsp32::~NetworkTimeKeeperEsp32() {}
// write code here...
NetworkTimeKeeperEsp32::NetworkTimeKeeperEsp32(char *defaultSntpTimeServer) {
  ESP_LOGI(TAG, "Initializing SNTP");
  config = {
      .smooth_sync = false,
      .server_from_dhcp = true, // accept NTP offers from DHCP server
      .wait_for_sync = true,
      .start = false, // start SNTP service explicitly (after connecting)
      .sync_cb = NULL,
      .renew_servers_after_new_IP =
          true, // let esp-netif update configured SNTP server(s) after
                // receiving DHCP lease
      .ip_event_to_renew =
          IP_EVENT_STA_GOT_IP, // IP event on which we refresh the configuration
      .index_of_first_server =
          1, // updates from server num 1, leaving server 0 (from DHCP) intact
      .num_of_servers = (1),
      .servers = defaultSntpTimeServer,
  };
  // esp_netif_sntp_init(&config); will fail here : requires to be called when event loop is started/initialized.
}

bool doSomething(HostConfigurationDescription *configuration) {
  return (nullptr != configuration);
}

void NetworkTimeKeeperEsp32::onGotConfiguration(
    HostConfigurationDescription *configuration) {
  ESP_LOGI(TAG, "Starting SNTP");
  esp_netif_sntp_init(&config); // won't succeed at retrieving DHCP time server
  esp_netif_sntp_start();

  ESP_LOGI(TAG, "List of configured NTP servers:");

  for (uint8_t i = 0; i < SNTP_MAX_SERVERS; ++i) {
    if (esp_sntp_getservername(i)) {
      ESP_LOGI(TAG, "server %d: %s", i, esp_sntp_getservername(i));
    } else {
      // we have either IPv4 or IPv6 address, let's print it
      char buff[INET6_ADDRSTRLEN];
      ip_addr_t const *ip = esp_sntp_getserver(i);
      if (ipaddr_ntoa_r(ip, buff, INET6_ADDRSTRLEN) != NULL)
        ESP_LOGI(TAG, "server %d: %s", i, buff);
    }
  }

  // wait for time to be set
  time_t now = 0;
  struct tm timeinfo = {0};
  int retry = 0;
  const int retry_count = 15;
  while (esp_netif_sntp_sync_wait(2000 / portTICK_PERIOD_MS) ==
             ESP_ERR_TIMEOUT &&
         ++retry < retry_count) {
    ESP_LOGI(TAG, "Waiting for system time to be set... (%d/%d)", retry,
             retry_count);
  }
  if (retry >= retry_count) {
    ESP_LOGE(TAG, "COULD NOT sync time");
  } else {
    ESP_LOGE(TAG, "Could sync time");
  }
  char strftime_buf[64];
  // Set timezone to Paris, France and print local time
  setenv("TZ", "CET-1CEST-2,M3.5.0/02:00:00,M10.5.0/03:00:00", 1);
  tzset();
  time(&now);
  localtime_r(&now, &timeinfo);
  strftime(strftime_buf, sizeof(strftime_buf), "%c", &timeinfo);
  ESP_LOGI(TAG, "The current date/time with timezone is: %s", strftime_buf);
}

void NetworkTimeKeeperEsp32::onLostConfiguration() {}