#pragma once

#include <napi.h>
#include <nabto/nabto_client.h>

class Connection;


class ConnectionConnectContext;

class ConnectionConnectContext {
 public:
    ConnectionConnectContext(NabtoClient* client, NabtoClientConnection* connection, Napi::Env env) 
        : future_(nabto_client_future_new(client)),  deferred_(Napi::Promise::Deferred::New(env))
    {
        ttsf_ = TTSF::New(env, "TSFN", 0, 1, this, [](Napi::Env, void *,
                                                                                            ConnectionConnectContext *ctx) { // Finalizer used to clean threads up
            delete ctx;
        });
        nabto_client_connection_connect(connection, future_);
        nabto_client_future_set_callback(future_, ConnectionConnectContext::futureCallback, this);
    }

    ~ConnectionConnectContext() {
        nabto_client_future_free(future_);
    }
    static void CallJS(Napi::Env env, Napi::Function callback, ConnectionConnectContext *context, void **data)
    {
        if (context->ec_ == NABTO_CLIENT_EC_OK)
        {
            context->deferred_.Resolve(Napi::String::New(env, "OK"));
        }
        else
        {
            context->deferred_.Reject(Napi::String::New(env, nabto_client_error_get_message(context->ec_)));
        }
    }
    typedef Napi::TypedThreadSafeFunction<ConnectionConnectContext, void *, ConnectionConnectContext::CallJS> TTSF;

    static void futureCallback(NabtoClientFuture* future, NabtoClientError ec, void* userData)
    {
        ConnectionConnectContext* coap = static_cast<ConnectionConnectContext*>(userData);
        coap->ec_ = ec;
        
        coap->ttsf_.NonBlockingCall();
        coap->ttsf_.Release();
    }

    Napi::Value Promise() {
        return deferred_.Promise();
    }

    NabtoClientFuture* future_;
    TTSF ttsf_;
    Napi::Promise::Deferred deferred_;
    NabtoClientError ec_;
};

class Connection : public Napi::ObjectWrap<Connection> {
 public:
  static Napi::Object Init(Napi::Env env, Napi::Object exports);
  Connection(const Napi::CallbackInfo& info);
  ~Connection();

  NabtoClientConnection* getConnection() {
    return connection_;
  }

 private:

  
  Napi::Value GetClientFingerprint(const Napi::CallbackInfo& info);
  Napi::Value Connect(const Napi::CallbackInfo& info);
  void SetOptions(const Napi::CallbackInfo& info);

  NabtoClient* client_;
  NabtoClientConnection* connection_;
};
