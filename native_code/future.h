#pragma once

#include <napi.h>
#include <nabto/nabto_client.h>

class FutureContext
{
public:
    FutureContext(NabtoClient *client, Napi::Env env)
    : future_(nabto_client_future_new(client)), client_(client), deferred_(Napi::Promise::Deferred::New(env))
    {
        ttsf_ = TTSF::New(env, "TSFN", 0, 1, this, [](Napi::Env, void *, FutureContext *ctx)
                          { delete ctx; });
    }

    virtual ~FutureContext()
    {
        nabto_client_future_free(future_);
    }

    void arm(bool repeatable) {
        repeatable_ = repeatable;
        deferred_ = Napi::Promise::Deferred::New(deferred_.Env());
        nabto_client_future_set_callback(future_, FutureContext::futureCallback, this);
    }

    void stop() {
        ttsf_.Release();
    }

    static void CallJS(Napi::Env env, Napi::Function callback, FutureContext *context, void **data)
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
    typedef Napi::TypedThreadSafeFunction<FutureContext, void *, FutureContext::CallJS> TTSF;

    static void futureCallback(NabtoClientFuture *future, NabtoClientError ec, void *userData)
    {
        auto ctx = static_cast<FutureContext *>(userData);
        ctx->ec_ = ec;

        ctx->ttsf_.NonBlockingCall();
        if (!ctx->repeatable_) {
            ctx->stop();
        }
    }

    Napi::Value Promise()
    {
        return deferred_.Promise();
    }

    NabtoClientFuture* future_;
    NabtoClient* client_;
    TTSF ttsf_;
    Napi::Promise::Deferred deferred_;
    NabtoClientError ec_;
    bool repeatable_ = false;
};
