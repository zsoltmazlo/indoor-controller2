/* Includes ------------------------------------------------------------------*/
#include "spark_wiring_version.h"
#include "spark_wiring_system.h"
#include "spark_wiring_startup.h"
#include "system_mode.h"

#include "debug.h"

PRODUCT_ID(PLATFORM_ID);
PRODUCT_VERSION(3);

SYSTEM_THREAD(ENABLED);

SYSTEM_MODE(MANUAL);

void setup() {
    signaling::set_state(signaling::INIT);
    signaling::start_thread(4);
    delay(2000);
}

void loop() {
    signaling::set_state(signaling::IDLE);
    delay(2000);
}