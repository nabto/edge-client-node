
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

export interface Connection {
    setOptions(options: ConnectionOptions) : void;
    getClientFingerprint() : string;
    connect() : Promise<void>;
    createCoapRequest(method: string, path: string): CoapRequest;
}

export interface NabtoClient {
    version() : string;
    createPrivateKey(): string;
    setLogLevel(logLevel: string) : void;
    setLogCallback(callback: (message: LogMessage) => void) : void;
    stop() : void;
    //createConnection() : Connection;
}