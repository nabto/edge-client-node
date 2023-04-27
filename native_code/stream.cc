#include "connection.h"
#include "stream.h"
#include "node_nabto_client.h"

class OpenFutureContext: public FutureContext
{
public:
    OpenFutureContext(NabtoClient* client, Napi::Env env, NabtoClientStream* stream, uint32_t port): FutureContext(client, env)
    {
        nabto_client_stream_open(stream, future_, port);
        arm(false);
    }
};

class CloseFutureContext: public FutureContext
{
public:
    CloseFutureContext(NabtoClient* client, Napi::Env env, NabtoClientStream* stream): FutureContext(client, env)
    {
        nabto_client_stream_close(stream, future_);
        arm(false);
    }
};

class WriteFutureContext: public FutureContext
{
public:
    WriteFutureContext(NabtoClient* client, Napi::Env env, NabtoClientStream* stream, Napi::ArrayBuffer data): FutureContext(client, env)
    {
        nabto_client_stream_write(stream, future_, data.Data(), data.ByteLength());
        arm(false);
    }
};



Napi::Object Stream::Init(Napi::Env env, Napi::Object exports)
{
    Napi::Function func =
        DefineClass(env,
            "Stream",
            { InstanceMethod("open", &Stream::Open),
             InstanceMethod("readSome", &Stream::ReadSome),
             InstanceMethod("readAll", &Stream::ReadAll),
             InstanceMethod("write", &Stream::Write),
             InstanceMethod("close", &Stream::Close),
             InstanceMethod("getData", &Stream::GetData),
             InstanceMethod("abort", &Stream::Abort)
             });

    Napi::FunctionReference* constructor = new Napi::FunctionReference();
    *constructor = Napi::Persistent(func);
    env.SetInstanceData(constructor);

    exports.Set("Stream", func);
    return exports;
}


Stream::Stream(const Napi::CallbackInfo& info)
    : Napi::ObjectWrap<Stream>(info) {

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


    Connection* co = Napi::ObjectWrap<Connection>::Unwrap(connection.ToObject());


    stream_ = nabto_client_stream_new(co->getConnection());
}

Stream::~Stream()
{
    nabto_client_stream_free(stream_);
}


Napi::Value Stream::Open(const Napi::CallbackInfo& info)
{
    Napi::Env env = info.Env();

    int length = info.Length();
    if (length < 1 || !info[0].IsNumber())
    {
        Napi::TypeError::New(env, "Expected stream port").ThrowAsJavaScriptException();
        return Napi::Value();
    }
    OpenFutureContext* ofc = new OpenFutureContext(client_, info.Env(), stream_, info[0].ToNumber().Uint32Value());
    return ofc->Promise();
}

Napi::Value Stream::ReadSome(const Napi::CallbackInfo& info)
{
    this->reader_ = new ReadSomeFutureContext(client_, info.Env(), stream_);
    return this->reader_->Promise();
}

Napi::Value Stream::ReadAll(const Napi::CallbackInfo& info)
{
    Napi::Env env = info.Env();

    int length = info.Length();
    if (length < 1 || !info[0].IsNumber())
    {
        Napi::TypeError::New(env, "Expected read length").ThrowAsJavaScriptException();
        return Napi::Value();
    }
    this->reader_ = new ReadAllFutureContext(client_, info.Env(), stream_, info[0].ToNumber().Uint32Value());
    return this->reader_->Promise();
}

Napi::Value Stream::Write(const Napi::CallbackInfo& info)
{
    Napi::Env env = info.Env();

    int length = info.Length();
    if (length < 1 || !info[0].IsArrayBuffer())
    {
        Napi::TypeError::New(env, "Expected arguments format: ArrayBuffer").ThrowAsJavaScriptException();
        return Napi::Value();
    }

    Napi::ArrayBuffer buf = info[0].As<Napi::ArrayBuffer>();
    WriteFutureContext* wfc = new WriteFutureContext(client_, info.Env(), stream_, buf);
    return wfc->Promise();

}

Napi::Value Stream::Close(const Napi::CallbackInfo& info)
{
    CloseFutureContext* cfc = new CloseFutureContext(client_, info.Env(), stream_);
    return cfc->Promise();
}

Napi::Value Stream::GetData(const Napi::CallbackInfo& info)
{
    void* payload;
    size_t length;
    // TODO: handle reader_ not set and getBuffer returning false
    this->reader_->getBuffer(&payload, &length);
    Napi::ArrayBuffer buf = Napi::ArrayBuffer::New(info.Env(), payload, length);
    this->reader_ = nullptr;
    return buf;
}

void Stream::Abort(const Napi::CallbackInfo& info)
{
    return nabto_client_stream_abort(stream_);
}

