#include <napi.h>
#include "connection.h"
#include "node_nabto_client.h"

#include <nabto/nabto_client.h>

Napi::Object Connection::Init(Napi::Env env, Napi::Object exports)
{
    Napi::Function func =
        DefineClass(env,
                    "Connection",
                    {InstanceMethod("setOptions", &Connection::SetOptions),
                     InstanceMethod("connect", &Connection::Connect),
                     InstanceMethod("close", &Connection::Close),
                     InstanceMethod("getClientFingerprint", &Connection::GetClientFingerprint)});

    Napi::FunctionReference *constructor = new Napi::FunctionReference();
    *constructor = Napi::Persistent(func);
    env.SetInstanceData(constructor);

    exports.Set("Connection", func);
    return exports;
}

Connection::Connection(const Napi::CallbackInfo &info)
    : Napi::ObjectWrap<Connection>(info)
{
    Napi::Env env = info.Env();

    int length = info.Length();
    if (length <= 0 || !info[0].IsObject())
    {
        Napi::TypeError::New(env, "Nabto Client object expected").ThrowAsJavaScriptException();
        return;
    }

    NodeNabtoClient *client = Napi::ObjectWrap<NodeNabtoClient>::Unwrap(info[0].ToObject());

    client_ = client->getClient();
    connection_ = nabto_client_connection_new(client->getClient());
}

Connection::~Connection()
{
    nabto_client_connection_free(connection_);
}

Napi::Value Connection::GetClientFingerprint(const Napi::CallbackInfo &info)
{
    char *fp;
    NabtoClientError ec = nabto_client_connection_get_client_fingerprint(connection_, &fp);

    if (ec != 0)
    {
        Napi::Error::New(info.Env(), nabto_client_error_get_message(ec)).ThrowAsJavaScriptException();
        return info.Env().Undefined();
    }

    Napi::Value retVal = Napi::String::New(info.Env(), fp);

    nabto_client_string_free(fp);
    return retVal;
}

void Connection::SetOptions(const Napi::CallbackInfo &info)
{
    int length = info.Length();
    Napi::Value result;
    if (length <= 0 || !info[0].IsString())
    {
        Napi::TypeError::New(info.Env(), "String expected").ThrowAsJavaScriptException();
        return;
    }

    nabto_client_connection_set_options(connection_, info[0].ToString().Utf8Value().c_str());
}

Napi::Value Connection::Connect(const Napi::CallbackInfo &info)
{
    ConnectionConnectContext *ccc = new ConnectionConnectContext(client_, connection_, info.Env());
    return ccc->Promise();
}

Napi::Value Connection::Close(const Napi::CallbackInfo &info)
{
    ConnectionCloseContext *ccc = new ConnectionCloseContext(client_, connection_, info.Env());
    return ccc->Promise();
}