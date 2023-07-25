
// header include
#include "NetworkTimeKeeperEsp32.hpp"

NetworkTimeKeeperEsp32::~NetworkTimeKeeperEsp32() {}
// write code here...

bool doSomething(HostConfigurationDescription *configuration) {
    return (nullptr != configuration) ;
}

void NetworkTimeKeeperEsp32::onGotConfiguration(HostConfigurationDescription *configuration) {
    doSomething(configuration);
}

void NetworkTimeKeeperEsp32::onLostConfiguration() {}