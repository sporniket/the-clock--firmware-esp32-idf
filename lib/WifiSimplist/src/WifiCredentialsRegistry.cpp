
// header include
#include "WifiCredentialsRegistry.hpp"

WifiCredentialsRegistry::~WifiCredentialsRegistry() {}
// write code here...

WifiCredentials *WifiCredentialsRegistry::find(WifiCredentials *query) {
  for (std::vector<WifiCredentials *>::iterator it = registry.begin();
       it != registry.end(); it++) {
    WifiCredentials *candidate = *it;
    if (std::equal(std::begin(candidate->ssid), std::end(candidate->ssid),
                   std::begin(query->ssid), std::end(query->ssid))) {
      return candidate;
    }
  }
  return nullptr;
}

void WifiCredentialsRegistry::moveRankUpExcept(WifiCredentials *query) {
  for (std::vector<WifiCredentials *>::iterator it = registry.begin();
       it != registry.end(); it++) {
    WifiCredentials *entry = *it;
    if (entry != query) {
      entry->rank++;
    }
  }
}

void WifiCredentialsRegistry::moveRankUpUntil(WifiCredentials *query) {
  for (std::vector<WifiCredentials *>::iterator it = registry.begin();
       it != registry.end(); it++) {
    WifiCredentials *entry = *it;
    if (entry == query) {
      break;
    }
    entry->rank++;
  }
}

void WifiCredentialsRegistry::moveRankDownAfter(WifiCredentials *query) {
  bool found = false;
  for (std::vector<WifiCredentials *>::iterator it = registry.begin();
       it != registry.end(); it++) {
    WifiCredentials *entry = *it;
    if (entry == query) {
      found = true;
      continue;
    }
    if (found) {
      entry->rank--;
    }
  }
}

void WifiCredentialsRegistry::rearrange() {
  std::sort(registry.begin(), registry.end(), compareEntriesByRank);
  rewind();
}

WifiCredentialsRegistry *
WifiCredentialsRegistry::put(WifiCredentials *const credentials) {
  WifiCredentials *entry = find(credentials);
  if (nullptr == entry) {
    // insert
    entry = new WifiCredentials();
    std::copy(std::begin(credentials->ssid), std::end(credentials->ssid),
              std::begin(entry->ssid));
    std::copy(std::begin(credentials->key), std::end(credentials->key),
              std::begin(entry->key));
    entry->keyType = credentials->keyType;
    entry->rank = 0;
    registry.push_back(entry);

  } else {
    // update
    entry->keyType = credentials->keyType;
    std::copy(std::begin(credentials->key), std::end(credentials->key),
              std::begin(entry->key));
    entry->rank = 0;
  }
  moveRankUpUntil(entry);
  rearrange();
  return this;
}

WifiCredentialsRegistry *
WifiCredentialsRegistry::remove(WifiCredentials *const credentials) {
  WifiCredentials *entry;
  std::vector<WifiCredentials *>::iterator it = registry.begin();
  while (it != registry.end()) {
    entry = *it;
    if (std::equal(std::begin(entry->ssid), std::end(entry->ssid),
                   std::begin(credentials->ssid),
                   std::end(credentials->ssid))) {
      moveRankDownAfter(entry);
      registry.erase(it);
      return rewind();
    }
    it++;
  }
  return this;
}

WifiCredentialsRegistry *
WifiCredentialsRegistry::setPreferred(WifiCredentials *const credentials) {
  WifiCredentials *entry = find(credentials);
  if (nullptr != entry) {
    moveRankUpUntil(entry);
    rearrange();
  }
  return this;
}
