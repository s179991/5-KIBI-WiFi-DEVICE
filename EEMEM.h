#ifndef ESP_EEPROM
#define ESP_EEPROM

#include <EEPROM.h>

#include "settings.h"

#define EEMEM_SIZE      1024
#define EEMEM_HOST_ADDR 0
#define EEMEM_PORT_ADDR 256
#define EEMEM_SSID_ADDR 512
#define EEMEM_PASS_ADDR 768

bool EEMEM_Write(uint address, const String data);
bool EEMEM_Read(uint address, String &data);

bool EEMEM_GetParams(String &HOST, String &PORT, String &SSID, String &PASS);
bool EEMEM_GetHOST(String &HOST);
bool EEMEM_GetPORT(String &PORT);
bool EEMEM_GetSSID(String &SSID);
bool EEMEM_GetPASS(String &PASS);
bool EEMEM_SetHOST(String HOST);
bool EEMEM_SetPORT(String PORT);
bool EEMEM_SetSSID(String SSID);
bool EEMEM_SetPASS(String PASS);

bool EEMEM_Write(uint address, const String data)
{
  const char *c = data.c_str();

  EEPROM.begin(EEMEM_SIZE);
  while (*c != 0)
  {
    EEPROM.put(address, *c);
    address++;
    c++;
  }
  EEPROM.put(address, 0);
  EEPROM.commit();
  return EEPROM.end();
}
bool EEMEM_Read(uint address, String &data)
{
  char c;
  data = "";
  EEPROM.begin(EEMEM_SIZE);
  while (true)
  {
    EEPROM.get(address, c);
    address++;

    if (c == 255)
    {
      EEPROM.end();
      return false;
    }

    if (c == 0)
    {
      EEPROM.end();
      return true;
    }

    if (c != 0)
    {
      data += c;
    }
  }
  EEPROM.end();
  return true;
}
bool EEMEM_GetParams(String &HOST, String &PORT, String &SSID, String &PASS)
{
  bool success = true;
  success &= EEMEM_GetHOST(HOST);
  success &= EEMEM_GetPORT(PORT);
  success &= EEMEM_GetSSID(SSID);
  success &= EEMEM_GetPASS(PASS);
  return success;
}
bool EEMEM_GetHOST(String &HOST)
{
  if (!EEMEM_Read(EEMEM_HOST_ADDR, HOST))
  {
    EEMEM_Write(EEMEM_HOST_ADDR, STA_HOST_DEFAULT);
    HOST = STA_HOST_DEFAULT;
    return false;
  }
  return true;
}
bool EEMEM_GetPORT(String &PORT)
{
  if (!EEMEM_Read(EEMEM_PORT_ADDR, PORT))
  {
    EEMEM_Write(EEMEM_PORT_ADDR, STA_PORT_DEFAULT);
    PORT = STA_PORT_DEFAULT;
    return false;
  }
  return true;
}
bool EEMEM_GetSSID(String &SSID)
{
  if (!EEMEM_Read(EEMEM_SSID_ADDR, SSID))
  {
    EEMEM_Write(EEMEM_SSID_ADDR, STA_SSID_DEFAULT);
    SSID = STA_SSID_DEFAULT;
    return false;
  }
  return true;
}
bool EEMEM_GetPASS(String &PASS)
{
  if (!EEMEM_Read(EEMEM_PASS_ADDR, PASS))
  {
    EEMEM_Write(EEMEM_PASS_ADDR, STA_PASS_DEFAULT);
    PASS = STA_PASS_DEFAULT;
    return false;
  }
  return true;
}
bool EEMEM_SetHOST(String HOST)
{
  return EEMEM_Write(EEMEM_HOST_ADDR, HOST);
}
bool EEMEM_SetPORT(String PORT)
{
  return EEMEM_Write(EEMEM_PORT_ADDR, PORT);
}
bool EEMEM_SetSSID(String SSID)
{
  return EEMEM_Write(EEMEM_SSID_ADDR, SSID);
}
bool EEMEM_SetPASS(String PASS)
{
  return EEMEM_Write(EEMEM_PASS_ADDR, PASS);
}

#endif