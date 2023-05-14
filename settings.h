#ifndef SETTINGS
#define SETTINGS

#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <ESPAsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <ArduinoJson.h>

#define DEBUG           true
#define LED             2

#define LED_IS_ON       (digitalRead(LED) == LOW)
#define LED_ON          digitalWrite(LED, LOW)
#define LED_OFF         digitalWrite(LED, HIGH)
#define LED_CONFIG      pinMode(LED, OUTPUT); LED_OFF

#define SERIAL_CONFIG   Serial.begin(115200);

#define WIFI_IS_CONNECTED   (WiFi.status() == WL_CONNECTED)


#if DEBUG
  #define LOG(text) Serial.println(text)
#else
  #define LOG(text) ;
#endif

const String DEVICE_TOKEN = "1@r#(h=(m78ko5r8ebff*o4#aofihhp(ru1pj6tm731!m2oez_";

const String STA_SSID_DEFAULT = "BoroBanino";
const String STA_PASS_DEFAULT = "!@Borowik0599";
const String STA_HOST_DEFAULT = "192.168.0.103";
const String STA_PORT_DEFAULT = "8000";

const String AP_SSID = "5@KIBI'2023pb-config";
const String AP_PASS = "admin123";

String STA_SSID = "";
String STA_PASS = "";
String STA_HOST = "";
String STA_PORT = "";

bool try_api_connect = false;
bool api_is_connected = false;
bool wifi_is_connected = false;
uint retry_time = 0;
uint retry_cnt = 0;

#endif