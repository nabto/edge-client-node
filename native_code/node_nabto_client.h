#pragma once

#include <napi.h>
#include <nabto/nabto_client.h>

class LogMessage {
 public:
    LogMessage(std::string message, std::string severity) 
      : message_(message), severity_(severity)
    {
    }

    std::string message_;
    std::string severity_;
};


using Context = void;
void CallJs(Napi::Env env, Napi::Function callback, Context *context, LogMessage *data);
typedef Napi::TypedThreadSafeFunction<Context, LogMessage, CallJs> LogCallbackFunction;

class NodeNabtoClient : public Napi::ObjectWrap<NodeNabtoClient> {
 public:
  static Napi::Object Init(Napi::Env env, Napi::Object exports);
  virtual void Finalize(Napi::Env env);
  NodeNabtoClient(const Napi::CallbackInfo& info);
  ~NodeNabtoClient();
  void Stop(const Napi::CallbackInfo& info);

  NabtoClient* getClient() { return nabtoClient_; }

 private:
  static void LogCallback(const NabtoClientLogMessage* log, void* userData);
  Napi::Value GetVersion(const Napi::CallbackInfo& info);
  Napi::Value CreatePrivateKey(const Napi::CallbackInfo& info);
  void SetLogLevel(const Napi::CallbackInfo& info);
  void SetLogCallback(const Napi::CallbackInfo& info);

  NabtoClient* nabtoClient_;
  LogCallbackFunction logCallback_;
};
