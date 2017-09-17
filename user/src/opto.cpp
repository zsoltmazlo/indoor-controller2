///* Includes ------------------------------------------------------------------*/
//#include "spark_wiring_version.h"
//#include "spark_wiring_system.h"
//#include "spark_wiring_startup.h"
//#include "system_mode.h"
//#include "spark_wiring_wifi.h"
//#include "spark_wiring_rgb.h"
//#include "spark_wiring.h"
//#include "spark_wiring_usbserial.h"
//#include "include/wiced_utilities.h"
//#include <inttypes.h>
//
//PRODUCT_ID(PLATFORM_ID);
//PRODUCT_VERSION(3);
//SYSTEM_THREAD(ENABLED);
//SYSTEM_MODE(MANUAL);
//
////STARTUP(WiFi.selectAntenna(ANT_EXTERNAL));
////STARTUP(spark::WiFi.selectAntenna(ANT_INTERNAL));
//
//constexpr int sensorsDistance = 72; // in mm
//
//enum DetectionState {
//    NO_OBJECT_DETECTED,
//    GATE_OCCUPIED
//};
//
//enum DetectionDirection {
//    UNDEFINED,
//    GATE1_TO_GATE2,
//    GATE2_TO_GATE1
//};
//
//
//DetectionState gate1, gate2;
//DetectionDirection direction;
//uint8_t red, blue;
//
//uint32_t analog_value_reference;
//
///*
// * For each gate, we are saving the first and second moment when the sensor's
// * state is changed
// */
//uint32_t gate1_ts1, gate1_ts2, gate2_ts1, gate2_ts2;
//double ellapsed_time_a, ellapsed_time_b, gate1_trespass, gate2_trespass;
//bool show_measurements = false;
//
//void setup() {
//    Serial.begin(9600);
//    pinMode(A0, INPUT);
//    pinMode(A1, INPUT);
//    pinMode(D7, OUTPUT);
//    digitalWrite(D7, HIGH);
//    RGB.control(true);
//    red = 0;
//    blue = 0;
//    RGB.color(0, 0, 0);
//    RGB.brightness(255, true);
//
//    gate1 = NO_OBJECT_DETECTED;
//    gate2 = NO_OBJECT_DETECTED;
//    direction = UNDEFINED;
//
//    // measure one to adjust sensors' value to environment
//    analog_value_reference = analogRead(A0) - 150;
//}
//
//double event_occured(const char* prefix, uint32_t ticks) {
//    double ms = ((double) ticks) / System.ticksPerMicrosecond() / 1000.0f;
//    Serial.print(prefix);
//    Serial.print(ms);
//    Serial.println("ms");
//    Serial.flush();
//    return ms;
//}
//
//void loop() {
//    bool g1 = analogRead(A0) <= analog_value_reference;
//    bool g2 = analogRead(A1) <= analog_value_reference;
//
//    switch (gate1) {
//        case NO_OBJECT_DETECTED:
//            if (g1) {
//                gate1_ts1 = System.ticks();
//                gate1 = GATE_OCCUPIED;
//
//                if (direction == UNDEFINED && gate2 == NO_OBJECT_DETECTED) {
//                    direction = GATE1_TO_GATE2;
//                }
//
//                if (direction == GATE2_TO_GATE1) {
//                    ellapsed_time_a = event_occured("gate2->gate1 (a): ", gate1_ts1 - gate2_ts1);
//                    Serial.print("velocity: ");
//                    Serial.print(sensorsDistance / ellapsed_time_a);
//                    Serial.println("mm/ms");
//                }
//            }
//            break;
//        case GATE_OCCUPIED:
//            if (!g1) {
//                gate1_ts2 = System.ticks();
//                gate1 = NO_OBJECT_DETECTED;
//                gate1_trespass = event_occured("gate1 trespass: ", gate1_ts2 - gate1_ts1);
//
//                if (direction == GATE2_TO_GATE1) {
//                    ellapsed_time_b = event_occured("gate2->gate1 (b): ", gate1_ts2 - gate2_ts2);
//                    direction = UNDEFINED;
//                    show_measurements = true;
//                }
//            }
//            break;
//    }
//
//    switch (gate2) {
//        case NO_OBJECT_DETECTED:
//            if (g2) {
//                gate2_ts1 = System.ticks();
//                gate2 = GATE_OCCUPIED;
//
//                if (direction == UNDEFINED && gate1 == NO_OBJECT_DETECTED) {
//                    direction = GATE2_TO_GATE1;
//                }
//                if (direction == GATE1_TO_GATE2) {
//                    ellapsed_time_a = event_occured("gate1->gate2 (a): ", gate2_ts1 - gate1_ts1);
//                    Serial.print("velocity: ");
//                    Serial.print(sensorsDistance / ellapsed_time_a);
//                    Serial.println("mm/ms");
//                }
//
//            }
//            break;
//        case GATE_OCCUPIED:
//            if (!g2) {
//                gate2_ts2 = System.ticks();
//                gate2 = NO_OBJECT_DETECTED;
//                gate2_trespass = event_occured("gate2 trespass: ", gate2_ts2 - gate2_ts1);
//
//                if (direction == GATE1_TO_GATE2) {
//                    ellapsed_time_b = event_occured("gate1->gate2 (b): ", gate2_ts2 - gate1_ts2);
//                    direction = UNDEFINED;
//                    show_measurements = true;
//                }
//            }
//            break;
//    }
//
//    if (show_measurements) {
//        show_measurements = false;
//        double vel_a = sensorsDistance / ellapsed_time_a;
//        double vel_b = sensorsDistance / ellapsed_time_b;
//        double len_a = sensorsDistance / ellapsed_time_a*gate1_trespass;
//        double len_b = sensorsDistance / ellapsed_time_b*gate2_trespass;
//        Serial.print("velocity (a): ");
//        Serial.print(vel_a);
//        Serial.print("mm/ms\nlength (a): ");
//        Serial.print(len_a);
//        Serial.print("mm\nvelocity (b): ");
//        Serial.print(vel_b);
//        Serial.print("mm/ms\nlength (b): ");
//        Serial.print(len_b);
//        Serial.print("mm\navg velocity: ");
//        Serial.print((vel_a + vel_b) / 2.0);
//        Serial.print("mm/ms\navg length: ");
//        Serial.print((len_a + len_b) / 2.0);
//        Serial.println("mm\n----");
//    }
//
//    RGB.color(gate1 == GATE_OCCUPIED ? 255 : 0, 0, gate2 == GATE_OCCUPIED ? 255 : 0);
//    //    Serial.print(analogRead(A0));
//    //    Serial.print(" ");
//    //    Serial.println(analogRead(A1));
//}