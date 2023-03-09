import { NabtoClientImpl } from './impl/NabtoClientImpl';


export interface ConnectionOptions {
    Local?: boolean,
    Remote?: boolean,
    PrivateKey?: string,
    ProductId?: string,
    DeviceId?: string,
    ServerConnectToken?: string,
    ServerKey?: string

}

export interface LogMessage {
    message: string,
    severity: string
}

export interface CoapResponse {
    getResponseContentFormat() : number;
    getResponseStatusCode() : number;
    getResponsePayload() : ArrayBuffer;
}

export interface CoapRequest {
    setRequestPayload(contentFormat: number, content: ArrayBuffer) : void;
    execute() : Promise<CoapResponse>;
}

export interface TCPTunnel {
    getLocalPort() : number;
    open(service : string, localPort : number) : Promise<void>;
    close() : Promise<void>;
}

export interface Connection {
    setOptions(options: ConnectionOptions) : void;
    getClientFingerprint() : string;
    connect() : Promise<void>;
    close() : Promise<void>;
    createCoapRequest(method: string, path: string): CoapRequest;
    createTCPTunnel(): TCPTunnel;
}

export abstract class NabtoClient {
    static create() : NabtoClient {
        return new NabtoClientImpl();
    }
    abstract version() : string;
    abstract createPrivateKey(): string;
    abstract setLogLevel(logLevel: string) : void;
    abstract setLogCallback(callback: (message: LogMessage) => void) : void;
    abstract stop() : void;
    abstract createConnection() : Connection;
}