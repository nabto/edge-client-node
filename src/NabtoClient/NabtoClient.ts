import { NabtoClientImpl } from './impl/NabtoClientImpl';

/**
 * All promise rejections will throw an instance of this error.
 * code is the string representation of the underlying client SDK error
 * Error.message will be the error message from the underlying client SDK error.
 */
export class NabtoError extends Error {
    code?: string
    constructor(message?: string, code?: string) {
        super(message);
        this.code = code;
        Object.setPrototypeOf(this, NabtoError.prototype);
    }
}

/**
 * If a promise rejection from Connection.connect() has the code: `NABTO_CLIENT_EC_NO_CHANNELS` it will be an instance of this error.
 * remoteError is the error of the remote connect attempt
 * localError is the error of the local connect attempt
 * directCandidatesError is the error of the direct candidate connect attempt
 */
export class NabtoNoChannelsError extends NabtoError {
    remoteError?: NabtoError;
    localError?: NabtoError;
    directCandidatesError?: NabtoError;

    constructor(message?: string, code?: string, remoteError?: NabtoError, localError?: NabtoError, directCandidatesError?: NabtoError) {
        super(message, code);
        this.remoteError = remoteError;
        this.localError = localError;
        this.directCandidatesError = directCandidatesError;
        Object.setPrototypeOf(this, NabtoNoChannelsError.prototype);
    }
}

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
