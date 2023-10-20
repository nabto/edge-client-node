#pragma once

#include <napi.h>
#include <nabto/nabto_client.h>
#include <napi.h>

class TCPTunnelOpenContext
{
public:
    TCPTunnelOpenContext(NabtoClient *client, NabtoClientTcpTunnel *tcpTunnel, const std::string& service, uint16_t localPort, Napi::Env env)
        : future_(nabto_client_future_new(client)), deferred_(Napi::Promise::Deferred::New(env))
    {
        ttsf_ = TTSF::New(env, "TSFN", 0, 1, this, [](Napi::Env, void *,
                                                      TCPTunnelOpenContext *ctx) {
            delete ctx;
        });
        nabto_client_tcp_tunnel_open(tcpTunnel, future_, service.c_str(), localPort);
        nabto_client_future_set_callback(future_, TCPTunnelOpenContext::futureCallback, this);
    }

    ~TCPTunnelOpenContext()
    {
        nabto_client_future_free(future_);
    }
    static void CallJS(Napi::Env env, Napi::Function callback, TCPTunnelOpenContext *context, void **data)
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
    typedef Napi::TypedThreadSafeFunction<TCPTunnelOpenContext, void *, TCPTunnelOpenContext::CallJS> TTSF;

    static void futureCallback(NabtoClientFuture *future, NabtoClientError ec, void *userData)
    {
        auto *ctx = static_cast<TCPTunnelOpenContext *>(userData);
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

class TCPTunnelCloseContext
{
public:
    TCPTunnelCloseContext(NabtoClient *client, NabtoClientTcpTunnel *tcpTunnel, Napi::Env env)
        : future_(nabto_client_future_new(client)), deferred_(Napi::Promise::Deferred::New(env))
    {
        ttsf_ = TTSF::New(env, "TSFN", 0, 1, this, [](Napi::Env, void *,
                                                      TCPTunnelCloseContext *ctx) {
            delete ctx;
        });
        nabto_client_tcp_tunnel_close(tcpTunnel, future_);
        nabto_client_future_set_callback(future_, TCPTunnelCloseContext::futureCallback, this);
    }

    ~TCPTunnelCloseContext()
    {
        nabto_client_future_free(future_);
    }
    static void CallJS(Napi::Env env, Napi::Function callback, TCPTunnelCloseContext *context, void **data)
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
    typedef Napi::TypedThreadSafeFunction<TCPTunnelCloseContext, void *, TCPTunnelCloseContext::CallJS> TTSF;

    static void futureCallback(NabtoClientFuture *future, NabtoClientError ec, void *userData)
    {
        auto *ctx = static_cast<TCPTunnelCloseContext *>(userData);
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

class TCPTunnel : public Napi::ObjectWrap<TCPTunnel>
{
public:
    static Napi::Object Init(Napi::Env env, Napi::Object exports);
    TCPTunnel(const Napi::CallbackInfo &info);
    ~TCPTunnel();

    NabtoClientTcpTunnel *getCoap()
    {
        return tcpTunnel_;
    }

private:
    Napi::Value GetLocalPort(const Napi::CallbackInfo &info);

    /*
     * return a promise
     */
    Napi::Value Open(const Napi::CallbackInfo &info);
    Napi::Value Close(const Napi::CallbackInfo &info);

    NabtoClient *client_;
    NabtoClientTcpTunnel *tcpTunnel_;
};
