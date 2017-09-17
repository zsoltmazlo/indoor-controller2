///* Includes ------------------------------------------------------------------*/
//#include "spark_wiring_version.h"
//#include "spark_wiring_system.h"
//#include "spark_wiring_startup.h"
//#include "system_mode.h"
//#include "spark_wiring_wifi.h"
//#include "spark_wiring_rgb.h"
//#include "spark_wiring.h"
//#include "spark_wiring_usbserial.h"
//
//PRODUCT_ID(PLATFORM_ID);
//PRODUCT_VERSION(3);
//SYSTEM_THREAD(ENABLED);
//SYSTEM_MODE(MANUAL);
//
////STARTUP(WiFi.selectAntenna(ANT_EXTERNAL));
////STARTUP(spark::WiFi.selectAntenna(ANT_INTERNAL));
//
//void setup() {
//    //    g = 0;
//    //    RGB.control(true);
//    //    RGB.brightness(0xFF);
//    //    RGB.color(0, 0xFF, 0);
//    //    spark::WiFi.on();
//    //    spark::WiFi.setCredentials("lsmx49");
//    //    spark::WiFi.connect();
//    pinMode(DAC1, OUTPUT);
//    pinMode(D0, INPUT);
//    analogWriteResolution(DAC1, 12); // sets analogWrite resolution to 12 bits
//    
//    // on D0 we will send a pwn signal with 51/255=20% duty cycle with 20kHz update frequency
////    analogWrite(D0, 51, 20000);
//    
//    Serial.begin(9600);
//
//}
//
//bool increment = true;
//double x = 49;
//
//// 1 mm is 30.56, 5mm: 152.79
//
//void loop() {
//    
//    // wait while D0 is low
//    while( digitalRead(D0) );
//
//    // 0..4095 means 0..3V3 thus we can increment with 16 which will mean 12mV
//    x += 152.79 * (increment ? 1 : -1);
//    if( x > 3100.0 ) {
//        increment = false;
//        delay(1000);
//    }
//    if( x <= 50.0 ) {
//        increment = true;
//        delay(1000);
//    }
////    analogWrite(DAC1, x);
////    Serial.println(x);
////    Serial.println((uint32_t)x);
//    analogWrite(DAC1, (uint32_t)x);
//    delay(300); // "debouncer"
////    int freq = analogWriteMaxFrequency(DAC1);
////    Serial.println(freq);
////    delay(1000);
//
//
//    //    g ^= 0xFF;
//    //    RGB.color(0xFF, g, 0);
//    //    RGB.control(true);
//    //    delay(2000);
//    //    System.sleep(0, InterruptMode::RISING, 5);
//}