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
