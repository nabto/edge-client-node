import { NabtoClient, Connection, ConnectionOptions, LogMessage, CoapRequest, CoapResponse } from '../NabtoClient'
var nabto_client = require('bindings')('nabto_client');

export class CoapResponseImpl implements CoapResponse {
    coapRequest: any;
    getResponsePayload() : ArrayBuffer {
        return this.coapRequest.getResponsePayload();
    }
    getResponseStatusCode(): number {
        return this.coapRequest.getResponseStatusCode();
    }
    getResponseContentFormat(): number {
        return this.coapRequest.getResponseContentFormat();
    }
    constructor(coapRequest: any) {
        this.coapRequest = coapRequest;
    }
}

export class CoapRequestImpl implements CoapRequest {
    coapRequest: any;
    setRequestPayload(contentFormat: number, payload: ArrayBuffer) : void {
        
    }

    async execute() : Promise<CoapResponse> {
        const result = await this.coapRequest.execute();
        return new CoapResponseImpl(this.coapRequest);
    }

    constructor(nabtoClient: any, connection: any, method: string, path: string) {
        this.coapRequest = new nabto_client.Coap(nabtoClient, connection, method, path);
    }
}

export class ConnectionImpl implements Connection {
    connection: any;
    nabtoClient: any;

    setOptions(options: ConnectionOptions) {
        this.connection.setOptions(JSON.stringify(options));
    }

    getClientFingerprint() : string {
        return this.connection.getClientFingerprint();
    }

    connect() : Promise<void> {
        return this.connection.connect();
    }

    createCoapRequest(method: string, path: string) : CoapRequest {
        return new CoapRequestImpl(this.nabtoClient, this.connection, method, path);
    }

    constructor(client: any) {
        this.connection = new nabto_client.Connection(client);
        this.nabtoClient = client;
    }
}

export class NabtoClientImpl implements NabtoClient {

    nabtoClient: any;

    version() : string {
        return this.nabtoClient.getVersion();
    }

    createPrivateKey() : string {
        return this.nabtoClient.createPrivateKey();
    }

    createConnection() : Connection {
        return new ConnectionImpl(this.nabtoClient);
    }

    setLogLevel(logLevel: string) {
        this.nabtoClient.setLogLevel(logLevel);
    }
    
    setLogCallback(callback: (logMessage: LogMessage) => void)
    {
        this.nabtoClient.setLogCallback(callback);
    }

    stop() : void {
        this.nabtoClient.stop();
    }

    constructor() {
        this.nabtoClient = new nabto_client.NabtoClient();
    }
}