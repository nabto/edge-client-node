#include <napi.h>
#include "node_nabto_client.h"
#include "connection.h"
#include "coap.h"

Napi::Object InitAll(Napi::Env env, Napi::Object exports) {
  Napi::Object tmp = NodeNabtoClient::Init(env, exports);
  auto tmp2 = Coap::Init(env,tmp);
  return Connection::Init(env, tmp2);
}

NODE_API_MODULE(addon, InitAll)