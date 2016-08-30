#ifndef M2XCLIENT_H
#define M2XCLIENT_H

#include <ArduinoJson.h>
#include "Client.h"

static const int E_OK = 0;
static const int E_NOCONNECTION = -1;

static const char* DEFAULT_M2X_HOST = "api-m2x.att.com";
static const int DEFAULT_M2X_PORT = 80;

class M2XClient
{
  public:
    M2XClient(Client* client,
              const char* key,
              const char* host = DEFAULT_M2X_HOST,
              int port = DEFAULT_M2X_PORT);

    int connect(void);
    int deleteValues(const char* deviceId, const char* streamName, const char* from, const char* end);

    template <class T>
    int updateStreamValue(const char* deviceId, const char* streamName, T value);

  private:
    Client* _client;
    const char* _key;
    const char* _host;
    int _port;
};

template <class T>
int M2XClient::updateStreamValue(const char* deviceId, const char* streamName, T value)
{
  DynamicJsonBuffer jsonBuffer;
  JsonObject& root = jsonBuffer.createObject();
  root["value"] = value;

  char buff[100];
  root.printTo(buff, sizeof(buff));
  String strBuff(buff);
  strBuff += "\n";

  String endpoint = "/v2/devices/" + (String)deviceId + "/streams/" + streamName + "/value";

  String request = "PUT " + endpoint + " HTTP/1.1\r\n" +
    "X-M2X-KEY: " + _key + "\r\n" +
    "Host: " + _host + "\r\n" +
    "Content-Type: application/json\r\n" +
    "Content-Length: " + strBuff.length() + "\r\n\r\n" + strBuff;

  _client->print(request);

  String response = "";
  String chunk = "";
  int limit = 0;
  int sendCount = 0;

  do
  {
    if (_client->connected())
    {
      chunk = _client->readStringUntil('\n');
      response += chunk;
    }
    limit++;
  } while (chunk.length() > 0 && limit < 100);

  if (response.length() > 12)
  {
    String responseCode = response.substring(9, 12);
    if(responseCode == "202")
    {
      return E_OK;
    }
  }

  return -1;
}

#endif
