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

// standard
// include <cstring>

// esp32 includes
#include "driver/i2c.h"
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
// -- Display
#include "SevenSegmentsFont.hpp"
#include "Tm1637IicBridgeEsp32.hpp"
// -- timekeepers
#include "NetworkTimeKeeperEsp32.hpp"

#include "macros_property.hpp"

extern "C" {
void app_main(void);
}

//====================================================================
// -- BRAINSTORMING
enum TimeKeepingEvents {
  POWER_ON_RESET, // on power on, time is unknown ; if there is an RTC -->
                  // READING_RTC
  UNSYNCHRONIZED, // displayed time may be not wrong, but drifting or garbage is
                  // likely
  SYNCHRONIZED,   // when time has been set
  READING_RTC,    // when synchronizing from RTC
  READING_RTC_DONE,     // --> success --> SYNCHONIZED
  READING_RTC_FAIL,     // --> fail --> UNSYNCHRONIZED
  WRITING_RTC,          // when saving synchronized time to RTC
  WRITING_RTC_DONE,     // --> SYNCHRONIZED
  WRITING_RTC_FAIL,     // --> SYNCHRONIZED anyway (require a RTC lifecycle)
  MANUAL_SETTING,       // Time is being adjusted manually
  MANUAL_SETTING_DONE,  // Time has been set
  NETWORK_TIME_PROBING, // start requesting from ntp
  NETWORK_TIME_PROBING_DONE, // start requesting from ntp
  NETWORK_TIME_PROBING_FAIL, // start requesting from ntp
  NETWORK_TIME_DONE,         //
};
//====================================================================

//====================================================================
// GPIO pins affectation from configuration
#define PIN_BUTTON_MENU gpio_num_t(CONFIG_PIN_BUTTON_MENU)
#define PIN_STATUS_MAIN gpio_num_t(CONFIG_PIN_STATUS_MAIN)

static constexpr char *TAG = (char *)"the-clock";
static constexpr char *NAME_STORAGE_WIFI = (char *)"tclk_wcreg";

static constexpr char *GREETINGS_STRING = (char *)CONFIG_LABEL_TITLE;

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

//====================================================================
// --- the clock display
enum DisplayMode { GREETINGS, TIME, CHANGE_HOUR, CHANGE_MINUTES, MENU };

const char FILL_CHAR = 0x20;    // a.k.a. ASCII space character
const uint8_t DOT_BIT = 1 << 7; // To light the separator colon
const uint8_t PHASE_MAX = 4;

// Sample task : display updater
class DisplayUpdaterTask : public Task {
private:
  const uint8_t TTL_GREETINGS = 1;
  Tm1637IicBridgeEsp32 iicBridge = Tm1637IicBridgeEsp32();
  Tm1637Registers displayRegisters;
  int iicPort;
  bool iicReady = false;
  DisplayMode mode = GREETINGS;
  bool nightTimeMode = false;
  /**
   * @brief The display buffer will 4 steps of animation.
   *
   * * No animation when displaying greetings
   * * 2 phases animation when displaying time (blinking the colon separator or
   * not)
   * * 4 phases animation when setting the clock (one field blinking and the
   * blinking colon in quadrature)
   * * 4 phases animation in menu mode (blinking text and the blinking colon in
   * quadrature)
   *
   * All of these animation (or lack of) will be expanded into the 4 steps.
   */
  char buffer[16];
  /**
   * @brief Intermediate buffer to store the instructed change of the content to
   * display.
   *
   * To avoid displaying in the middle of changing.
   */
  char bufferChange[4];
  /**
   * @brief When bufferChange must be applied.
   */
  bool changeToApply = false;
  /**
   * @brief Select mode to apply BEFORE setting new content
   */
  DisplayMode modeToApply = GREETINGS;

  /**
   * @brief The animation phase will be updated every 0.2 seconds.
   */
  uint8_t phase = 0;

  uint8_t ttl = 0;

  SevenSegmentFont *font = (SevenSegmentFont *)&SevenSegmentsFontUsAscii;

  void applyChange() {
    mode = modeToApply;
    // step 1 : blind copy
    char *dst = buffer;
    for (uint8_t i = 0; i < 4; i++) {
      std::memcpy(dst, bufferChange, 4);
      dst += 4;
    }
  }

  void applyChange2() {
    mode = modeToApply;
    // step 1 : blind copy
    char *dst = buffer;
    // step 2 : clear where it needs (2 phase over 4)
    dst = buffer + 8;
    if (MENU == mode || CHANGE_HOUR == mode) {
      dst[0] = FILL_CHAR;
      dst[1] = FILL_CHAR;
      dst[4] = FILL_CHAR;
      dst[5] = FILL_CHAR;
    }
    if (MENU == mode || CHANGE_MINUTES == mode) {
      dst[2] = FILL_CHAR;
      dst[3] = FILL_CHAR;
      dst[6] = FILL_CHAR;
      dst[7] = FILL_CHAR;
    }
  }

public:
  DisplayUpdaterTask() {
    for (uint8_t i = 0; i < 16; i++) {
      buffer[i] = FILL_CHAR;
    }
  }
  virtual ~DisplayUpdaterTask() {}

  void run(void *data) {
    const TickType_t SLEEP_TIME = 250 / portTICK_PERIOD_MS; // 4 Hz

    while (true) {
      if (iicReady) {
        if (ttl > 0) {
          --ttl;
        }

        // commit pending changes
        if (0 == ttl && changeToApply) {
          applyChange();
          changeToApply = false;
          switch (mode) {
          case GREETINGS:
            ttl = TTL_GREETINGS;
            break;
          case TIME:
            break;
          case CHANGE_HOUR:
            break;
          case CHANGE_MINUTES:
            break;
          case MENU:
            break;
          }
        }

        // animation management
        ++phase;
        if (phase >= PHASE_MAX) {
          phase = 0;
        }

        // update display
        displayRegisters.control.brightness = nightTimeMode ? 1 : 7;

        uint8_t buffer_start = phase * 4;
        displayRegisters.data.digits[0] =
            font->glyphData[(uint8_t)buffer[buffer_start]];
        if (phase - 1 < 2) {
          displayRegisters.data.digits[1] =
              font->glyphData[(uint8_t)buffer[buffer_start + 1]] | DOT_BIT;
        } else {
          displayRegisters.data.digits[1] =
              font->glyphData[(uint8_t)buffer[buffer_start + 1]];
        }
        displayRegisters.data.digits[2] =
            font->glyphData[(uint8_t)buffer[buffer_start + 2]];
        displayRegisters.data.digits[3] =
            font->glyphData[(uint8_t)buffer[buffer_start + 3]];

        iicBridge.upload(&displayRegisters, iicPort);
      }
      vTaskDelay(SLEEP_TIME);
    }
  }

  // external updaters
  void scheduleContent(char *source) {
    bool endOfSource = false;
    for (uint8_t i = 0; i < 4; i++) {
      char val = source[i];
      if (val == 0)
        endOfSource = true;
      bufferChange[i] = endOfSource ? FILL_CHAR : val;
    }
    changeToApply = true;
  }

  /**
   * @brief the next content change will also change to this mode.
   *
   * @param mode of display.
   */
  void scheduleModeChange(DisplayMode mode) { modeToApply = mode; }

  /**
   * @brief The updater cannot queue the changes, so it is usually best to try
   * again later.
   *
   * @return true when there is a change that has not be applied yet.
   */
  bool isBusy() { return ttl > 0 || changeToApply || !iicReady; }
  DisplayMode getMode() { return mode; }
  void setNightTime(bool value) { nightTimeMode = value; }

  // ----- setup iic
  void setupIic(uint8_t iicPort, const i2c_config_t *conf) {
    this->iicPort = iicPort;
    i2c_port_t port = static_cast<i2c_port_t>(this->iicPort);
    i2c_param_config(port, conf);

    ESP_ERROR_CHECK(i2c_set_data_mode((i2c_port_t)port, I2C_DATA_MODE_LSB_FIRST,
                                      I2C_DATA_MODE_LSB_FIRST));

    ESP_ERROR_CHECK(i2c_driver_install(port, conf->mode, 0, 0, 0));

    displayRegisters.control.switchedOn = true;
    displayRegisters.control.brightness = 7;
    displayRegisters.data.size = 4;

    this->iicReady = true;
  }
};

//====================================================================
// --- the clock command interface
/**
 * @brief The interface implemented by the clock, relative to buttons message.
 *
 * @note For each button, the clock will acknowledge two types of messages :
 * @note - A normal click, that happens when a buttons is transitioning from
 * high to low ;
 * @note - A long click, when the button is maintained push for a long enough
 * time
 * @note ---
 * @note For menu and back buttons, the long click will happen only once, until
 * the button is released.
 * @note ---
 * @note For up and down buttons, the long click will happen regularly, until
 * the button is released.
 *
 */
class TheClockCommandListener {
public:
  virtual void onMenuClick() = 0;

  virtual void onMenuLongClick() = 0;

  virtual void onBackClick() = 0;

  virtual void onBackLongClick() = 0;

  virtual void onUpClick() = 0;

  virtual void onUpLongClick() = 0;

  virtual void onDownClick() = 0;

  virtual void onDownLongClick() = 0;
};

// --- the clock main loop
class TheClockTask : public Task, public TheClockCommandListener {
  PROPERTY(TheClockTask,DisplayUpdaterTask,Display)
  PROPERTY(TheClockTask,WifiStationEsp32,WifiStation)
private:
  // Manage display of greetings
  uint8_t greetingsPosition = 0;
  uint8_t GREETINGS_POSITION_MAX = (uint8_t)strlen(GREETINGS_STRING) - 4;
  uint8_t phaseTime = 0;
  uint8_t PHASE_TIME_MAX =
      5; // wait at least a half seconds before updating time again.

  char timeBuffer[5]; // 4 digits + string terminator
  time_t now = 0;
  struct tm timeinfo = {.tm_sec = 0,
                        .tm_min = 0,
                        .tm_hour = 0,
                        .tm_mday = 0,
                        .tm_mon = 0,
                        .tm_year = 0,
                        .tm_wday = 0,
                        .tm_yday = 0,
                        .tm_isdst = 0};

public:
  virtual ~TheClockTask() {
    setenv("TZ", "CET-1CEST-2,M3.5.0/02:00:00,M10.5.0/03:00:00",
           1); // TODO make it configurable
    tzset();
  }

  void run(void *data) {
    const TickType_t SLEEP_TIME = 100 / portTICK_PERIOD_MS; // 10 Hz

    while (true) {
      if (0 == phaseTime) {
        time(&now);
        localtime_r(&now, &timeinfo);

        // update displayed time buffer
        strftime(timeBuffer, sizeof(timeBuffer), "%H%M", &timeinfo);
      }
      if (hasDisplay()) {
        // processing requiring the display
        myDisplay->setNightTime(timeinfo.tm_hour < 8 || timeinfo.tm_hour >= 20);

        if (!myDisplay->isBusy()) {
          // processing requiring an IDLE display (no pending update)
          switch (myDisplay->getMode()) {
          case GREETINGS:
            greetingsPosition =
                (greetingsPosition + 1) % GREETINGS_POSITION_MAX;
            myDisplay->scheduleContent(GREETINGS_STRING + greetingsPosition);
            break;
          case TIME:
            if (0 == phaseTime) {
              myDisplay->scheduleContent(timeBuffer);
              phaseTime = PHASE_TIME_MAX;
            }
            break;
          case CHANGE_HOUR:
            ESP_LOGI(TAG, "TheClockTask: change hour -- or not");
            break;
          case CHANGE_MINUTES:
            ESP_LOGI(TAG, "TheClockTask: change minutes -- or not");
            break;
          case MENU:
            ESP_LOGI(TAG, "TheClockTask: menu -- or not");
            break;
          default:
            ESP_LOGE(TAG, "TheClockTask: UNKNOWN MODE");
          }
        }
      }
      if (0 == greetingsPosition) {
        myDisplay->scheduleModeChange(TIME);
        phaseTime = 0; // force display of time next cycle
      }

      vTaskDelay(SLEEP_TIME); // do nothing while no display
    }
  }

  // === TheClockCommandListener
  virtual void onMenuClick() { ESP_LOGI(TAG, "TheClockTask: on menu click"); }

  virtual void onMenuLongClick() {
    ESP_LOGI(TAG, "TheClockTask: on menu LONG click");
    if(hasWifiStation()) {
      myWifiStation->forgetKnownAccessPoints();
    }
  }

  virtual void onBackClick() { ESP_LOGI(TAG, "TheClockTask: on back click"); }

  virtual void onBackLongClick() {
    ESP_LOGI(TAG, "TheClockTask: on back LONG click");
  }

  virtual void onUpClick() { ESP_LOGI(TAG, "TheClockTask: on up click"); }

  virtual void onUpLongClick() {
    ESP_LOGI(TAG, "TheClockTask: on up LONG click");
  }

  virtual void onDownClick() { ESP_LOGI(TAG, "TheClockTask: on down click"); }

  virtual void onDownLongClick() {
    ESP_LOGI(TAG, "TheClockTask: on down LONG click");
  }
};

//====================================================================
// Sample task : led updater
class LedUpdaterTask : public Task {
private:
  GeneralPurposeInputOutput *gpio;
  FeedbackLed *led;

public:
  virtual ~LedUpdaterTask() {}
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
      bool nextValue = led->next();
      gpio->getDigital()->write(CONFIG_PIN_STATUS_MAIN,
                                CONFIG_PIN_STATUS_MAIN_INVERTED ? !nextValue
                                                                : nextValue);
      vTaskDelay(SLEEP_TIME);
    }
  }
};

// Sample task : button watcher
class ButtonWatcherTask : public Task, public InputButtonListener {
private:
  GeneralPurposeInputOutput *gpio;
  InputButton *buttonMenu;
  InputButton *buttonBack;
  InputButton *buttonUp;
  InputButton *buttonDown;
  FeedbackLed *led; // TODO move to TheClock
  TheClockCommandListener *theClock;

  // long clicks managements
  uint8_t counterMenu = 0;
  uint8_t counterBack = 0;
  uint8_t counterUp = 0;
  uint8_t counterDown = 0;
  const uint8_t LONG_CLICK_MENU = 100; // 2 seconds
  const uint8_t LONG_CLICK_BACK = 100; // 2 seconds
  const uint8_t LONG_CLICK_UP = 25;    // 0.5 seconds
  const uint8_t LONG_CLICK_DOWN = 25;  // 0.5s seconds

public:
  virtual ~ButtonWatcherTask() {}
  ButtonWatcherTask *withGpio(GeneralPurposeInputOutput *gpio) {
    this->gpio = gpio;
    return this;
  }
  ButtonWatcherTask *withButtonMenu(InputButton *button) {
    this->buttonMenu = button;
    button->withListener(this);
    return this;
  }
  ButtonWatcherTask *withButtonBack(InputButton *button) {
    this->buttonBack = button;
    button->withListener(this);
    return this;
  }
  ButtonWatcherTask *withButtonUp(InputButton *button) {
    this->buttonUp = button;
    button->withListener(this);
    return this;
  }
  ButtonWatcherTask *withButtonDown(InputButton *button) {
    this->buttonDown = button;
    button->withListener(this);
    return this;
  }
  ButtonWatcherTask *withLed(FeedbackLed *led) {
    this->led = led;
    return this;
  }
  ButtonWatcherTask *withTheClock(TheClockCommandListener *theClock) {
    this->theClock = theClock;
    return this;
  }
  void run(void *data) {
    const TickType_t SLEEP_TIME = 20 / portTICK_PERIOD_MS; // 50 Hz
    while (true) {
      // update state
      buttonMenu->update(gpio->getDigital()->read(buttonMenu->getId()));
      buttonBack->update(gpio->getDigital()->read(buttonBack->getId()));
      buttonUp->update(gpio->getDigital()->read(buttonUp->getId()));
      buttonDown->update(gpio->getDigital()->read(buttonDown->getId()));

      // manage long clicks
      if (!buttonMenu->isHigh()) {
        if (LONG_CLICK_MENU >= counterMenu) {
          ++counterMenu;
          if (LONG_CLICK_MENU == counterMenu) {
            theClock->onMenuLongClick();
            // only once until release
          }
        }
      }
      if (!buttonBack->isHigh()) {
        if (LONG_CLICK_BACK >= counterBack) {
          ++counterBack;
          if (LONG_CLICK_BACK == counterBack) {
            theClock->onBackLongClick();
            // only once until release
          }
        }
      }
      if (!buttonUp->isHigh()) {
        ++counterUp;
        if (LONG_CLICK_UP == counterUp) {
          theClock->onUpLongClick();
          counterUp = 0; // repeat
        }
      }
      if (!buttonDown->isHigh()) {
        ++counterDown;
        if (LONG_CLICK_DOWN == counterDown) {
          theClock->onDownLongClick();
          counterDown = 0; // repeat
        }
      }
      vTaskDelay(SLEEP_TIME);
    }
  }
  void onInputButtonEvent(InputButtonEvent *event) {
    if (!isStarted())
      return; // no connection
    if (STATE_CHANGE == event->type) {
      switch (event->source->getId()) {
      case CONFIG_PIN_BUTTON_MENU:
        if (!event->source->isHigh()) {
          theClock->onMenuClick();
        } else {
          counterMenu = 0;
        }
        break;
      case CONFIG_PIN_BUTTON_BACK:
        if (!event->source->isHigh()) {
          theClock->onBackClick();
        } else {
          counterBack = 0;
        }
        break;
      case CONFIG_PIN_BUTTON_UP:
        if (!event->source->isHigh()) {
          theClock->onUpClick();
        } else {
          counterUp = 0;
        }
        break;
      case CONFIG_PIN_BUTTON_DOWN:
        if (!event->source->isHigh()) {
          theClock->onDownClick();
        } else {
          counterDown = 0;
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
ButtonWatcherTask *buttonWatcher;
InputButton *button;
FeedbackLed *mainLed;
DisplayUpdaterTask *displayUpdater;
TheClockTask *theClock;

// -- wifi
WifiStationEsp32 *wifiStation;
LoggerHostConfigurationEventListener *listener;
NetworkTimeKeeperEsp32 *networkTimeKeeper;

// TODO : support configurable button inversion !
InputButton *createButton(uint64_t gpioId) {
  return (new InputButton(gpioId)) //
      ->withDebouncer(DEBOUNCER_TYPICAL);
}

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

  // Tasks
  // -- LED
  ledUpdater = (new LedUpdaterTask()) //
                   ->withGpio(gpio)   //
                   ->withLed(mainLed);
  ledUpdater->start();

  // -- Buttons
  buttonWatcher = (new ButtonWatcherTask())                                  //
                      ->withGpio(gpio)                                       //
                      ->withLed(mainLed)                                     //
                      ->withButtonMenu(createButton(CONFIG_PIN_BUTTON_MENU)) //
                      ->withButtonBack(createButton(CONFIG_PIN_BUTTON_BACK)) //
                      ->withButtonUp(createButton(CONFIG_PIN_BUTTON_UP))     //
                      ->withButtonDown(createButton(CONFIG_PIN_BUTTON_DOWN)) //
      ;
  buttonWatcher->start();

  // -- Seven segment display
  displayUpdater = new DisplayUpdaterTask();

  // -- -- i2c #1
  int i2c_master_port = 0;
  i2c_config_t conf = {
      .mode = I2C_MODE_MASTER,
      .sda_io_num =
          CONFIG_PIN_IIC_1_SDA, // select SDA GPIO specific to your project
      .scl_io_num =
          CONFIG_PIN_IIC_1_SCL, // select SCL GPIO specific to your project
      .sda_pullup_en = GPIO_PULLUP_DISABLE,
      .scl_pullup_en = GPIO_PULLUP_DISABLE,
      .master = {.clk_speed =
                     CONFIG_IIC_1_CLK_FREQ_HZ}, // select frequency specific to
                                                // your project
      .clk_flags = 0, // optional; you can use I2C_SCLK_SRC_FLAG_* flags to
                      // choose i2c source clock here
  };

  displayUpdater->setupIic(i2c_master_port, &conf);
  displayUpdater->start();

  // -- The clock
  theClock = (new TheClockTask())->withDisplay(displayUpdater);
  theClock->start();
  buttonWatcher->withTheClock(theClock);

  // -- wifi
  listener = new LoggerHostConfigurationEventListener();
  networkTimeKeeper = new NetworkTimeKeeperEsp32(CONFIG_SNTP_TIME_SERVER);
  wifiStation = WifiHelperEsp32::setupAndRunStation(NAME_STORAGE_WIFI, listener,
                                                    networkTimeKeeper);
  theClock->withWifiStation(wifiStation);
  // and voila
}