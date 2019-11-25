#define M5STACK_200Q

#include <HTTPClient.h>
#include <M5Stack.h>
#include <WiFi.h>
#include <ArduinoJson.h>

float accX = 0.0F;
float accY = 0.0F;
float accZ = 0.0F;
float gyroX = 0.0F;
float gyroY = 0.0F;
float gyroZ = 0.0F;
float pitch = 0.0F;
float roll  = 0.0F;
float yaw   = 0.0F;
float temp = 0.0F;
int MoPin = 26;  // vibrator Grove connected to digital pin 26


// WiFi
const char* WIFI_SSID = "WIFI_SSID"; 
const char* WIFI_PASSWROD= "WIFI_PASSWROD";

// WEB API 
const String citycode = "1252408";
const String baseurl = "http://api.openweathermap.org/data/2.5/forecast";
const String apikey = "apikey";

const String sunny = "Clear";
const String cloudy = "Clouds";
const String rainy = "Rain";

const char* imgNameArr[] = {"/title.jpg", "/answer_01.jpg", "/answer_02.jpg", "/answer_03.jpg", "/answer_04.jpg", "/answer_05.jpg", "/answer_06.jpg", "/answer_07.jpg", "/answer_08.jpg", "/answer_09.jpg",
"/answer_10.jpg", "/answer_11.jpg", "/answer_12.jpg", "/answer_13.jpg", "/answer_14.jpg", "/answer_15.jpg", "/answer_16.jpg", "/answer_17.jpg", "/answer_18.jpg", "/answer_19.jpg", "/answer_20.jpg"};

unsigned int counter;


void updateWeather() {
  
  HTTPClient http;
  String url = baseurl + "?id=" + citycode + "&APPID=" + apikey;
  http.begin(url);
  int httpCode = http.GET();
  if (httpCode == HTTP_CODE_OK) {
    String payload = http.getString();
    payload.replace("\\", "¥");
    DynamicJsonDocument doc(15000);
    deserializeJson(doc, payload);
    JsonVariant tomorrow = doc["list"][0];
    String weather = tomorrow["weather"][0]["main"];
        
    if (weather == sunny) {
      
      M5.Lcd.drawJpgFile(SD, imgNameArr[random(1, 6)], 0, 0);
    } else if (weather == cloudy) {
      
      M5.Lcd.drawJpgFile(SD, imgNameArr[random(7, 15)], 0, 0);
    } else if (weather == rainy) {
      
      M5.Lcd.drawJpgFile(SD, imgNameArr[random(16, 20)], 0, 0);
    }
    counter++;
  } else {
     M5.Lcd.print("NG!");
  }
}

bool shakeCheck() {
  M5.IMU.getGyroData(&gyroX,&gyroY,&gyroZ);
  M5.IMU.getAccelData(&accX,&accY,&accZ);
  M5.IMU.getAhrsData(&pitch,&roll,&yaw);
  M5.IMU.getTempData(&temp);
        
  if ( abs(accX) > 1.0 && abs(accY) > 1.0 && abs(accZ) > 1.0) {
    return true;
    digitalWrite(MoPin, HIGH);
  }
  else {
    digitalWrite(MoPin, LOW);
  }
  delay(1);
  
}


void setup() {

  // Initialize the M5Stack object
  M5.begin();
   /*
    Power chip connected to gpio21, gpio22, I2C device
    Set battery charging voltage and current
    If used battery, please call this function in your project
  */
  M5.Power.begin();

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
  WiFi.begin(WIFI_SSID, WIFI_PASSWROD);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    M5.Lcd.print('.');
  }
  M5.Lcd.println("...Connected!");
  delay(2000);
  
  M5.Lcd.println("Shake Start !!");
  counter = 0;
}

void loop() {

  if (counter == 0 && shakeCheck()) {
    
    updateWeather();
  }
  
  delay(3000);

}