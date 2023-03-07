import { NabtoClient, Connection, ConnectionOptions, LogMessage, CoapRequest, CoapResponse, TCPTunnel } from '../NabtoClient'
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
        this.coapRequest.setRequestPayload(contentFormat, payload);
    }

    async execute() : Promise<CoapResponse> {
        const result = await this.coapRequest.execute();
        return new CoapResponseImpl(this.coapRequest);
    }

    constructor(nabtoClient: any, connection: any, method: string, path: string) {
        this.coapRequest = new nabto_client.Coap(nabtoClient, connection, method, path);
    }
}

export class TCPTunnelImpl implements TCPTunnel {
    tcpTunnel: any;

    getLocalPort() : number {
        return this.tcpTunnel.getLocalPort();
    }
    open(service : string, localPort : number) : Promise<void> {
        return this.tcpTunnel.open(service, localPort);
    }

    close() : Promise<void> {
        return this.tcpTunnel.close();
    }
    constructor(nabtoClient: any, connection: any) {
        this.tcpTunnel = new nabto_client.TCPTunnel(nabtoClient, connection);
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

    close() : Promise<void> {
        return this.connection.close();
    }


    createCoapRequest(method: string, path: string) : CoapRequest {
        return new CoapRequestImpl(this.nabtoClient, this.connection, method, path);
    }

    createTCPTunnel() : TCPTunnel {
        return new TCPTunnelImpl(this.nabtoClient, this.connection);
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