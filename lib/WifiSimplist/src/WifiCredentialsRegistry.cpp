// Copyright 2023 David SPORN
// ---
// This file is part of 'Wifi Simplist'.
// ---
// 'Wifi Simplist' is free software: you can redistribute it and/or
// modify it under the terms of the GNU General Public License as published
// by the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.

// 'Wifi Simplist' is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General
// Public License for more details.

// You should have received a copy of the GNU General Public License along
// with 'Wifi Simplist'. If not, see <https://www.gnu.org/licenses/>. 

// header include
#include "WifiCredentialsRegistry.hpp"

WifiCredentialsRegistry::~WifiCredentialsRegistry() {}
// write code here...

WifiCredentials *WifiCredentialsRegistry::find(WifiCredentials *query) {
  for (std::vector<WifiCredentials *>::iterator it = registry.begin();
       it != registry.end(); it++) {
    WifiCredentials *candidate = *it;
    if (candidate->isSameSsid(query)) {
      return candidate;
    }
  }
  return nullptr;
}

void WifiCredentialsRegistry::moveRankDownExcept(WifiCredentials *query) {
  for (std::vector<WifiCredentials *>::iterator it = registry.begin();
       it != registry.end(); it++) {
    WifiCredentials *entry = *it;
    if (entry != query) {
      entry->rankDown();
    }
  }
}

void WifiCredentialsRegistry::moveRankDownUntil(WifiCredentials *query) {
  for (std::vector<WifiCredentials *>::iterator it = registry.begin();
       it != registry.end(); it++) {
    WifiCredentials *entry = *it;
    if (entry == query) {
      break;
    }
    entry->rankDown();
  }
}

void WifiCredentialsRegistry::moveRankUpAfter(WifiCredentials *query) {
  bool found = false;
  for (std::vector<WifiCredentials *>::iterator it = registry.begin();
       it != registry.end(); it++) {
    WifiCredentials *entry = *it;
    if (entry == query) {
      found = true;
      continue;
    }
    if (found) {
      entry->rankUp();
    }
  }
}

void WifiCredentialsRegistry::rearrange() {
  std::sort(registry.begin(), registry.end(), WifiCredentials::compareByRank);
  while (registry.size() > maxSize) {
    WifiCredentials *last = registry.back();
    registry.pop_back();
    delete (last);
  }
  rewind();
}

WifiCredentials *const WifiCredentialsRegistry::next() {
  WifiCredentials *result = nullptr;
  if (hasNext()) {
    result = *registryIterator;
    registryIterator++;
  }
  return result;
}

WifiCredentialsRegistry *
WifiCredentialsRegistry::put(WifiCredentials *const credentials) {
  WifiCredentials *entry = find(credentials);
  if (nullptr == entry) {
    // insert
    entry = new WifiCredentials(*credentials);
    registry.push_back(entry);

  } else {
    // update
    entry->setKeyType(credentials->getKeyType());
    entry->setKey(credentials->getKey());
  }
  entry->rankFirst();
  moveRankDownExcept(entry);
  rearrange();
  return this;
}

WifiCredentialsRegistry *
WifiCredentialsRegistry::remove(WifiCredentials *const credentials) {
  WifiCredentials *entry;
  std::vector<WifiCredentials *>::iterator it = registry.begin();
  while (it != registry.end()) {
    entry = *it;
    if (entry->isSameSsid(credentials)) {
      moveRankUpAfter(entry);
      registry.erase(it);
      delete (entry);
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
    moveRankDownUntil(entry);
    entry->rankFirst();
    rearrange();
  }
  return this;
}
