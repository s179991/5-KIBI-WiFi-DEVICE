#include "settings.h"
#include "utils.h"
#include "EEMEM.h"

#include "settings_html.h"

AsyncWebServer server(80);
AsyncWebSocket socket("/socket/");

int NetInit(void);
int ServerInit(void);
void Error(void);

//Socket Functions
void onWebSocketEvent(AsyncWebSocket *server, AsyncWebSocketClient *client, AwsEventType type, void *arg, uint8_t *data, size_t len);
void onWebSocketMessage(void *arg, uint8_t *data, size_t len);

void setup() 
{
  LED_CONFIG;
  SERIAL_CONFIG;

  NetInit();
  ServerInit();

  ledTime(50, 50);
}

void loop() 
{
  static int delta_time = 0;

  GetElapsedTime(delta_time);
  ledTask(delta_time);

  int ret = WiFiConnectionTask();
  if (ret ==  1)  // connected to WiFi
  {
    ledTime(50, 450);
    try_api_connect = true;
    wifi_is_connected = true;
    SocketSendConnectionInfo(socket, "wifi_connection_success");
  }
  if (ret == -1)  // connection to WiFi failed
  {
    ledTime(50, 50);
    wifi_is_connected = false;
    try_api_connect = false;
    api_is_connected = false;
    SocketSendConnectionInfo(socket, "wifi_connection_failed");
  }

  if (!wifi_is_connected) { return; }

  if (try_api_connect)
  {
    ret = ApiConnectionTask(delta_time);
    if (ret == 1)
    {
      try_api_connect = false;
      api_is_connected = true;
      SocketSendConnectionInfo(socket, "api_connection_success");
    }
  }

  if (!api_is_connected) { return; }
  
  ledTime(500, 500);
}

int NetInit()
{
  LOG("");
  LOG("");

  IPAddress IP(192,168,1,1);
  IPAddress gateway(192,168,1,1);
  IPAddress subnet(255,255,255,0);

  EEMEM_GetParams(STA_HOST, STA_PORT, STA_SSID, STA_PASS);

  bool success = true;
  // mode: AP and STA
  success &= WiFi.mode(WIFI_AP_STA);
  // Configuration AP Mode
  success &= WiFi.softAPConfig(IP, gateway, subnet);
  // Start AP Mode with
  if (success)
  {
    success = WiFi.softAP(AP_SSID, AP_PASS);
  }
  else
  {
    LOG("Soft AP configuration error for IP: " + IP.toString() + ", gateway: " + gateway.toString() + ", subnet: " + subnet.toString());
    Error();
  }
  // WiFi Connection begi
  WiFi.begin(STA_SSID, STA_PASS);

  if (success)
  {
    LOG("AP configuration:");
    LOG("    SSID: " + AP_SSID);
    LOG("    PASS: " + AP_PASS);
    LOG("     IP:  " + WiFi.softAPIP().toString());
    LOG("     MAC: " + WiFi.macAddress());
    LOG("");
    LOG("STA configuration:");
    LOG("    SSID: " + STA_SSID);
    LOG("    PASS: " + STA_PASS);
    LOG("    HOST: " + STA_HOST);
    LOG("    PORT: " + STA_PORT);
  }
  else
  {
    LOG("AP & STA configuration error!");
    Error();
  }
  return 0;
}

int ServerInit()
{
  socket.onEvent(onWebSocketEvent);
  server.addHandler(&socket);

  server.on("/ping/", HTTP_GET, [] (AsyncWebServerRequest *request) {
    request->send(200, "text/plain", "OK");
  });

  server.on("/settings/", HTTP_GET, [] (AsyncWebServerRequest *request) {
    request->send_P(200, "text/html", settings_html, processor);
  });

  server.on("/", HTTP_GET, [] (AsyncWebServerRequest *request) {
    request->redirect("/settings/");
  });

  server.begin();

  return 1;
}

void Error()
{
  while (true)
  {
    LED_ON;
    delay(50);
    LED_OFF;
    delay(50);
  }
}

void onWebSocketEvent(AsyncWebSocket *server, AsyncWebSocketClient *client, AwsEventType type, void *arg, uint8_t *data, size_t len) 
{
    switch (type) {
      case WS_EVT_CONNECT:
        LOG();
        LOG("WebSocket client connected");
        break;
      case WS_EVT_DISCONNECT:
        LOG();
        LOG("WebSocket client disconnected");
        break;
      case WS_EVT_DATA:
        onWebSocketMessage(arg, data, len);
        break;
      case WS_EVT_PONG:
      case WS_EVT_ERROR:
        break;
  }
}

void onWebSocketMessage(void *arg, uint8_t *data, size_t len) 
{
  AwsFrameInfo *info = (AwsFrameInfo*)arg;
  if (info->final && info->index == 0 && info->len == len && info->opcode == WS_TEXT) 
  {
    const uint8_t size = JSON_OBJECT_SIZE(5);
    StaticJsonDocument<size> json;
    DeserializationError err = deserializeJson(json, data);
    if (err) 
    {
      LOG();
      LOG(F("deserializeJson() failed with code ") + (String)err.c_str());
      return;
    }

    const char *type = json["type"];
    if (strcmp(type, "save") == 0) 
    {
      bool newWiFiConnection = false;
      bool newApiConnection = false;
      String SSID = (String)json["SSID"];
      String PASS = (String)json["PASS"];
      String HOST = (String)json["HOST"];
      String PORT = (String)json["PORT"];
      LOG();
      LOG("New STA configuration:");
      LOG("    SSID: " + SSID);
      LOG("    PASS: " + PASS);
      LOG("    HOST: " + HOST);
      LOG("    PORT: " + PORT);

      if (STA_SSID != SSID) 
      {
        EEMEM_SetSSID(SSID);
        STA_SSID = SSID;
        newWiFiConnection = true;
      }
      if (STA_PASS != PASS) 
      {
        EEMEM_SetPASS(PASS);
        STA_PASS = PASS;
        newWiFiConnection = true;
      }
      if (STA_HOST != HOST) 
      {
        EEMEM_SetHOST(HOST);
        STA_HOST = HOST;
        newApiConnection = true;
      }
      if (STA_PORT != PORT) 
      {
        EEMEM_SetPORT(PORT);
        STA_PORT = PORT;
        newApiConnection = true;
      }
      if (newWiFiConnection)
      {
        WiFi.disconnect();
        WiFi.begin(STA_SSID, STA_PASS);
        ledTime(50, 50);
        wifi_is_connected = false;
        api_is_connected = false;
        try_api_connect = false;
        retry_time = 0;
        retry_cnt = 0;
      }
      if (newApiConnection) 
      {
        api_is_connected = false;
        if (wifi_is_connected)
          try_api_connect = true;
        retry_time = 0;
        retry_cnt = 0;
      }
      SocketSendSaveConfirmation(socket);
    }
  }
}


