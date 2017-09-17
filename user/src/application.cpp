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
#include "spark_wiring_i2c.h"

#include <mutex>

PRODUCT_ID(PLATFORM_ID)
PRODUCT_VERSION(3)
SYSTEM_THREAD(ENABLED)
SYSTEM_MODE(MANUAL)

STARTUP(WiFi.selectAntenna(ANT_EXTERNAL));

#define FIRMWARE_VERSION "2.0"

#define TFT_CS      D5
#define TFT_RST     D7
#define TFT_DC      D6
//      TFT_SCK     D4
//      TFT_MOSI    D2
#define TFT_BACKL   DAC

#define TFT_BUTTON      TX
#define LED_POTM1       A0
#define LED_POTM2       A1
#define LED_POTM3       A2
#define LED_STRIP1      WKP
#define LED_STRIP2      A4
#define LED_STRIP3      A5

Display display = Display(TFT_CS, TFT_DC, TFT_RST, TFT_BACKL);

connection::ConnectionManager connManager;

#define DISPLAY_KEY_LED1 0
#define DISPLAY_KEY_LED2 1
#define DISPLAY_KEY_LED3 2
#define DISPLAY_KEY_IP 3
#define DISPLAY_KEY_PORT 4
#define DISPLAY_KEY_CH 5
#define DISPLAY_KEY_TEMP 6
#define DISPLAY_KEY_MAC 7
#define DISPLAY_KEY_VERSION 8
#define DISPLAY_KEY_BAUD 9


#define SERIAL_BAUD 9600

std::mutex display_mutex;

void set_led_brightness(uint8_t pin, uint8_t display_key, const utils::Message& data) {
    signaling::set_state(signaling::LED_CHANGED);
    int brightness = data;
    debug::println("MAIN | Setting up LED%d brightness: %u", display_key - DISPLAY_KEY_LED1 + 1, brightness);
    uint8_t width = map(brightness, 0, 100, 0, 255);
    analogWrite(pin, width);

    display_mutex.lock();
    display.updateItem(display_key, brightness);
    display_mutex.unlock();
}

void application_task_worker(void) {

    debug::println("APP | Thread started.");

    uint16_t led1_prev, led2_prev, led3_prev = 0;

    auto potmeter_check = [](
            uint8_t potm_pin,
            uint8_t led_pin,
            uint8_t display_key,
            uint16_t * prev_value) {
        uint16_t potm = analogRead(potm_pin);
        uint8_t width = map(potm, 35, 4080, 0, 100);

        if (abs((*prev_value - width)) > 1) {
            debug::println("APP | led potmeter%d: %d", display_key - DISPLAY_KEY_LED1 + 1, potm);
            set_led_brightness(led_pin, display_key, width);
            *prev_value = width;
        }
    };

    // active low
    pinMode(TFT_BUTTON, INPUT_PULLUP);
    Display::DisplayBlacklight backlight = Display::On;

    pinMode(LED_POTM1, INPUT);
    pinMode(LED_POTM2, INPUT);
    pinMode(LED_POTM3, INPUT);

    while (1) {
        backlight = digitalRead(TFT_BUTTON) == 0 ? Display::On : Display::Off;
        display.setBacklight(backlight);

        // if backlight is off, then disable all led at once
        while (backlight == Display::Off) {
            set_led_brightness(LED_STRIP1, DISPLAY_KEY_LED1, 0);
            set_led_brightness(LED_STRIP2, DISPLAY_KEY_LED2, 0);
            set_led_brightness(LED_STRIP3, DISPLAY_KEY_LED3, 0);
            backlight = digitalRead(TFT_BUTTON) == 0 ? Display::On : Display::Off;
        }


        // measuring potential-meter, and if difference with previous measurement
        // is higher than 20, then set the pulse width too
        // 
        // This means that it will map input voltages between 0 and 3.3 volts
        // into integer values between 0 and 4095. This yields a resolution 
        // between readings of: 3.3 volts / 4096 units or, 0.0008 volts (0.8 mV) 
        // per unit.
        // 
        // therefore the measurement threshold is 40*0.8mV = 32mV
        potmeter_check(LED_POTM1, LED_STRIP1, DISPLAY_KEY_LED1, &led1_prev);
        potmeter_check(LED_POTM2, LED_STRIP2, DISPLAY_KEY_LED2, &led2_prev);
        potmeter_check(LED_POTM3, LED_STRIP3, DISPLAY_KEY_LED3, &led3_prev);
        delay(80);
    }
}

void temperature_read_worker() {

    uint8_t address = 0x40;
    Wire.begin();

    // Heater off, 14 bit Temperature and Humidity Measurement Resolution 
    Wire.beginTransmission(address);
    Wire.write(0x02);
    Wire.write(0x0);
    Wire.write(0x0);
    Wire.endTransmission();

    char _bffr[16];
    display.enableGraph(0, 66, 320, 100);
    uint8_t msb, lsb, measurement_count;
    float temp;
    measurement_count = 0;

    while (1) {

        // read temperature value
        Wire.beginTransmission(address);
        Wire.write(0x00);
        Wire.endTransmission();

        delay(10);
        Wire.requestFrom(address, (uint8_t) 2);

        msb = Wire.read();
        lsb = Wire.read();
        uint16_t raw = msb << 8 | lsb;
        temp = raw / pow(2, 16) * 165 - 40;

        // wait here while the display mutex is locked by other process
        display_mutex.lock();

        // one hour is elapsed after last measurement, show on graph
        if (measurement_count == 0) {
            display.addDataToGraph(temp);
        }

        // 60 * 30 sec = 180 sec (0.5 hour)
        ++measurement_count;
        if (measurement_count == 60) {
            measurement_count = 0;
        }


        sprintf(_bffr, "%.1fC", temp);
        display.updateItem(DISPLAY_KEY_TEMP, _bffr);
        display_mutex.unlock();

        // for 30s this measurement will be fine
        delay(30000);

    }

}

void setup() {
    display.init();
    connManager.init();
    display.setBacklight(Display::On);
    debug::init(SERIAL_BAUD);

    // enable dfu mode on button click
    System.on(button_click, [](system_event_t ev, int param) {
        System.dfu();
    });


    // STAGE ONE: initializing
    display.println("Photon started.");
    display.println("Firmware version : %s", FIRMWARE_VERSION);
    debug::println("MAIN | Photon started.\n\tFirmware version: %s", FIRMWARE_VERSION);

    pinMode(LED_STRIP1, OUTPUT);
    pinMode(LED_STRIP2, OUTPUT);
    pinMode(LED_STRIP3, OUTPUT);
    signaling::set_state(signaling::INIT);
    signaling::start_thread(4);

    // STAGE TWO: establish connection
    display.println("Connecting...");

    connManager.connectToNetwork("lsmx49");

    auto macAddress = connManager.getMACAddress();
    auto ipAddress = connManager.getIpAddress();
    debug::println("MAIN | Connected to network.\n\tIP: %s", ipAddress.c_str());
    debug::println("MAIN | \tMAC: ", macAddress.c_str());
    display.println("MAC .............: %s", macAddress.c_str());
    display.println("IP ..............: %s", ipAddress.c_str());

    //    display.println("Starting server...");
    //    connManager.startTcpServer(3300);
    display.print("Connecting to broker...");
    connManager.connectToBroker("192.168.1.3", 1883);
    display.println("done.");
    debug::println("MAIN | Connected to broker: 192.168.1.3");


    // STAGE THREE: set callbacks
    connManager.addMessageHandler({"led1", std::bind(set_led_brightness, LED_STRIP1, DISPLAY_KEY_LED1, std::placeholders::_1)});
    connManager.addMessageHandler({"led2", std::bind(set_led_brightness, LED_STRIP2, DISPLAY_KEY_LED2, std::placeholders::_1)});
    connManager.addMessageHandler({"led3", std::bind(set_led_brightness, LED_STRIP3, DISPLAY_KEY_LED3, std::placeholders::_1)});

    // STAGE FOUR: create application task for setting highest priority for it
    display.println("Starting app...");
    delay(2000);

    display.clear();

    // Properties order: font size, NL/SL, color, x, y, offset, postfix
    display.addItem(DISPLAY_KEY_LED1, "LED1", ItemProperties{4, ItemProperties::NEW_LINE, ST7735_YELLOW, 12, 186, 0, "%"});
    display.addItem(DISPLAY_KEY_LED2, "LED2", ItemProperties{4, ItemProperties::NEW_LINE, ST7735_YELLOW, 115, 186, 0, "%"});
    display.addItem(DISPLAY_KEY_LED3, "LED3", ItemProperties{4, ItemProperties::NEW_LINE, ST7735_YELLOW, 220, 186, 0, "%"});
    display.addItem(DISPLAY_KEY_TEMP, "Temperature", ItemProperties{4, ItemProperties::NEW_LINE, ST7735_GREEN, 12, 12, 0, ""});
    display.addItem(DISPLAY_KEY_IP, "channel:", ItemProperties{1, ItemProperties::SAME_LINE, COLOR_YELLOW, 160, 12, 6, ""});
    display.addItem(DISPLAY_KEY_PORT, "status :", ItemProperties{1, ItemProperties::SAME_LINE, COLOR_YELLOW, 160, 22, 6, ""});
    display.addItem(DISPLAY_KEY_MAC, "MAC    :", ItemProperties{1, ItemProperties::SAME_LINE, COLOR_BLUE, 160, 32, 6, ""});
    display.addItem(DISPLAY_KEY_VERSION, "version:", ItemProperties{1, ItemProperties::SAME_LINE, COLOR_BLUE, 160, 42, 6, ""});
    display.addItem(DISPLAY_KEY_BAUD, "baud   :", ItemProperties{1, ItemProperties::SAME_LINE, COLOR_BLUE, 160, 52, 6, ""});

    display.updateItem(DISPLAY_KEY_IP, "chiron/ledstrip");
    display.updateItem(DISPLAY_KEY_MAC, macAddress);
    display.updateItem(DISPLAY_KEY_VERSION, FIRMWARE_VERSION);
    display.updateItem(DISPLAY_KEY_BAUD, SERIAL_BAUD);

    Thread("application", &application_task_worker, 9);
    Thread("temperature", &temperature_read_worker, 5);
}

void loop() {
    signaling::set_state(signaling::IDLE);
    delay(2000);

    display_mutex.lock();
    switch (connManager.getConnectionState()) {
        case connection::ConnectionManager::CONNECTED:
            display.updateItem(DISPLAY_KEY_PORT, "connected");
            break;
        case connection::ConnectionManager::DISCONNECTED:
            display.updateItem(DISPLAY_KEY_PORT, "disconnected");
            break;
        case connection::ConnectionManager::CONNECTING:
            display.updateItem(DISPLAY_KEY_PORT, "reconnecting...");
            break;

    }
    display_mutex.unlock();



}