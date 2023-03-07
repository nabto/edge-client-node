#include "coap.h"
#include "connection.h"
#include "tcp_tunnel.h"
#include "node_nabto_client.h"

Napi::Object TCPTunnel::Init(Napi::Env env, Napi::Object exports)
{
    Napi::Function func =
        DefineClass(env,
                    "TCPTunnel",
                    {InstanceMethod("getLocalPort", &TCPTunnel::GetLocalPort),
                     InstanceMethod("open", &TCPTunnel::Open),
                     InstanceMethod("close", &TCPTunnel::Close)});

    Napi::FunctionReference *constructor = new Napi::FunctionReference();
    *constructor = Napi::Persistent(func);
    env.SetInstanceData(constructor);

    exports.Set("TCPTunnel", func);
    return exports;
}

TCPTunnel::TCPTunnel(const Napi::CallbackInfo &info)
    : Napi::ObjectWrap<TCPTunnel>(info)
{
    Napi::Env env = info.Env();

    int length = info.Length();
    if (length < 2)
    {
        Napi::TypeError::New(env, "Expects 2 arguments client, connection").ThrowAsJavaScriptException();
    }
    Napi::Value client = info[0];
    Napi::Value connection = info[1];
    
    if (!client.IsObject())
    {
        Napi::TypeError::New(env, "Nabto Client object expected").ThrowAsJavaScriptException();
        return;
    }

    if (!connection.IsObject())
    {
        Napi::TypeError::New(env, "Nabto Connection object expected").ThrowAsJavaScriptException();
        return;
    }

    NodeNabtoClient* c = Napi::ObjectWrap<NodeNabtoClient>::Unwrap(client.ToObject());
    client_ = c->getClient();


    Connection *co = Napi::ObjectWrap<Connection>::Unwrap(connection.ToObject());

    tcpTunnel_ = nabto_client_tcp_tunnel_new(co->getConnection());
}

TCPTunnel::~TCPTunnel() {
    nabto_client_tcp_tunnel_free(tcpTunnel_);
}

Napi::Value TCPTunnel::GetLocalPort(const Napi::CallbackInfo &info) 
{
    uint16_t localPort;
    int ec = nabto_client_tcp_tunnel_get_local_port(tcpTunnel_, &localPort);
    if (ec != NABTO_CLIENT_EC_OK) {
        Napi::Error::New(info.Env(), nabto_client_error_get_message(ec)).ThrowAsJavaScriptException();
        return info.Env().Undefined();
    }
    Napi::Number n = Napi::Number::New(info.Env(), localPort);
    return n;
}

Napi::Value TCPTunnel::Open(const Napi::CallbackInfo &info) 
{
    if (info.Length() != 2) {
        Napi::TypeError::New(info.Env(), "expects 2 arguments").ThrowAsJavaScriptException();
        return info.Env().Undefined();
    }

    Napi::Value service = info[0];
    Napi::Value localPort = info[1];

    if (!service.IsString()) {
        Napi::TypeError::New(info.Env(), "expects service to be a string").ThrowAsJavaScriptException();
        return info.Env().Undefined();
    }

    if (!localPort.IsNumber()) {
        Napi::TypeError::New(info.Env(), "expects localPort to be a number").ThrowAsJavaScriptException();
        return info.Env().Undefined();
    }

    auto ctx = new TCPTunnelOpenContext(client_, tcpTunnel_, service.ToString().Utf8Value(), localPort.ToNumber().Uint32Value(), info.Env());
    return ctx->Promise();
}

Napi::Value TCPTunnel::Close(const Napi::CallbackInfo &info) 
{
    auto ctx = new TCPTunnelCloseContext(client_, tcpTunnel_, info.Env());
    return ctx->Promise();
}