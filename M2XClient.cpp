#include "M2XClient.h"

M2XClient::M2XClient(Client* client,
                    const char* key,
                    const char* host,
                    int port)
{
  _client = client;
  _key = key;
  _host = host;
  _port = port;
}

int M2XClient::connect(void)
{
  if(!_client->connect(_host, _port))
  {
    return E_NOCONNECTION;
  }

  return E_OK;
}

int M2XClient::deleteValues(const char* deviceId, const char* streamName, const char* from, const char* end)
{
  DynamicJsonBuffer jsonBuffer;
  JsonObject& root = jsonBuffer.createObject();
  root["from"] = from;
  root["end"] = end;

  char buff[100];
  root.printTo(buff, sizeof(buff));
  String strBuff(buff);
  strBuff += "\n";

  String endpoint = "/v2/devices/" + (String)deviceId + "/streams/" + streamName + "/values";

  String request = "DELETE " + endpoint + " HTTP/1.1\r\n" +
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
    if(responseCode == "204")
    {
      return E_OK;
    }
  }

  return -1;
}
