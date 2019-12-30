#define M5STACK_MPU6886
#include <HTTPClient.h>
#include <M5Stack.h>
#include <WiFi.h>
#include <ArduinoJson.h>
#include <Wire.h> // include I2C library
#include <i2c_touch_sensor.h>
#include <MPR121.h>
​
// vibrator function variable
float accX = 0.0F;
float accY = 0.0F;
float accZ = 0.0F;
int MoPin = 26;  // vibrator Grove connected to digital pin 26
​
// touch sensor function variable
// include our Grove I2C touch sensor library
// initialize the Grove I2C touch sensor
// IMPORTANT: in this case, INT pin was connected to pin7 of the Arduino 
// (this is the interrupt pin)
i2ctouchsensor touchsensor; // keep track of 4 pads' states
long previousMillis = 0;
long interval = 100;
​
// WiFi
const char* WIFI_SSID = "WIFI_SSID"; 
const char* WIFI_PASSWROD= "WIFI_PASSWROD";
​
// WEB API 
const String citycode = "1252408";
const String baseurl = "http://api.openweathermap.org/data/2.5/forecast";
const String apikey = "apikey";
​
const String sunny = "Clear";
const String cloudy = "Clouds";
const String rainy = "Rain";
const char* imgNameArr[] = {"/title_01.jpg", "/answer_01.jpg", "/answer_02.jpg", "/answer_03.jpg", "/answer_04.jpg", "/answer_05.jpg", "/answer_06.jpg", "/answer_07.jpg", "/answer_08.jpg", "/answer_09.jpg",
"/answer_10.jpg", "/answer_11.jpg", "/answer_12.jpg", "/answer_13.jpg", "/answer_14.jpg", "/answer_15.jpg", "/answer_16.jpg", "/answer_17.jpg", "/answer_18.jpg", "/answer_19.jpg", "/answer_20.jpg", "/title_02.jpg"};
bool isDone = false;
​
​
void updateWeather() {
  HTTPClient http;
  String url = baseurl + "?id=" + citycode + "&APPID=" + apikey;
//  http.begin(url);
//  int httpCode = http.GET();
//  if (httpCode == HTTP_CODE_OK) {
   if (true) {
    String payload = http.getString();
    payload.replace("\\", "¥");
    DynamicJsonDocument doc(15000);
    deserializeJson(doc, payload);
    JsonVariant tomorrow = doc["list"][0];
    String weather = tomorrow["weather"][0]["main"];
    if (true) {
       M5.Lcd.drawJpgFile(SD, imgNameArr[random(1, 20)], 0, 0);
    }
//    if (weather == sunny) {
//      M5.Lcd.drawJpgFile(SD, imgNameArr[random(1, 6)], 0, 0);
//    } else if (weather == cloudy) {
//      M5.Lcd.drawJpgFile(SD, imgNameArr[random(7, 15)], 0, 0);
//    } else if (weather == rainy) {
//      M5.Lcd.drawJpgFile(SD, imgNameArr[random(16, 20)], 0, 0);
//    }
    isDone = true;
  } else {
     M5.Lcd.print("WebAPI NG!");
  }
}
​
void shakeStart() {
  
  M5.Lcd.drawJpgFile(SD, imgNameArr[21], 0, 0);
  isDone = false;
}
​
bool shakeCheck() {
  
  while(true) {
    M5.IMU.getAccelData(&accX,&accY,&accZ);
    
    if ( abs(accX) > 1.2 && abs(accY) > 1.2 && abs(accZ) > 1.2 ) {
        return true;
    }
    if ( abs(accX) > 0.5 && abs(accY) > 0.5 && abs(accZ) > 0.5 ) {
      digitalWrite(MoPin, HIGH);
    }
    else {
      digitalWrite(MoPin, LOW);
    }
    delay(1);
  }
}
​
void setup() {
  
  // Initialize the M5Stack object
  M5.begin();
   /*
    Power chip connected to gpio21, gpio22, I2C device
    Set battery charging voltage and current
    If used battery, please call this function in your project
  */
  M5.Power.begin();
  Wire.begin(); // needed by the GroveMultiTouch lib  
  touchsensor.initialize(); // initialize the feelers     // initialize the containers
  
  // Initialize IMU
  M5.IMU.Init();
  pinMode( MoPin, OUTPUT );
  M5.Lcd.fillScreen(BLACK);
  M5.Lcd.setTextColor(GREEN , BLACK);
  M5.Lcd.setTextSize(2);
  
  // Display title image
  M5.Lcd.drawJpgFile(SD, imgNameArr[0], 0, 0);
  // WiFi Setup
  M5.Lcd.print("Connecting to WiFi");
  WiFi.mode(WIFI_STA);
  WiFi.disconnect();
//  WiFi.begin(WIFI_SSID, WIFI_PASSWROD);
//  while (WiFi.status() != WL_CONNECTED) {
//    delay(500);
//    M5.Lcd.print('.');
//  }
  delay(500);//WIFI OFF
  M5.Lcd.println("...Connected!");
  delay(2000);
  
  // Shake Start
  shakeStart();
}
​
bool isTouch() {
   unsigned char MPR_Query=0;
   unsigned long currentMillis = millis();
 if(currentMillis - previousMillis > interval)
  {
    previousMillis = currentMillis;
    touchsensor.getTouchState();
  }
 for (int i=0;i<5;i++)
 {
 if (touchsensor.touched&(1<<i))
  {
    return true;
  }
  
 }
}
​
void loop() {
  // update button state
  M5.update();
  
  if (!isDone && shakeCheck()) {
    updateWeather();
    digitalWrite(MoPin, LOW);
  }
  if (isDone && (M5.BtnB.wasReleased() || isTouch())) {
    shakeStart();
  }
  
}