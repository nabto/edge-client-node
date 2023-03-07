import 'mocha'
import { strict as assert } from 'node:assert';
import chai from 'chai';

import { NabtoClient, LogMessage } from '../src/NabtoClient/NabtoClient'
import { NabtoClientImpl } from '../src/NabtoClient/impl/NabtoClientImpl';
 
const expect = chai.expect;

const testDevice = {
    productId: "pr-fatqcwj9",
    deviceId: "de-avmqjaje",
    url: "https://pr-fatqcwj9.clients.nabto.net",
    key: "sk-72c860c244a6014248e64d5273e3e0ec",
    fp: "fcb78f8d53c67dbc4f72c36ca6cd2d5fc5592d584222059f0d76bdb514a9340c"
}

describe('Ligecycle', () => {

    it('test client create destroy', () => {

        const client = new NabtoClientImpl();
        const version = client.version();
        assert.equal(version, "5.12.0");

    });

    it('create private key', () => {
        const client = new NabtoClientImpl();
        const privateKey = client.createPrivateKey();
        assert(privateKey.startsWith("-----BEGIN EC PRIVATE"));
    })

    it('create connection', () => {
        const client = new NabtoClientImpl();
        const connection = client.createConnection();
        connection.setOptions({PrivateKey: client.createPrivateKey()});
    })

    it('invalid log level', () => {
        const client = new NabtoClientImpl();
        try {
            expect(client.setLogLevel("foobar")).not.to.throw(RangeError);
          } catch (err) {
            //console.log(err);
          }
    })

    it('test logging', () => {
        const client = new NabtoClientImpl();
        client.setLogLevel("trace");
        client.setLogCallback((logMessage: LogMessage) => {
            console.log(logMessage);
        });
        client.stop();
    })

    it('create connection and connect', async () => {
        const client = new NabtoClientImpl();
        client.setLogLevel("trace");
        client.setLogCallback((logMessage: LogMessage) => {
            console.log(logMessage);
        });
        const connection = client.createConnection();
        connection.setOptions({PrivateKey: client.createPrivateKey(), ProductId: testDevice.productId, DeviceId: testDevice.deviceId, ServerKey: testDevice.key });
        await connection.connect();
        client.stop();
    })

    it.only('Create coap request', async () => {
        const client = new NabtoClientImpl();
        client.setLogLevel("trace");
        client.setLogCallback((logMessage: LogMessage) => {
            console.log(`${logMessage.severity}: ${logMessage.message}`);
        });
        const connection = client.createConnection();
        connection.setOptions({PrivateKey: client.createPrivateKey(), ProductId: testDevice.productId, DeviceId: testDevice.deviceId, ServerKey: testDevice.key });
        await connection.connect();

        const coapRequest = connection.createCoapRequest("GET", "/hello-world");
        const coapResponse = await coapRequest.execute();

        assert.equal(coapResponse.getResponseStatusCode(), 205);
        assert.equal(coapResponse.getResponseContentFormat(), 0);
        assert.equal((new Buffer(coapResponse.getResponsePayload())).toString('utf8'), "Hello world");
        client.stop();
    });
})