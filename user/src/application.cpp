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
#define LED_POTM    A1
#define HIFI_POTM   A0
#define LED_STRIP   D1

Display display = Display(TFT_CS, TFT_DC, TFT_RST);

connection::ConnectionManager connManager;

#define DISPLAY_KEY_LED 0
#define DISPLAY_KEY_HIFI 1
#define DISPLAY_KEY_IP 2
#define DISPLAY_KEY_PORT 3
#define DISPLAY_KEY_CH 4

void set_led_brightness(const connection::MessageData& data) {
    signaling::set_state(signaling::LED_CHANGED);
    uint8_t brightness = (uint8_t) data.d;
    debug::println("MAIN | Setting up LED brightness: %u", brightness);
    uint8_t width = map(brightness, 0, 100, 0, 255);
    analogWrite(LED_STRIP, width);
    display.updateItem(DISPLAY_KEY_LED, brightness);
}

void set_hifi_volume(const connection::MessageData& data) {
    uint8_t volume = (uint8_t) data.d;
    debug::println("MAIN | Setting up HiFi volume: %u", volume);
    display.updateItem(DISPLAY_KEY_HIFI, volume);
}

void change_channel(const connection::MessageData& data) {
    signaling::set_state(signaling::CHANNEL_CHANGED);
    uint8_t channel = (uint8_t) data.d;
    debug::println("MAIN | Change TV to channel: %d", channel);
    change_to_channel(channel);
    display.updateItem(DISPLAY_KEY_CH, channel);
}

uint8_t hifi_req_volume = 0;

void volume_task_worker(void) {
    uint8_t hifi_current_vol = 0;

    // first, we have to zero the volume down
    for (uint8_t i = 0; i < 30; ++i) {
        send_ir_command(SAMSUNG_VOLU_D);
        delay(300);
    }

    while (1) {

        if (hifi_current_vol != hifi_req_volume) {
            signaling::set_state(signaling::VOLUME);

            if (hifi_current_vol > hifi_req_volume) {
                send_ir_command(SAMSUNG_VOLU_D);
                --hifi_current_vol;
            } else {
                send_ir_command(SAMSUNG_VOLU_U);
                ++hifi_current_vol;
            }
        }

        delay(300);

    }
}

void application_task_worker(void) {

    debug::println("APP | Thread started.");

    uint16_t potm, led_potm_prev = 0, hifi_potm_prev = 0;

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
        potm = analogRead(LED_POTM);

        if (abs((led_potm_prev - potm)) > 20) {
            uint8_t width = map(potm, 35, 4080, 0, 100);
            debug::println("APP | led potmeter: %d", potm);
            analogWrite(LED_STRIP, (uint8_t) (width * 2.55f));
            display.updateItem(DISPLAY_KEY_LED, width);
            debug::println("APP | Setting up LED brightness: %u", width);
        }
        led_potm_prev = potm;

        // now for the HIFI volume too
        potm = analogRead(HIFI_POTM);

        if (abs((hifi_potm_prev - potm)) > 20) {
            // there is a small offset of the potmeter, therefore we have to
            // adjust it
            uint8_t volume = map(potm, 0, 4080, 0, 30);

            if (hifi_req_volume != volume) {
                debug::println("APP | hifi potmeter: %d", potm);
                display.updateItem(DISPLAY_KEY_HIFI, volume);
                debug::println("APP | Setting up HiFi volume: %u", volume);
                hifi_req_volume = volume;
            }

        }

        hifi_potm_prev = potm;
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

    // STAGE TWO: establish connection
    display.print("Connecting...");

    connManager.connectToNetwork();
    display.println("done.");

    connManager.getIpAddress(address);
    display.println("IP:\n   %s", address);
    debug::println("MAIN | Connected to network.\n\tIP: %s", address);

    display.println("Starting server...");

    connManager.startTcpServer(3300);

    // STAGE THREE: set callbacks
    connManager.addMessageHandler({"volume", &set_hifi_volume});
    connManager.addMessageHandler({"led", &set_led_brightness});
    connManager.addMessageHandler({"tvchannel", &change_channel});

    // STAGE FOUR: create application task for setting highest priority for it
    display.println("Starting app...");
    delay(2000);

    display.clear();
    // Properties order: font size, NL/SL, color, x, y, offset, postfix
    display.addItem(DISPLAY_KEY_LED, "LED brightness",{4, ItemProperties::NEW_LINE, ST7735_YELLOW, 3, 0, 20, "%"});
    display.addItem(DISPLAY_KEY_HIFI, "Sound system volume",{4, ItemProperties::NEW_LINE, ST7735_GREEN, 3, 50, 35, " "});
    display.addItem(DISPLAY_KEY_IP, "IP:  ",{1, ItemProperties::SAME_LINE, ST7735_BLUE, 3, 100, 6, " "});
    display.addItem(DISPLAY_KEY_PORT, "port:",{1, ItemProperties::SAME_LINE, ST7735_BLUE, 3, 110, 6, " "});
    display.addItem(DISPLAY_KEY_CH, "Last channel:",{1, ItemProperties::SAME_LINE, ST7735_BLUE, 3, 120, 6, " "});

    display.updateItem(DISPLAY_KEY_IP, address);
    display.updateItem(DISPLAY_KEY_PORT, 3300);
    display.updateItem(DISPLAY_KEY_HIFI, 20);

    Thread("application", &application_task_worker, 9);
    Thread("volume", &volume_task_worker, 5);
}

void loop() {
    signaling::set_state(signaling::IDLE);
    delay(2000);
}