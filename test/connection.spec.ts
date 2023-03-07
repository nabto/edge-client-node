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