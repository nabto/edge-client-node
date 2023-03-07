#pragma once

#include <napi.h>
#include <nabto/nabto_client.h>
#include <napi.h>

class CoapExecuteContext;

class CoapExecuteContext {
 public:
    CoapExecuteContext(NabtoClient* client, NabtoClientCoap* coap, Napi::Env env) 
        : future_(nabto_client_future_new(client)),  deferred_(Napi::Promise::Deferred::New(env))
    {
        ttsf_ = TTSF::New(env, "TSFN", 0, 1, this, [](Napi::Env, void *,
                                                                                            CoapExecuteContext *ctx) { // Finalizer used to clean threads up
            delete ctx;
        });
        nabto_client_coap_execute(coap, future_);
        nabto_client_future_set_callback(future_, CoapExecuteContext::futureCallback, this);
    }

    ~CoapExecuteContext() {
        nabto_client_future_free(future_);
    }
    static void CallJS(Napi::Env env, Napi::Function callback, CoapExecuteContext *context, void **data)
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
    typedef Napi::TypedThreadSafeFunction<CoapExecuteContext, void *, CoapExecuteContext::CallJS> TTSF;

    static void futureCallback(NabtoClientFuture* future, NabtoClientError ec, void* userData)
    {
        CoapExecuteContext* coap = static_cast<CoapExecuteContext*>(userData);
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

class Coap : public Napi::ObjectWrap<Coap>
{
public:
    static Napi::Object Init(Napi::Env env, Napi::Object exports);
    Coap(const Napi::CallbackInfo &info);
    ~Coap();

    NabtoClientCoap *getCoap()
    {
        return coap_;
    }

private:
    void SetRequestPayload(const Napi::CallbackInfo &info);
    Napi::Value GetResponsePayload(const Napi::CallbackInfo &info);
    Napi::Value GetResponseStatusCode(const Napi::CallbackInfo &info);
    Napi::Value GetResponseContentFormat(const Napi::CallbackInfo &info);
    /*
     * return a promise 
     */
    Napi::Value Execute(const Napi::CallbackInfo &info);
    
    NabtoClient* client_;
    NabtoClientCoap* coap_;

};
