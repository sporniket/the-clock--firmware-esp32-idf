// Copyright 2023 David SPORN
// ---
// This file is part of 'the clock by sporniket -- ESP32/IDF firmware'.
// ---
// 'ESP32/IDF workspace for IIC' is free software: you can redistribute it
// and/or modify it under the terms of the GNU General Public License as
// published by the Free Software Foundation, either version 3 of the License,
// or (at your option) any later version.

// 'ESP32/IDF workspace for IIC' is distributed in the hope that it will be
// useful, but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General
// Public License for more details.

// You should have received a copy of the GNU General Public License along
// with 'ESP32/IDF workspace for IIC'. If not, see
// <https://www.gnu.org/licenses/>. 

// esp32 includes
#include "esp_log.h"
#include "nvs.h"
#include "nvs_flash.h"

// sdk config
#include "sdkconfig.h"

// project includes
// -- tasks and gpios
#include "DigitalInputOutputEsp32.hpp"
#include "FeedbackLed.hpp"
#include "GeneralPurposeInputOutput.hpp"
#include "InputButton.hpp"
#include "Task.h"
// -- wifi
#include "WifiHelperEsp32.hpp"
#include "WifiSimplist.hpp"
// -- timekeepers
#include "NetworkTimeKeeperEsp32.hpp"

extern "C" {
void app_main(void);
}

//====================================================================
// GPIO pins affectation from configuration
#define PIN_BUTTON_MENU gpio_num_t(CONFIG_PIN_BUTTON_MENU)
#define PIN_STATUS_MAIN gpio_num_t(CONFIG_PIN_STATUS_MAIN)

static constexpr char *TAG = (char *)"the-clock";
static constexpr char *NAME_STORAGE_WIFI = (char *)"tclk_wcreg";

class LoggerHostConfigurationEventListener
    : public HostConfigurationEventListener {
public:
  virtual void onGotConfiguration(HostConfigurationDescription *configuration) {
    ESP_LOGI(TAG, "LoggerHostConfigurationEventListener got ip...");
  }

  /**
   * @brief Previously received configuration is now invalid (destroyed).
   *
   */
  virtual void onLostConfiguration() {
    ESP_LOGI(TAG, "LoggerHostConfigurationEventListener lost ip...");
  }
};

// Sample task : led updater
class LedUpdaterTask : public Task {
private:
  GeneralPurposeInputOutput *gpio;
  FeedbackLed *led;

public:
  LedUpdaterTask *withGpio(GeneralPurposeInputOutput *gpio) {
    this->gpio = gpio;
    return this;
  }
  LedUpdaterTask *withLed(FeedbackLed *led) {
    this->led = led;
    return this;
  }
  void run(void *data) {
    const TickType_t SLEEP_TIME = 100 / portTICK_PERIOD_MS; // 10 Hz
    while (true) {
      gpio->getDigital()->write(CONFIG_PIN_STATUS_MAIN, led->next());
      vTaskDelay(SLEEP_TIME);
    }
  }
};

// Sample task : button watcher
class ButtonWatcherTask : public Task, public InputButtonListener {
private:
  GeneralPurposeInputOutput *gpio;
  InputButton *button;
  FeedbackLed *led;

public:
  ButtonWatcherTask *withGpio(GeneralPurposeInputOutput *gpio) {
    this->gpio = gpio;
    return this;
  }
  ButtonWatcherTask *withButton(InputButton *button) {
    this->button = button;
    button->withListener(this); // so that onInputButtonEvent(...) is called.
    return this;
  }
  ButtonWatcherTask *withLed(FeedbackLed *led) {
    this->led = led;
    return this;
  }
  void run(void *data) {
    const TickType_t SLEEP_TIME = 20 / portTICK_PERIOD_MS; // 50 Hz
    while (true) {
      button->update(gpio->getDigital()->read(CONFIG_PIN_BUTTON_MENU));
      vTaskDelay(SLEEP_TIME);
    }
  }
  void onInputButtonEvent(InputButtonEvent *event) {
    if (!isStarted())
      return; // no connection
    if (STATE_CHANGE == event->type) {
      switch (event->source->getId()) {
      case CONFIG_PIN_BUTTON_MENU:
        if (event->source->isHigh()) {
          led->setFeedbackSequenceAndLoop(BLINK_THRICE);
        } else {
          led->setFeedbackSequenceAndLoop(BLINK_ONCE);
        }
        break;
      }
    }
  }
};

// global instances
// -- tasks and gpios
GeneralPurposeInputOutput *gpio;
Task *ledUpdater;
Task *buttonWatcher;
Task *displayUpdater;
InputButton *button;
FeedbackLed *mainLed;
// -- wifi
WifiStationEsp32 *wifiStation;
LoggerHostConfigurationEventListener *listener;
NetworkTimeKeeperEsp32 *networkTimeKeeper;


void app_main(void) {
  // setup
  // -- NVS
  esp_err_t err = nvs_flash_init();
  if (err == ESP_ERR_NVS_NO_FREE_PAGES ||
      err == ESP_ERR_NVS_NEW_VERSION_FOUND) {
    // NVS partition was truncated and needs to be erased
    // Retry nvs_flash_init
    ESP_ERROR_CHECK(nvs_flash_erase());
    err = nvs_flash_init();
  }
  ESP_ERROR_CHECK(err);

  // -- I/O peripherals
  gpio = (new GeneralPurposeInputOutput())
             ->withDigital(new DigitalInputOutputEsp32());
  gpio->getDigital()->setup(CONFIG_PIN_STATUS_MAIN, WRITE);
  gpio->getDigital()->setup(CONFIG_PIN_BUTTON_MENU, READ);

  // -- animated led
  mainLed = new FeedbackLed();
  mainLed->setFeedbackSequenceAndLoop(BLINK_ONCE);

  // -- action button
  button = new InputButton(CONFIG_PIN_BUTTON_MENU);
  button->withDebouncer(DEBOUNCER_TYPICAL);

  // Tasks
  ledUpdater = (new LedUpdaterTask()) //
                   ->withGpio(gpio)   //
                   ->withLed(mainLed);
  ledUpdater->start();

  buttonWatcher = (new ButtonWatcherTask()) //
                      ->withGpio(gpio)      //
                      ->withLed(mainLed)    //
                      ->withButton(button);
  buttonWatcher->start();

  // -- wifi
  listener = new LoggerHostConfigurationEventListener();
  networkTimeKeeper = new NetworkTimeKeeperEsp32() ;
  wifiStation =
      WifiHelperEsp32::setupAndRunStation(NAME_STORAGE_WIFI, listener, networkTimeKeeper);
  // and voila
}