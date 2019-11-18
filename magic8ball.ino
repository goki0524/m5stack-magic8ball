#include <HTTPClient.h>
#include <M5Stack.h>
#include <WiFi.h>
#include <ArduinoJson.h>
#include "utility/MPU6886.h"

// WiFi
const char* WIFI_SSID = "WIFI_SSID"; 
const char* WIFI_PASSWROD= "WIFI_PASSWROD";

// WEB API 
const String citycode = "citycode";
const String baseurl = "http://api.openweathermap.org/data/2.5/forecast";
const String apikey = "apikey";

const String sunny = "Clear";
const String cloudy = "Clouds";
const String rainy = "Rain";

// ja
//const String sunnyArr[] = {"確実です。", "決定的にそうです。", "間違いありません。", "はい、確かに。", "当てにしていいです。", "私の知るところでは、イエスです。"};
//const String cloudyArr[] = {"多分きっとそうです。", "幸先良好です。", "はい。", "兆候がいいです。", "答えははっきりしていません。もう一度試して下さい。", "後でまた尋ねて下さい。", "答えは今教えてあげない方がいいと思います。", "今のところ予測はつきません。", "精神を集中してもう一度尋ねて下さい。"};
//const String rainyArr[] = {"当てにしてはいけません。", "私の答えは、ノーです。", "私の資料／情報源によるとダメだそうです。", "幸先はあまり良くありません。", "非常に怪しい／危なっかしいです。"};

// en
//const String sunnyArr[] = {"It is certain", "It is decidedly so", "Without a doubt", "Yes definitely", "You may rely on it", "As I see it, yes"};
//const String cloudyArr[] = {"Most likely", "Outlook good", "Yes", "Signs point to yes", "Reply hazy try again", "Ask again later", "Better not tell you now", "Cannot predict now", "Concentrate and ask again"};
//const String rainyArr[] = {"Don't count on it", "My reply is no", "My sources say no", "Outlook not so good", "Very doubtful"};

const char* imgNameArr[] = {"/title.jpg", "/answer_01.jpg", "/answer_02.jpg", "/answer_03.jpg", "/answer_04.jpg", "/answer_05.jpg", "/answer_06.jpg", "/answer_07.jpg", "/answer_08.jpg", "/answer_09.jpg",
"/answer_10.jpg", "/answer_11.jpg", "/answer_12.jpg", "/answer_13.jpg", "/answer_14.jpg", "/answer_15.jpg", "/answer_16.jpg", "/answer_17.jpg", "/answer_18.jpg", "/answer_19.jpg", "/answer_20.jpg"};

unsigned int counter;

MPU6886 IMU;

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



void setup() {
  // put your setup code here, to run once:
  
  M5.begin();
  M5.Lcd.clear();
  M5.Lcd.setTextSize(2);

  // Font Setup
//  M5.Lcd.setTextWrap(true, true);
  // Font Japanese
//  String fileName = "/Final-Frontier-28.vlw";
//  M5.Lcd.loadFont(fileName, SD);

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
  
  M5.Lcd.fillScreen(0x3186);
  counter = 0;
}

void loop() {
  // put your main code here, to run repeatedly:
  if (counter == 0) {
      updateWeather();
    }
  delay(5000);

}
