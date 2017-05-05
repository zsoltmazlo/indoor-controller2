/* Includes ------------------------------------------------------------------*/
#include "spark_wiring_version.h"
#include "spark_wiring_system.h"
#include "spark_wiring_startup.h"
#include "system_mode.h"

#include "utils/signaling.h"

#include "display/Adafruit_GFX.h"
#include "display/Adafruit_ST7735.h"
#include "display/Display.h"
#include "utils/debug.h"
#include "connection/ConnectionManager.h"
#include "utils/irtransceiver.h"
#include "spark_wiring_usartserial.h"

PRODUCT_ID(PLATFORM_ID);
PRODUCT_VERSION(3);
SYSTEM_THREAD(ENABLED);
SYSTEM_MODE(MANUAL);

//STARTUP(WiFi.selectAntenna(ANT_EXTERNAL));
STARTUP(WiFi.selectAntenna(ANT_INTERNAL));

#define TFT_CS      A2
#define TFT_RST     D6
#define TFT_DC      D5
#define POTMETER    A1
#define LED_STRIP   D1

Display display = Display(TFT_CS, TFT_DC, TFT_RST);

ConnectionManager connManager;

void set_led_brightness(uint8_t brightness) {
    debug::println("MAIN | Setting up LED brightness: %u", brightness);
    uint8_t width = map(brightness, 0, 100, 0, 255);
    analogWrite(LED_STRIP, width);
    display.setLedPulseWidth(brightness);
}

void set_hifi_volume(uint8_t volume) {
    debug::println("MAIN | Setting up HiFi volume: %u", volume);
    display.setHifiVolume(volume);
}

void change_channel(uint8_t channel) {
    debug::println("MAIN | Change TV to channel: %d", channel);
    change_to_channel(channel);
}

void application_task_worker(void) {

    debug::println("APP | Thread started.");

    uint16_t potm, potm_prev = 0;

    // initialize values
    set_led_brightness(50);
    set_hifi_volume(0);

    while (1) {

        // measuring potential-meter, and if difference with previous measurement
        // is higher than 20, then set the pulse width too
        // 
        // This means that it will map input voltages between 0 and 3.3 volts
        // into integer values between 0 and 4095. This yields a resolution 
        // between readings of: 3.3 volts / 4096 units or, 0.0008 volts (0.8 mV) 
        // per unit.
        // 
        // therefore the measurement threshold is 40*0.8mV = 32mV
        potm = analogRead(POTMETER);

        if (abs((potm_prev - potm)) > 20) {
            uint8_t width = map(potm, 0, 4080, 0, 100);
            analogWrite(LED_STRIP, potm >> 4);
            display.setLedPulseWidth(width);
            debug::println("APP | Setting up LED brightness: %u", width);
        }
        potm_prev = potm;

        delay(40);
    }
}

void setup() {
    display.init();
    connManager.init();
    debug::init(9600);
    Serial1.begin(9600);

    char address[20];
    connManager.getMACAddress(address);


    // STAGE ONE: initializing
    display.println("Photon started.");
    display.println("Firmware version:\n   %d", System.versionNumber());
    display.println("MAC:\n   %s", address);
    debug::println("MAIN | Photon started.\n\tFirmware version: %d\n\t"
            "MAC: %s", System.versionNumber(), address);

    pinMode(LED_STRIP, OUTPUT);
    signaling::set_state(signaling::INIT);
    signaling::start_thread(4);

    display.print("Connecting...");

    connManager.connectToNetwork();
    display.println("done.");

    connManager.getIpAddress(address);
    display.println("IP:\n   %s", address);
    debug::println("MAIN | Connected to network.\n\tIP: %s", address);

    display.println("Starting server...");

    connManager.startTcpServer(3300);

    // set callbacks
    connManager.setChangeChannelCallback(
            std::bind(&change_channel, std::placeholders::_1));
    connManager.setHifiVolumeArrivedCallback(
            std::bind(&set_hifi_volume, std::placeholders::_1));
    connManager.setLedBrightnessArrivedCallback(
            std::bind(&set_led_brightness, std::placeholders::_1));

    // create application task for setting highest priority for it
    display.println("Starting app...");
    delay(2000);
    display.showApplicationUi(address, 3300);
    Thread("application", &application_task_worker, 9);
}

void loop() {
    signaling::set_state(signaling::IDLE);
    delay(2000);
}