
// header include
#include "WifiCredentialsRegistryDaoUsingNvs.hpp"

WifiCredentialsRegistryDaoUsingNvs::~WifiCredentialsRegistryDaoUsingNvs() {}
// write code here...

static const char *TAG_LOAD = "WifiCredentialsRegistryDaoUsingNvs::loadInto";
static const char *TAG_SAVE = "WifiCredentialsRegistryDaoUsingNvs::saveFrom";

static const char *FMT_KEY_COUNT = "count";
static const char *FMT_KEY_SSID = "%d.ssid";
static const char *FMT_KEY_KEYTYPE = "%d.ktype";
static const char *FMT_KEY_KEYPASS = "%d.key";

void WifiCredentialsRegistryDaoUsingNvs::logErrorGetItem(const char *tag,
                                                         esp_err_t err,
                                                         char *keyname) {
  switch (err) {
  case ESP_ERR_NVS_NOT_FOUND:
    ESP_LOGE(tag, "Nothing to read '%s,%s' from.", getDesignator()->c_str(),
             keyname);
    break;
  default:
    ESP_LOGE(tag, "Error (%s) reading '%s.%s' !", esp_err_to_name(err),
             getDesignator()->c_str(), keyname);
  }
}

void WifiCredentialsRegistryDaoUsingNvs::logErrorSetItem(const char *tag,
                                                         esp_err_t err,
                                                         char *keyname) {
  ESP_LOGE(tag, "Error (%s) writing '%s.%s'!", esp_err_to_name(err),
           getDesignator()->c_str(), keyname);
}

void WifiCredentialsRegistryDaoUsingNvs::logErrorCommit(const char *tag,
                                                        esp_err_t err,
                                                        char *keyname) {
  ESP_LOGE(tag, "Error (%s) commiting '%s.%s'!", esp_err_to_name(err),
           getDesignator()->c_str(), keyname);
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

  char keyname[16];

  // init storage before reading
  uint8_t count = 0;

  // read the number of item to read
  sprintf(keyname, FMT_KEY_COUNT);
  err = handle->get_item(keyname, count);
  if (err != ESP_OK) {
    logErrorGetItem(TAG_LOAD, err, keyname);
    return false;
  }
  if (0 == count) {
    // nothing to read, done
    return true;
  }

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
      logErrorGetItem(TAG_SAVE, err, keyname);
      return false;
    }

    // read key
    sprintf(keyname, FMT_KEY_KEYPASS, item);
    err = handle->get_string(keyname, itemKey, MAX_LENGTH_OF_KEYPASS);
    if (err != ESP_OK) {
      logErrorGetItem(TAG_LOAD, err, keyname);
      return false;
    }

    // read keytype
    sprintf(keyname, FMT_KEY_KEYTYPE, item);
    err = handle->get_item(keyname, keyType);
    if (err != ESP_OK) {
      logErrorGetItem(TAG_LOAD, err, keyname);
      return false;
    }
    WifiCredentials wc((uint8_t *)itemSsid, (uint8_t *)itemKey, keyType);
    temp.put(&wc);
  }
  // At this point, temp contains the registry in reverse order.
  while (temp.hasNext()) {
    recipient->put(temp.next());
  }
  // At this point, recipient is filled with the registry in expected order.
  return true;
}

bool WifiCredentialsRegistryDaoUsingNvs::saveFrom(
    WifiCredentialsRegistry *const source) {
  esp_err_t err;
  std::unique_ptr<nvs::NVSHandle> handle =
      nvs::open_nvs_handle(getDesignator()->c_str(), NVS_READWRITE, &err);
  if (err != ESP_OK) {
    ESP_LOGE(TAG_LOAD, "Error (%s) opening NVS handle!", esp_err_to_name(err));
    return false;
  }
  char keyname[16];
  source->rewind();
  while (source->hasNext()) {
    WifiCredentials *item = source->next();
    // write ssid
    sprintf(keyname, FMT_KEY_SSID, item);
    err = handle->set_string(keyname, (char *)(item->getSsid()));
    if (err != ESP_OK) {
      logErrorSetItem(TAG_SAVE, err, keyname);
      return false;
    }
    err = handle->commit();
    if (err != ESP_OK) {
      logErrorCommit(TAG_SAVE, err, keyname);
      return false;
    }

    // write key
    sprintf(keyname, FMT_KEY_KEYPASS, item);
    err = handle->set_string(keyname, (char *)(item->getKey()));
    if (err != ESP_OK) {
      logErrorSetItem(TAG_SAVE, err, keyname);
      return false;
    }

    // write keytype
    sprintf(keyname, FMT_KEY_KEYTYPE, item);
    err = handle->set_item(keyname, item->getKeyType());
    if (err != ESP_OK) {
      logErrorSetItem(TAG_SAVE, err, keyname);
      return false;
    }
    err = handle->commit();
    if (err != ESP_OK) {
      logErrorCommit(TAG_SAVE, err, keyname);
      return false;
    }
  }
  // at this point the whole registry has been written without problem
  // read the number of item to read
  sprintf(keyname, FMT_KEY_COUNT);
  err = handle->set_item(keyname, source->getSize());
  if (err != ESP_OK) {
    logErrorSetItem(TAG_SAVE, err, keyname);
    return false;
  }
  err = handle->commit();
  if (err != ESP_OK) {
    logErrorCommit(TAG_SAVE, err, keyname);
    return false;
  }

  return true;
}
