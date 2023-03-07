#include <napi.h>
#include "node_nabto_client.h"
#include "connection.h"
#include "coap.h"
#include "tcp_tunnel.h"

Napi::Object InitAll(Napi::Env env, Napi::Object exports) {
  Napi::Object tmp = NodeNabtoClient::Init(env, exports);
  tmp = Coap::Init(env,tmp);
  tmp = TCPTunnel::Init(env,tmp);
  return Connection::Init(env, tmp);
}

NODE_API_MODULE(addon, InitAll)