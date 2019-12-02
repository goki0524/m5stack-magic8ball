#define M5STACK_MPU6886
#include <M5Stack.h>
#include <WiFi.h>
#include <ESPmDNS.h>
#include <WiFiClient.h>
#include "WebServer.h"
#include <Preferences.h>
#include <ArduinoJson.h>
#include <HTTPClient.h>

const IPAddress apIP(192, 168, 4, 1);
const char* apSSID = "M5STACK_SETUP";
boolean settingMode;
String ssidList;
String wifi_ssid;
String wifi_password;

// DNSServer dnsServer;
WebServer webServer(80);

// wifi config store
Preferences preferences;

// WEB API 
const String citycode = "1252408";
const String baseurl = "http://api.openweathermap.org/data/2.5/forecast";
const String apikey = "apikey";
const String sunny = "Clear";
const String cloudy = "Clouds";
const String rainy = "Rain";
const char* imgNameArr[] = {"/title_01.jpg", "/answer_01.jpg", "/answer_02.jpg", "/answer_03.jpg", "/answer_04.jpg", "/answer_05.jpg", "/answer_06.jpg", "/answer_07.jpg", "/answer_08.jpg", "/answer_09.jpg",
"/answer_10.jpg", "/answer_11.jpg", "/answer_12.jpg", "/answer_13.jpg", "/answer_14.jpg", "/answer_15.jpg", "/answer_16.jpg", "/answer_17.jpg", "/answer_18.jpg", "/answer_19.jpg", "/answer_20.jpg", "/title_02.jpg"};
unsigned int counter;

float accX = 0.0F;
float accY = 0.0F;
float accZ = 0.0F;
int MoPin = 26;  // vibrator Grove connected to digital pin 26

void setup() {
  M5.begin();
  preferences.begin("wifi-config");

  M5.Power.begin();
  // Initialize IMU
  M5.IMU.Init();
  pinMode( MoPin, OUTPUT );

  delay(10);
  if (restoreConfig()) {
    if (checkConnection()) {
      settingMode = false;
      shakeStart();
//      startWebServer();
      return;
    }
  }
  settingMode = true;
  setupMode();
}

void loop() {
  // update button state
  M5.update();
  
  if (settingMode) {
  } else {
    

    if (counter == 0 && shakeCheck()) {
      updateWeather();
      digitalWrite(MoPin, LOW);
    }

    if (counter == 1 && M5.BtnB.wasReleased()) {
      shakeStart();
    }
 
  }
  webServer.handleClient();
}

void shakeStart() {
  
  M5.Lcd.drawJpgFile(SD, imgNameArr[21], 0, 0);
  counter = 0;
}

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

void updateWeather() {
  HTTPClient http;
  String url = baseurl + "?id=" + citycode + "&APPID=" + apikey;
//  http.begin(url);
//  int httpCode = http.GET();
//  if (httpCode == HTTP_CODE_OK) {
   if (true) { // wifi-off
    String payload = http.getString();
    payload.replace("\\", "¥");
    DynamicJsonDocument doc(15000);
    deserializeJson(doc, payload);
    JsonVariant tomorrow = doc["list"][0];
    String weather = tomorrow["weather"][0]["main"];
    
    if (true) { // wifi-off >>>
       M5.Lcd.drawJpgFile(SD, imgNameArr[random(1, 20)], 0, 0);
    }// wifi-off <<<

//    if (weather == sunny) {
//      M5.Lcd.drawJpgFile(SD, imgNameArr[random(1, 6)], 0, 0);
//    } else if (weather == cloudy) {
//      M5.Lcd.drawJpgFile(SD, imgNameArr[random(7, 15)], 0, 0);
//    } else if (weather == rainy) {
//      M5.Lcd.drawJpgFile(SD, imgNameArr[random(16, 20)], 0, 0);
//    }

    counter++;
  } else {
     M5.Lcd.print("WebAPI NG!");
  }
}


boolean restoreConfig() {
  wifi_ssid = preferences.getString("WIFI_SSID");
  wifi_password = preferences.getString("WIFI_PASSWD");
  Serial.print("WIFI-SSID: ");
  M5.Lcd.print("WIFI-SSID: ");
  Serial.println(wifi_ssid);
  M5.Lcd.println(wifi_ssid);
  Serial.print("WIFI-PASSWD: ");
  M5.Lcd.print("WIFI-PASSWD: ");
  Serial.println(wifi_password);
  M5.Lcd.println(wifi_password);
  WiFi.begin(wifi_ssid.c_str(), wifi_password.c_str());

  if(wifi_ssid.length() > 0) {
    return true;
} else {
    return false;
  }
}

boolean checkConnection() {
  int count = 0;
  Serial.print("Waiting for Wi-Fi connection");
  M5.Lcd.print("Waiting for Wi-Fi connection");
  while ( count < 30 ) {
    if (WiFi.status() == WL_CONNECTED) {
      Serial.println();
      M5.Lcd.println();
      Serial.println("Connected!");
      M5.Lcd.println("Connected!");
      return (true);
    }
    delay(500);
    Serial.print(".");
    M5.Lcd.print(".");
    count++;
  }
  Serial.println("Timed out.");
  M5.Lcd.println("Timed out.");
  return false;
}

void startWebServer() {
  if (settingMode) {
    Serial.print("Starting Web Server at ");
    M5.Lcd.print("Starting Web Server at ");
    Serial.println(WiFi.softAPIP());
    M5.Lcd.println(WiFi.softAPIP());
    webServer.on("/settings", []() {
      String s = "<h1>Wi-Fi Settings</h1><p>Please enter your password by selecting the SSID.</p>";
      s += "<form method=\"get\" action=\"setap\"><label>SSID: </label><select name=\"ssid\">";
      s += ssidList;
      s += "</select><br>Password: <input name=\"pass\" length=64 type=\"password\"><input type=\"submit\"></form>";
      webServer.send(200, "text/html", makePage("Wi-Fi Settings", s));
    });
    webServer.on("/setap", []() {
      String ssid = urlDecode(webServer.arg("ssid"));
      Serial.print("SSID: ");
      M5.Lcd.print("SSID: ");
      Serial.println(ssid);
      M5.Lcd.println(ssid);
      String pass = urlDecode(webServer.arg("pass"));
      Serial.print("Password: ");
      M5.Lcd.print("Password: ");
      Serial.println(pass);
      M5.Lcd.println(pass);
      Serial.println("Writing SSID to EEPROM...");
      M5.Lcd.println("Writing SSID to EEPROM...");

      // Store wifi config
      Serial.println("Writing Password to nvr...");
      M5.Lcd.println("Writing Password to nvr...");
      preferences.putString("WIFI_SSID", ssid);
      preferences.putString("WIFI_PASSWD", pass);

      Serial.println("Write nvr done!");
      M5.Lcd.println("Write nvr done!");
      String s = "<h1>Setup complete.</h1><p>device will be connected to \"";
      s += ssid;
      s += "\" after the restart.";
      webServer.send(200, "text/html", makePage("Wi-Fi Settings", s));
      delay(3000);
      ESP.restart();
    });
    webServer.onNotFound([]() {
      String s = "<h1>AP mode</h1><p><a href=\"/settings\">Wi-Fi Settings</a></p>";
      webServer.send(200, "text/html", makePage("AP mode", s));
    });
  }
  else {
    Serial.print("Starting Web Server at ");
    M5.Lcd.print("Starting Web Server at ");
    Serial.println(WiFi.localIP());
    M5.Lcd.println(WiFi.localIP());
    
    webServer.on("/", []() {
      String s = "<h1>STA mode</h1><p><a href=\"/reset\">Reset Wi-Fi Settings</a></p>";
      webServer.send(200, "text/html", makePage("STA mode", s));
    });
    webServer.on("/reset", []() {
      // reset the wifi config
      preferences.remove("WIFI_SSID");
      preferences.remove("WIFI_PASSWD");
      String s = "<h1>Wi-Fi settings was reset.</h1><p>Please reset device.</p>";
      webServer.send(200, "text/html", makePage("Reset Wi-Fi Settings", s));
      delay(3000);
      ESP.restart();
    });
    
  }
  webServer.begin();
}

void setupMode() {
  WiFi.mode(WIFI_MODE_STA);
  WiFi.disconnect();
  delay(100);
  int n = WiFi.scanNetworks();
  delay(100);
  Serial.println("");
  M5.Lcd.println("");
  for (int i = 0; i < n; ++i) {
    ssidList += "<option value=\"";
    ssidList += WiFi.SSID(i);
    ssidList += "\">";
    ssidList += WiFi.SSID(i);
    ssidList += "</option>";
  }
  delay(100);
  WiFi.softAPConfig(apIP, apIP, IPAddress(255, 255, 255, 0));
  WiFi.softAP(apSSID);
  WiFi.mode(WIFI_MODE_AP);
  // WiFi.softAPConfig(IPAddress local_ip, IPAddress gateway, IPAddress subnet);
  // WiFi.softAP(const char* ssid, const char* passphrase = NULL, int channel = 1, int ssid_hidden = 0);
  // dnsServer.start(53, "*", apIP);
  startWebServer();
  Serial.print("Starting Access Point at \"");
  M5.Lcd.print("Starting Access Point at \"");
  Serial.print(apSSID);
  M5.Lcd.print(apSSID);
  Serial.println("\"");
  M5.Lcd.println("\"");
}

String makePage(String title, String contents) {
  String s = "<!DOCTYPE html><html><head>";
  s += "<meta name=\"viewport\" content=\"width=device-width,user-scalable=0\">";
  s += "<title>";
  s += title;
  s += "</title></head><body>";
  s += contents;
  s += "</body></html>";
  return s;
}

String urlDecode(String input) {
  String s = input;
  s.replace("%20", " ");
  s.replace("+", " ");
  s.replace("%21", "!");
  s.replace("%22", "\"");
  s.replace("%23", "#");
  s.replace("%24", "$");
  s.replace("%25", "%");
  s.replace("%26", "&");
  s.replace("%27", "\'");
  s.replace("%28", "(");
  s.replace("%29", ")");
  s.replace("%30", "*");
  s.replace("%31", "+");
  s.replace("%2C", ",");
  s.replace("%2E", ".");
  s.replace("%2F", "/");
  s.replace("%2C", ",");
  s.replace("%3A", ":");
  s.replace("%3A", ";");
  s.replace("%3C", "<");
  s.replace("%3D", "=");
  s.replace("%3E", ">");
  s.replace("%3F", "?");
  s.replace("%40", "@");
  s.replace("%5B", "[");
  s.replace("%5C", "\\");
  s.replace("%5D", "]");
  s.replace("%5E", "^");
  s.replace("%5F", "-");
  s.replace("%60", "`");
  return s;
}