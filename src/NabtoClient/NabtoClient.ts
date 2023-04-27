import { NabtoClientImpl } from './impl/NabtoClientImpl';


export interface ConnectionOptions {
    Local?: boolean,
    Remote?: boolean,
    PrivateKey?: string,
    ProductId?: string,
    DeviceId?: string,
    ServerConnectToken?: string,
    ServerKey?: string,
    ServerUrl?: string,
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

export interface Stream {
    open(streamPort: number): Promise<void>;
    readSome(): Promise<ArrayBuffer>;
    readAll(length: number): Promise<ArrayBuffer>;
    write(data: ArrayBuffer): Promise<void>;
    close(): Promise<void>;
    abort(): void;
}

export interface Connection {
    setOptions(options: ConnectionOptions) : void;
    getClientFingerprint() : string;
    connect() : Promise<void>;
    close() : Promise<void>;
    createCoapRequest(method: string, path: string): CoapRequest;
    createTCPTunnel(): TCPTunnel;
    createStream(): Stream;
}

export interface NabtoClient {
    version(): string;
    createPrivateKey(): string;
    setLogLevel(logLevel: string): void;
    setLogCallback(callback: (message: LogMessage) => void): void;
    stop(): void;
    createConnection(): Connection;
}

export class NabtoClientFactory {
    static create(): NabtoClient {
        return new NabtoClientImpl();
    }
}
