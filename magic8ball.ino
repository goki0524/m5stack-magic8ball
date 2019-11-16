#include <HTTPClient.h>
#include <M5Stack.h>
#include <WiFi.h>
#include <ArduinoJson.h>


// WiFi
const char* WIFI_SSID = "ssid";
const char* WIFI_PASSWROD= "password";

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
const String sunnyArr[] = {"It is certain", "It is decidedly so", "Without a doubt", "Yes definitely", "You may rely on it", "As I see it, yes"};
const String cloudyArr[] = {"Most likely", "Outlook good", "Yes", "Signs point to yes", "Reply hazy try again", "Ask again later", "Better not tell you now", "Cannot predict now", "Concentrate and ask again"};
const String rainyArr[] = {"Don't count on it", "My reply is no", "My sources say no", "Outlook not so good", "Very doubtful"};

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

    M5.Lcd.setCursor(80,100);

    if (weather == sunny) {
      M5.Lcd.drawJpgFile(SD, "/sunny.jpg", 0, 80);
      M5.Lcd.print(sunnyArr[random(6)]);
    } else if (weather == cloudy) {
      M5.Lcd.drawJpgFile(SD, "/cloudy.jpg", 0, 80);
      M5.Lcd.print(cloudyArr[random(8)]);
    } else if (weather == rainy) {
      M5.Lcd.drawJpgFile(SD, "/rainy.jpg", 0, 80);
      M5.Lcd.print(rainyArr[random(4)]);
    }
    counter++;
  } else {
     M5.Lcd.print("NG!");
  }
}



void setup() {
  // put your setup code here, to run once:

  M5.begin();
  M5.Lcd.setBrightness(30);
  M5.Lcd.clear();
  M5.Lcd.setTextSize(2);

  // Font Setup
//  M5.Lcd.setTextWrap(true, true);
  // Font Japanese
//  String fileName = "/Final-Frontier-28.vlw";
//  M5.Lcd.loadFont(fileName, SD);

  // Display title image
  M5.Lcd.drawJpgFile(SD, "/title.jpg", 0, 0);

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
  delay(3000);

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
