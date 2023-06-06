
// header include
#include "WifiCredentialsRegistryDaoUsingNvs.hpp"

WifiCredentialsRegistryDaoUsingNvs::~WifiCredentialsRegistryDaoUsingNvs() {}
// write code here...

static const char *TAG_LOAD = "WifiCredentialsRegistryDaoUsingNvs::loadInto";
static const char *TAG_SAVE = "WifiCredentialsRegistryDaoUsingNvs::saveFrom";

static const char *FMT_KEY_SSID = "%d.ssid";
static const char *FMT_KEY_KEYTYPE = "%d.ktype";
static const char *FMT_KEY_KEYPASS = "%d.key";

void WifiCredentialsRegistryDaoUsingNvs::logErrorGetItem(const char *tag,
                                                         esp_err_t err) {
  switch (err) {
  case ESP_ERR_NVS_NOT_FOUND:
    ESP_LOGE(TAG_LOAD, "Nothing to read");
    break;
  default:
    ESP_LOGE(TAG_LOAD, "Error (%s) reading!\n", esp_err_to_name(err));
  }
}

bool WifiCredentialsRegistryDaoUsingNvs::loadInto(
    WifiCredentialsRegistry *recipient) {
  esp_err_t err;
  std::unique_ptr<nvs::NVSHandle> handle =
      nvs::open_nvs_handle(getDesignator()->c_str(), NVS_READWRITE, &err);
  if (err != ESP_OK) {
    ESP_LOGE(TAG_LOAD, "Error (%s) opening NVS handle!", esp_err_to_name(err));
    return false;
  }
  uint8_t count = 0;
  err = handle->get_item("count", count);
  if (err != ESP_OK) {
    logErrorGetItem(TAG_LOAD, err);
    return false;
  }

  char keyname[16];
  char itemSsid[MAX_LENGTH_OF_SSID];
  char itemKey[MAX_LENGTH_OF_KEYPASS];
  WifiKeyType keyType;
  WifiCredentialsRegistry temp(recipient->getCapacity());
  for (uint8_t item = count - 1; item >= 0; item--) {
    // read items in reverse order

    // init storage before reading
    itemSsid[0] = 0;
    itemKey[0] = 0;
    keyType = WifiKeyType::PASSWORD;
    
    // read ssid
    sprintf(keyname, FMT_KEY_SSID, item);
    err = handle->get_string(keyname, itemSsid, MAX_LENGTH_OF_SSID);
    if (err != ESP_OK) {
      logErrorGetItem(TAG_LOAD, err);
      return false;
    }

    // read key
    sprintf(keyname, FMT_KEY_KEYPASS, item);
    err = handle->get_string(keyname, itemKey, MAX_LENGTH_OF_KEYPASS);
    if (err != ESP_OK) {
      logErrorGetItem(TAG_LOAD, err);
      return false;
    }

    // read keytype
  }
  return true;
}

bool WifiCredentialsRegistryDaoUsingNvs::saveFrom(
    const WifiCredentialsRegistry *const source) {}
