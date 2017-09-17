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
//#include "softap_http.h"
//#include "utils/picojson.h"
//
//PRODUCT_ID(PLATFORM_ID);
//PRODUCT_VERSION(3);
//SYSTEM_THREAD(ENABLED);
//SYSTEM_MODE(MANUAL);
//
////STARTUP(WiFi.selectAntenna(ANT_INTERNAL));
//STARTUP(spark::WiFi.selectAntenna(ANT_INTERNAL));
//
//void myPage(const char* url, ResponseCallback* cb, void* cbArg, Reader* body, Writer* result, void* reserved) {
//    
//    
//    Serial.print("Handling page: ");
//    Serial.println(url);
//    
//    // other pages except sensor are not found
//    if( strcmp(url, "/sensor") != 0 ) {
//        cb(cbArg, 0, 404, nullptr, nullptr);
//        return;
//    }
//    
//    // otherwise
//    Header h("Content-type: application/json");
//    cb(cbArg, 0, 200, "application/json", &h);
//    
//    picojson::object response;
//    response["welcome"] = picojson::value("Hello there!");
//    response["ticks"] = picojson::value(System.ticks()*1.0);
//    response["button"] = picojson::value(digitalRead(D1)==1);
//    result->write(picojson::value(response).serialize(false).c_str());
//}
//
//STARTUP(softap_set_application_page_handler(myPage, nullptr));
//
//void setup() {
//    spark::WiFi.on();
//    pinMode(D1, INPUT);
//    pinMode(D7, OUTPUT);
////    System.set(SYSTEM_CONFIG_SOFTAP_PREFIX, "UniSense");
////    System.set(SYSTEM_CONFIG_SOFTAP_SUFFIX, "M3");
//    Serial.begin(9600);
//
//}
//
//void loop() {
//    digitalWrite(D7, digitalRead(D1));
//    delay(50);
//    
//}