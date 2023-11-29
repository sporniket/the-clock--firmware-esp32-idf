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
#ifndef WIFI_STATION_TYPES_HPP
#define WIFI_STATION_TYPES_HPP

// standard includes
#include <cstdint>

// esp32 includes

// project includes

// clang-format off
/**
 * @brief Model of a lifecycle of a wifi station.
 * 
 * ```mermaid
  flowchart TD
    CREATED -->|add wifi credential dao and listeners| READY_TO_INIT
    READY_TO_INIT -->|reload wifi credentials| READY_TO_INSTALL
    READY_TO_INSTALL --> |Install event handlers| INSTALLED
    INSTALLED --> |Has known access points| TRYING_KNOWN_ACCESS_POINTS
    INSTALLED --> |Has no known access points| TRYING_WPS
    TRYING_KNOWN_ACCESS_POINTS --> |No more known access points| DONE_TRYING_KNOWN_ACCESS_POINTS
    TRYING_KNOWN_ACCESS_POINTS --> |Got connection| CONNECTED
    DONE_TRYING_KNOWN_ACCESS_POINTS --> NOT_CONNECTED_AND_IDLE
    TRYING_WPS --> |Got connection| CONNECTED
    TRYING_WPS --> |No success, can retry| TRYING_WPS
    TRYING_WPS --> |No success, no more retry| NOT_CONNECTED_AND_IDLE
    NOT_CONNECTED_AND_IDLE --> |Relauch connection process| TRYING_WPS
    CONNECTED --> |Lost connection| NOT_CONNECTED_AND_IDLE
 * ```
 */
enum WifiStationLifecycleState {
  // clang-format on
  /**
   * @brief The instance has just been created.
   */
  CREATED,
  /**
   * @brief The instance is ready to be initialized.
   */
  READY_TO_INIT,
  /**
   * @brief The known access points credentials are reloaded and ready to use,
   * ready to install the event handlers.
   */
  READY_TO_INSTALL,
  /**
   * @brief The event handlers are installed, ready to connect.
   */
  INSTALLED,
  /**
   * @brief In the process of trying each known access point to get connected.
   */
  TRYING_KNOWN_ACCESS_POINTS,
  /**
   * @brief Tried each known access point, without success.
   */
  DONE_TRYING_KNOWN_ACCESS_POINTS,
  /**
   * @brief Trying to connect through wps.
   */
  TRYING_WPS,
  /**
   * @brief Ready to start, or nothing succeeded, waiting to be told what to do.
   */
  NOT_CONNECTED_AND_IDLE,
  /**
   * @brief Connected to an access point.
   */
  CONNECTED
};

/**
 * @brief Interface of a class that manages the lifecycle of a wifi station.
 * 
 */
class WifiStationLifecycleManager {
public:
  /**
   * @brief Try to change the current state to the target state.
   *
   * @return true when the state has been changed.
   */
  virtual bool changeStateToReadyToInit() = 0;

  /**
   * @brief Try to change the current state to the target state.
   *
   * @return true when the state has been changed.
   */
  virtual bool changeStateToReadyToInstall() = 0;

  /**
   * @brief Try to change the current state to the target state.
   *
   * @return true when the state has been changed.
   */
  virtual bool changeStateToInstalled() = 0;

  /**
   * @brief Try to change the current state to the target state.
   *
   * @return true when the state has been changed.
   */
  virtual bool changeStateToTryingKnownAccessPoints() = 0;

  /**
   * @brief Try to change the current state to the target state.
   *
   * @return true when the state has been changed.
   */
  virtual bool changeStateToDoneTryingKnownAccessPoints() = 0;

  /**
   * @brief Try to change the current state to the target state.
   *
   * @return true when the state has been changed.
   */
  virtual bool changeStateToTryingWps() = 0;

  /**
   * @brief Try to change the current state to the target state.
   *
   * @return true when the state has been changed.
   */
  virtual bool changeStateToNotConnectedAndIdle() = 0;

  /**
   * @brief Try to change the current state to the target state.
   *
   * @return true when the state has been changed.
   */
  virtual bool changeStateToConnected() = 0;
};

#endif
