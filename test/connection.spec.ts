import 'mocha'
import { strict as assert } from 'node:assert';
import chai from 'chai';

import { NabtoClient, LogMessage, Connection } from '../src/NabtoClient/NabtoClient'
import { NabtoClientImpl } from '../src/NabtoClient/impl/NabtoClientImpl';

const expect = chai.expect;

const testDevice = {
    productId: "pr-fatqcwj9",
    deviceId: "de-avmqjaje",
    url: "https://pr-fatqcwj9.clients.nabto.net",
    key: "sk-72c860c244a6014248e64d5273e3e0ec",
    fp: "fcb78f8d53c67dbc4f72c36ca6cd2d5fc5592d584222059f0d76bdb514a9340c"
}

describe('Connection', () => {

    it('create destroy connections', () => {

        const client = new NabtoClientImpl();
        for (var i = 0; i < 10; i++) {
            const connection = client.createConnection();
        }
        client.stop();
    });

    it('set connection options', () => {
        const client = new NabtoClientImpl();
        const connection = client.createConnection();
        connection.setOptions({PrivateKey: client.createPrivateKey()});
        client.stop();
    });

    it('create connection and connect', async () => {
        const client = new NabtoClientImpl();
       //client.setLogLevel("trace");
      //  client.setLogCallback((logMessage: LogMessage) => {
      //      console.log(logMessage);
      //  });
        const connection = client.createConnection();
        connection.setOptions({PrivateKey: client.createPrivateKey(), ProductId: testDevice.productId, DeviceId: testDevice.deviceId, ServerKey: testDevice.key });
        await connection.connect();
        client.stop();
    })

    it('connect no channels', async () => {
        const client = new NabtoClientImpl();
       //client.setLogLevel("trace");
      //  client.setLogCallback((logMessage: LogMessage) => {
      //      console.log(logMessage);
      //  });
        const connection = client.createConnection();
        connection.setOptions({PrivateKey: client.createPrivateKey(), ProductId: "pr-foobar", DeviceId: "de-foobar", ServerKey: testDevice.key });
        try {
            await connection.connect();
            expect(true).to.be.false;
        } catch (err: any) {
            expect(err).to.have.property("code");
            expect(err.code).to.be.a('string').and.to.equal("NABTO_CLIENT_EC_NO_CHANNELS");
            expect(err).to.have.property("localError");
            expect(err.localError).to.have.property("code");
            expect(err.localError.code).to.be.a('string').and.to.equal("NABTO_CLIENT_EC_NOT_FOUND");
            expect(err).to.have.property("remoteError");
            expect(err.remoteError).to.have.property("code");
            expect(err.remoteError.code).to.be.a('string').and.to.equal("NABTO_CLIENT_EC_UNKNOWN_PRODUCT_ID");
            expect(err).to.have.property("directCandidateError");
            expect(err.directCandidateError).to.have.property("code");
            expect(err.directCandidateError.code).to.be.a('string').and.to.equal("NABTO_CLIENT_EC_NONE");
        }
        client.stop();
    })

    it('close connection', async () => {
        const client = new NabtoClientImpl();
        // client.setLogLevel("trace");
        // client.setLogCallback((logMessage: LogMessage) => {
        //     console.log(logMessage);
        // });
        const connection = client.createConnection();
        connection.setOptions({PrivateKey: client.createPrivateKey(), ProductId: testDevice.productId, DeviceId: testDevice.deviceId, ServerKey: testDevice.key });
        await connection.connect();
        await connection.close();
        client.stop();
    })

    it('create many parallel connections', async () => {
        const client = new NabtoClientImpl();
        //client.setLogLevel("trace");
      //  client.setLogCallback((logMessage: LogMessage) => {
      //      console.log(logMessage);
      //  });

        const connections : Array<Connection> = new Array<Connection>();
        const connectPromises : Array<Promise<void> > = new Array<Promise<void>>();
        const closePromises : Array<Promise<void> > = new Array<Promise<void>>();

        for (var i = 0; i < 10; i++) {
            const connection = client.createConnection();
            connection.setOptions({PrivateKey: client.createPrivateKey(), ProductId: testDevice.productId, DeviceId: testDevice.deviceId, ServerKey: testDevice.key });
            connections.push(connection);
        }

        connections.forEach(c => {
            connectPromises.push(c.connect());
        })

        await Promise.all(connectPromises);

        connections.forEach(c => {
            closePromises.push(c.close());
        })

        await Promise.all(closePromises);

        client.stop();
    })

});
