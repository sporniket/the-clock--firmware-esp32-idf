#ifndef WIFI_CREDENTIALS_REGISTRY_HPP
#define WIFI_CREDENTIALS_REGISTRY_HPP

// standard includes
#include <cstdint>
#include <vector>

// esp32 includes

// project includes
#include "WifiSimplistTypes.hpp"

/** @brief Maintains a list of wifi credentials, sorted by a preference rank.
 * The preference criterion is "latest known good (connected to) network". The
 * registry can only enumerate the entries following the ranking order.
 */
class WifiCredentialsRegistry {
private:
  static const uint8_t DEFAULT_MAX_SIZE = 3;
  /**
   * @brief the internal storage of the registry.
   */
  std::vector<WifiCredentials *> registry;

  std::vector<WifiCredentials *>::iterator registryIterator;
  /**
   * @brief The registry will have a limited size, the default value will be
   * `DEFAULT_MAX_SIZE`.
   *
   */
  uint8_t maxSize;
  // Utilities
  /**
   * @brief Find an entry matching the query (same SSID) and return it.
   *
   * @param query the entry to find.
   * @return WifiCredentials* if there is no entry, returns `nullptr`.
   */
  WifiCredentials *find(WifiCredentials *query);
  /**
   * @brief Rank of each entry except the provided one is incremented.
   *
   * @param query the entry to keep untouched.
   */
  void moveRankUpExcept(WifiCredentials *query);
  /**
   * @brief Rank of each entry from the first to the provided one (excluded) is incremented.
   * 
   * @param query the entry from which to stop.
   */
  void moveRankUpUntil(WifiCredentials *query);
  /**
   * @brief Rank of each entry starting from the provided one (excluded) is decremented.
   * 
   * @param query 
   */
  void moveRankDownAfter(WifiCredentials *query);
  /**
   * @brief Sort the entries by rank, and regenerate the enumeration.
   */
  void rearrange() ;
  /**
   * @brief Comparator to sort the entries by rank.
   * 
   * @param e1 the left hand side
   * @param e2 the right hand side.
   * @return true if the left hand side is before the right hand side.
   */
  static bool compareEntriesByRank(WifiCredentials* e1, WifiCredentials* e2) {
    return e1->rank < e2->rank ;
  }

public:
  WifiCredentialsRegistry(
      uint8_t size = WifiCredentialsRegistry::DEFAULT_MAX_SIZE)
      : maxSize(size) {}
  virtual ~WifiCredentialsRegistry();

  // enumeration interface.
  /**
   * @brief Enumeration interface -- go back to the begining of the sequence.
   *
   * @return WifiCredentialsRegistry* this registry, to be able to chain with
   * another call.
   */
  WifiCredentialsRegistry *rewind() {
    registryIterator = registry.begin();
    return this;
  }
  /**
   * @brief Enumeration interface -- ask whether there will be another element.
   *
   * @return true when call to `next()` will not return `null`
   */
  bool hasNext() { return registryIterator != registry.end(); }
  /**
   * @brief Enumeration interface -- get the next element.
   *
   * @return WifiCredentials* the next element, or `nullptr` if the enumeration
   * is finished.
   */
  WifiCredentials *const next() {
    WifiCredentials *result = *registryIterator;
    registryIterator++;
    return result;
  }
  // update interface
  /**
   * @brief Registers new credentials, or an update of existing credentials
   * (same SSID). If the registry has more entries than the maximum size, the
   * entry with the highest rank is removed. The rank of the new/updated entry
   * will be 0 (first).
   *
   * @param credentials the credentials to register, will be deep-cloned.
   * @return WifiCredentialsRegistry* this registry, to be able to chain with
   * another call.
   */
  WifiCredentialsRegistry *put(WifiCredentials *const credentials);
  /**
   * @brief Remove the credentials (same SSID) from the registry.
   *
   * @param credentials the credentials to remove.
   * @return WifiCredentialsRegistry* this registry, to be able to chain with
   * another call.
   */
  WifiCredentialsRegistry *remove(WifiCredentials *const credentials);
  /**
   * @brief The given credentials, if found in the registry (same SSID) becomes
   * the first to be given after a call to `rewind`.
   *
   * @param credentials the credential to reprioritize.
   * @return WifiCredentialsRegistry*
   */
  WifiCredentialsRegistry *setPreferred(WifiCredentials *const credentials);
};

#endif