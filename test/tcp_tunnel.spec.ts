import 'mocha'
import { strict as assert } from 'node:assert';
import chai from 'chai';

import { NabtoClient, LogMessage, Connection } from '../src/NabtoClient/NabtoClient'
import { NabtoClientImpl } from '../src/NabtoClient/impl/NabtoClientImpl';

import { tcpTunnelDevice } from './test_devices';

const axios = require('axios').default;

describe('TCPTunnel', () => {

    var client: NabtoClient;

    beforeEach(async () => {
        client = new NabtoClientImpl();
    })

    afterEach(async() => {
        client.stop();
    })

    it('create destroy tunnel', async () => {        
        const connection = client.createConnection();
        connection.setOptions(tcpTunnelDevice);
        connection.setOptions({PrivateKey: client.createPrivateKey()});

        await connection.connect();
        const tunnel = connection.createTCPTunnel();
    });

    it('open tunnel', async () => {
        const connection = client.createConnection();
        connection.setOptions(tcpTunnelDevice);
        connection.setOptions({PrivateKey: client.createPrivateKey()});

        await connection.connect();
        const tunnel = connection.createTCPTunnel();
        await tunnel.open("http", 0);

        const localPort = tunnel.getLocalPort();
        await tunnel.close();
    });

    it('tunnel invoke http request', async () => {        
        const connection = client.createConnection();
        connection.setOptions(tcpTunnelDevice);
        connection.setOptions({PrivateKey: client.createPrivateKey()});

        await connection.connect();
        const tunnel = connection.createTCPTunnel();
        await tunnel.open("http", 0);

        const localPort = tunnel.getLocalPort();

        let response = await axios.get(`http://127.0.0.1:${localPort}/`);
        await tunnel.close();
    });   
});