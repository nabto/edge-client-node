#include "coap.h"
#include "connection.h"
#include "node_nabto_client.h"

Napi::Object Coap::Init(Napi::Env env, Napi::Object exports)
{
    Napi::Function func =
        DefineClass(env,
                    "Coap",
                    {InstanceMethod("setRequestPayload", &Coap::SetRequestPayload),
                     InstanceMethod("execute", &Coap::Execute),
                     InstanceMethod("getResponsePayload", &Coap::GetResponsePayload),
                     InstanceMethod("getResponseStatusCode", &Coap::GetResponseStatusCode),
                     InstanceMethod("getResponseContentFormat", &Coap::GetResponseContentFormat)});

    Napi::FunctionReference *constructor = new Napi::FunctionReference();
    *constructor = Napi::Persistent(func);
    env.SetInstanceData(constructor);

    exports.Set("Coap", func);
    return exports;
}

Coap::Coap(const Napi::CallbackInfo &info)
    : Napi::ObjectWrap<Coap>(info)
{
    Napi::Env env = info.Env();

    int length = info.Length();
    if (length < 4)
    {
        Napi::TypeError::New(env, "Expects 4 arguments client, connection, method and path").ThrowAsJavaScriptException();
    }
    Napi::Value client = info[0];
    Napi::Value connection = info[1];
    Napi::Value method = info[2];
    Napi::Value path = info[3];

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

    if (!method.IsString())
    {
        Napi::TypeError::New(env, "method is expected to be a string").ThrowAsJavaScriptException();
        return;
    }

    if (!path.IsString())
    {
        Napi::TypeError::New(env, "path is expected to be a string").ThrowAsJavaScriptException();
        return;
    }

    NodeNabtoClient* c = Napi::ObjectWrap<NodeNabtoClient>::Unwrap(client.ToObject());
    client_ = c->getClient();


    Connection *co = Napi::ObjectWrap<Connection>::Unwrap(connection.ToObject());

    coap_ = nabto_client_coap_new(co->getConnection(), method.ToString().Utf8Value().c_str(), path.ToString().Utf8Value().c_str());
}

Coap::~Coap() {
    nabto_client_coap_free(coap_);
}

void Coap::SetRequestPayload(const Napi::CallbackInfo &info)
{
    if (info.Length() != 2) {
        Napi::TypeError::New(info.Env(), "expects two argument contentFormat and bytes").ThrowAsJavaScriptException();
        return;
    }

    if (!info[0].IsNumber())
    {
        Napi::TypeError::New(info.Env(), "Expected a number")
            .ThrowAsJavaScriptException();
        return;
    }

    if (!info[1].IsArrayBuffer())
    {
        Napi::TypeError::New(info.Env(), "Expected an ArrayBuffer")
            .ThrowAsJavaScriptException();
        return;
    }

    Napi::Number contentFormat = info[0].ToNumber();
    Napi::ArrayBuffer buf = info[1].As<Napi::ArrayBuffer>();

    int ec = nabto_client_coap_set_request_payload(coap_, contentFormat.Uint32Value(), buf.Data(), buf.ByteLength());
    if (ec != NABTO_CLIENT_EC_OK) {
        Napi::Error::New(info.Env(), nabto_client_error_get_message(ec)).ThrowAsJavaScriptException();
        return;
    }
}

Napi::Value Coap::GetResponsePayload(const Napi::CallbackInfo &info) 
{
    void* payload;
    size_t payloadLength;

    int ec = nabto_client_coap_get_response_payload(coap_, &payload, &payloadLength);
    if (ec != NABTO_CLIENT_EC_OK) {
        Napi::Error::New(info.Env(), nabto_client_error_get_message(ec)).ThrowAsJavaScriptException();
        return info.Env().Undefined();
    }
    Napi::ArrayBuffer buf = Napi::ArrayBuffer::New(info.Env(), payload, payloadLength);
    return buf;
}

Napi::Value Coap::GetResponseStatusCode(const Napi::CallbackInfo &info) {
    uint16_t statusCode;
    int ec = nabto_client_coap_get_response_status_code(coap_, &statusCode);
    if (ec != NABTO_CLIENT_EC_OK) {
        Napi::Error::New(info.Env(), nabto_client_error_get_message(ec)).ThrowAsJavaScriptException();
        return info.Env().Undefined();
    }
    Napi::Number n = Napi::Number::New(info.Env(), statusCode);
    return n;
}

Napi::Value Coap::GetResponseContentFormat(const Napi::CallbackInfo &info) {
    uint16_t contentFormat;
    int ec = nabto_client_coap_get_response_content_format(coap_, &contentFormat);
    if (ec != NABTO_CLIENT_EC_OK) {
        Napi::Error::New(info.Env(), nabto_client_error_get_message(ec)).ThrowAsJavaScriptException();
        return info.Env().Undefined();
    }
    Napi::Number n = Napi::Number::New(info.Env(), contentFormat);
    return n;
}

Napi::Value Coap::Execute(const Napi::CallbackInfo &info) 
{
    CoapExecuteContext* ctx = new CoapExecuteContext(client_, coap_, info.Env());
    return ctx->Promise();
}