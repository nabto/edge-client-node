#pragma once

#include <napi.h>
#include <nabto/nabto_client.h>
#include <napi.h>

class CoapExecuteContext
{
public:
    CoapExecuteContext(NabtoClient *client, NabtoClientCoap *coap, Napi::Env env)
        : future_(nabto_client_future_new(client)), deferred_(Napi::Promise::Deferred::New(env))
    {
        ttsf_ = TTSF::New(env, "TSFN", 0, 1, this, [](Napi::Env, void *, CoapExecuteContext *ctx)
                          { delete ctx; });
        nabto_client_coap_execute(coap, future_);
        nabto_client_future_set_callback(future_, CoapExecuteContext::futureCallback, this);
    }

    ~CoapExecuteContext()
    {
        nabto_client_future_free(future_);
    }
    static void CallJS(Napi::Env env, Napi::Function callback, CoapExecuteContext *context, void **data)
    {
        if (context->ec_ == NABTO_CLIENT_EC_OK)
        {
            context->deferred_.Resolve(env.Undefined());
        }
        else
        {
            Napi::Error err = Napi::Error::New(callback.Env(), nabto_client_error_get_message(context->ec_));
            err.Set("code", nabto_client_error_get_string(context->ec_));
            context->deferred_.Reject(err.Value());
        }
    }
    typedef Napi::TypedThreadSafeFunction<CoapExecuteContext, void *, CoapExecuteContext::CallJS> TTSF;

    static void futureCallback(NabtoClientFuture *future, NabtoClientError ec, void *userData)
    {
        auto ctx = static_cast<CoapExecuteContext *>(userData);
        ctx->ec_ = ec;

        ctx->ttsf_.NonBlockingCall();
        ctx->ttsf_.Release();
    }

    Napi::Value Promise()
    {
        return deferred_.Promise();
    }

    NabtoClientFuture *future_;
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

    NabtoClient *client_;
    NabtoClientCoap *coap_;
};
