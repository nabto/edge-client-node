#include "node_nabto_client.h"

#include <nabto/nabto_client.h>

#include <iostream>

Napi::Object NodeNabtoClient::Init(Napi::Env env, Napi::Object exports) {
  Napi::Function func =
      DefineClass(env,
                  "NabtoClient",
                  {InstanceMethod("getVersion", &NodeNabtoClient::GetVersion),
                   InstanceMethod("createPrivateKey", &NodeNabtoClient::CreatePrivateKey),
                   InstanceMethod("setLogLevel", &NodeNabtoClient::SetLogLevel),
                   InstanceMethod("setLogCallback", &NodeNabtoClient::SetLogCallback),
                   InstanceMethod("stop", &NodeNabtoClient::Stop)
                   });

  Napi::FunctionReference* constructor = new Napi::FunctionReference();
  *constructor = Napi::Persistent(func);
  env.SetInstanceData(constructor);

  exports.Set("NabtoClient", func);
  return exports;
}

NodeNabtoClient::NodeNabtoClient(const Napi::CallbackInfo& info)
    : Napi::ObjectWrap<NodeNabtoClient>(info) {

  nabtoClient_ = nabto_client_new();
}

NodeNabtoClient::~NodeNabtoClient() 
{
    nabto_client_free(nabtoClient_);
}

void NodeNabtoClient::Finalize(Napi::Env env)
{
}

void NodeNabtoClient::Stop(const Napi::CallbackInfo& info)
{
    nabto_client_set_log_callback(nabtoClient_, NULL, NULL);
    if (logCallback_ != nullptr) {
        logCallback_.Release();
    }
}

Napi::Value NodeNabtoClient::GetVersion(const Napi::CallbackInfo& info) {
  return Napi::String::New(info.Env(), nabto_client_version());
}

Napi::Value NodeNabtoClient::CreatePrivateKey(const Napi::CallbackInfo& info)
{
    char* key;
    NabtoClientError ec = nabto_client_create_private_key(nabtoClient_, &key);

    if (ec != NABTO_CLIENT_EC_OK) {
        Napi::Error::New(info.Env(), nabto_client_error_get_message(ec)).ThrowAsJavaScriptException();
        return info.Env().Undefined();
    }

    Napi::Value retVal = Napi::String::New(info.Env(), key);

    nabto_client_string_free(key);
    return retVal;
}

void NodeNabtoClient::SetLogLevel(const Napi::CallbackInfo& info)
{
  int length = info.Length();
  Napi::Value result;
  if (length <= 0 || !info[0].IsString() ) {
    Napi::TypeError::New(info.Env(), "String expected").ThrowAsJavaScriptException();
    return;
  }

  NabtoClientError ec = nabto_client_set_log_level(nabtoClient_, info[0].ToString().Utf8Value().c_str());
  if (ec != NABTO_CLIENT_EC_OK) {
    Napi::Error::New(info.Env(), nabto_client_error_get_message(ec)).ThrowAsJavaScriptException();
  }
}


void CallJs(Napi::Env env, Napi::Function callback, Context *context, LogMessage *data)
{

  // Is the JavaScript environment still available to call into, eg. the TSFN is
  // not aborted
  if (env != nullptr) {
    // On Node-API 5+, the `callback` parameter is optional; however, this example
    // does ensure a callback is provided.
    if (callback != nullptr) {
        Napi::Object o = Napi::Object::New(callback.Env());
        o.Set("message", data->message_);
        o.Set("severity", data->severity_);
      callback.Call({o});
    }
  }
  if (data != nullptr) {
    // We're finished with the data.
    delete data;
  }
}

void NodeNabtoClient::LogCallback(const NabtoClientLogMessage* log, void* userData)
{
    NodeNabtoClient* client = static_cast<NodeNabtoClient*>(userData);

    auto l = new LogMessage(log->message, log->severityString);
    client->logCallback_.NonBlockingCall(l);
}

void NodeNabtoClient::SetLogCallback(const Napi::CallbackInfo& info)
{
  //Napi::Env env = info.Env();
  Napi::Function cb = info[0].As<Napi::Function>();
  //Context *context = new Napi::Reference<Napi::Value>(Persistent(info.This()));
  logCallback_ = LogCallbackFunction::New(cb.Env(), cb, "TSFN", 0, 1, NULL, [](Napi::Env, void*,
        Context *ctx) { // Finalizer used to clean threads up
        //delete ctx;
      });
    nabto_client_set_log_callback(nabtoClient_, NodeNabtoClient::LogCallback, this);
}

