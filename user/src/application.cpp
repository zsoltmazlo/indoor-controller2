/* Includes ------------------------------------------------------------------*/
#include "spark_wiring_version.h"
#include "spark_wiring_system.h"
#include "spark_wiring_startup.h"
#include "system_mode.h"

#include "debug.h"

#include "display/Adafruit_GFX.h"
#include "display/Adafruit_ST7735.h"
#include "include/wiced_utilities.h"
#include "display/Display.h"

PRODUCT_ID(PLATFORM_ID);
PRODUCT_VERSION(3);

SYSTEM_THREAD(ENABLED);

SYSTEM_MODE(MANUAL);

#define TFT_CS      A2
#define TFT_RST     D6
#define TFT_DC      D5
#define POTMETER    A1
#define LED_STRIP   D1

Display display = Display(TFT_CS, TFT_DC, TFT_RST);

uint8_t width, volume = 1;
uint16_t potm_prev = 0;

void application_task_worker(void) {

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
        uint16_t potm = analogRead(POTMETER);

        if (abs((potm_prev - potm)) > 20) {
            width = map(potm, 0, 4080, 0, 100);
            analogWrite(LED_STRIP, potm>>4);
        }
        potm_prev = potm;

        display.setLedPulseWidth(width);
        display.setHifiVolume(volume);
        display.setConnectedHosts(10);

        delay(40);
    }
}

void setup() {
    pinMode(LED_STRIP, OUTPUT);
    signaling::set_state(signaling::INIT);
    signaling::start_thread(4);
    display.init();

    // create application task for setting highest priority for it
    Thread("application", &application_task_worker, 9);
}

void loop() {
    signaling::set_state(signaling::IDLE);
    delay(2000);
}