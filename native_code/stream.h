#pragma once

#include <napi.h>
#include <nabto/nabto_client.h>
#include "future.h"

class ReadFutureContext: public FutureContext
{
public:
    ReadFutureContext(NabtoClient* client, Napi::Env env, NabtoClientStream* stream): FutureContext(client, env)
    {
        stream_ = stream;
    }

    ~ReadFutureContext()
    {
        free(readBuffer_);
    }

    bool getBuffer(void** buf, size_t* len)
    {
        *buf = readBuffer_;
        *len = readLength_;
        return true;
    }

protected:
    NabtoClientStream* stream_;
    void* readBuffer_;
    size_t readLength_;
};

class ReadSomeFutureContext: public ReadFutureContext
{
public:
    ReadSomeFutureContext(NabtoClient* client, Napi::Env env, NabtoClientStream* stream): ReadFutureContext(client, env, stream)
    {
        readBuffer_ = calloc(1, 1024);
        nabto_client_stream_read_some(stream, future_, (void*)readBuffer_, 1024, &readLength_);
        arm(false);
    }
};

class ReadAllFutureContext: public ReadFutureContext
{
public:
    ReadAllFutureContext(NabtoClient* client, Napi::Env env, NabtoClientStream* stream, size_t length): ReadFutureContext(client, env, stream)
    {
        readBuffer_ = calloc(1, length);
        nabto_client_stream_read_all(stream, future_, (void*)readBuffer_, length, &readLength_);
        arm(false);
    }
};



class Stream : public Napi::ObjectWrap<Stream>
{
public:
    static Napi::Object Init(Napi::Env env, Napi::Object exports);
    Stream(const Napi::CallbackInfo& info);
    ~Stream();

    Napi::Value Open(const Napi::CallbackInfo& info);
    Napi::Value ReadSome(const Napi::CallbackInfo& info);
    Napi::Value ReadAll(const Napi::CallbackInfo& info);
    Napi::Value Write(const Napi::CallbackInfo& info);
    Napi::Value Close(const Napi::CallbackInfo& info);
    void Abort(const Napi::CallbackInfo& info);

    Napi::Value GetData(const Napi::CallbackInfo& info);


private:
    NabtoClient* client_;
    NabtoClientStream* stream_;
    ReadFutureContext* reader_;


};
