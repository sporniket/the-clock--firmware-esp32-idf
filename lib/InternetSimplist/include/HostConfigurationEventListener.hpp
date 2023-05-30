// Copyright 2023 David SPORN
// ---
// This file is part of 'Internet Simplist'.
// ---
// 'Internet Simplist' is free software: you can redistribute it and/or 
// modify it under the terms of the GNU General Public License as published 
// by the Free Software Foundation, either version 3 of the License, or 
// (at your option) any later version.

// 'Internet Simplist' is distributed in the hope that it will be useful, 
// but WITHOUT ANY WARRANTY; without even the implied warranty of 
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General 
// Public License for more details.

// You should have received a copy of the GNU General Public License along 
// with 'Internet Simplist'. If not, see <https://www.gnu.org/licenses/>. 
#ifndef HOST_CONFIGURATION_EVENT_LISTENER_HPP
#define HOST_CONFIGURATION_EVENT_LISTENER_HPP

// standard includes
#include <cstdint>

// esp32 includes

// project includes
#include "InternetSimplistTypes.hpp"

/** @brief Interface to implement to react to getting/losing a host configuration.
 * 
 * e.g. when getting a host configuration, accessing a resource through the network.
 */
class HostConfigurationEventListener {
    private:

    public:
        virtual ~HostConfigurationEventListener() ;

        /**
         * @brief Event received when obtaining a host configuration.
         * 
         * @param configuration the configuration (ip address, ...).
         */
        virtual void onGotConfiguration(HostConfigurationDescription* configuration) = 0 ;

        /**
         * @brief Previously received configuration is now invalid (destroyed).
         * 
         */
        virtual void onLostConfiguration() = 0 ;

} ;

#endif