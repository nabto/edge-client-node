import { NabtoClient, LogMessage, ConnectionOptions } from './NabtoClient/NabtoClient'
import { NabtoClientImpl } from './NabtoClient/impl/NabtoClientImpl';
const axios = require('axios').default;

const tcpTunnelDevice : ConnectionOptions = { ProductId: "pr-fatqcwj9", DeviceId: "de-ijrdq47i", ServerConnectToken: "WzwjoTabnvux" };

async function manyConnections(n : number)
{
    const client = new NabtoClientImpl();
    //client.setLogLevel("trace");
    //client.setLogCallback((logMessage: LogMessage) => {
    //    console.log(`${logMessage.severity}: ${logMessage.message}`);
    //});

    for (var i = 0; i < n; i++) {

        console.log(`Connection ${i}`);
        const connection = client.createConnection();
        connection.setOptions({ PrivateKey: client.createPrivateKey() });
        connection.setOptions(tcpTunnelDevice);
        await connection.connect();

        const tunnel = connection.createTCPTunnel();
        await tunnel.open("http", 0);
        const localPort = tunnel.getLocalPort();

        const response = await axios.get(`http://127.0.0.1:${localPort}/`);

        await tunnel.close();
        await connection.close();

    }

    client.stop();

}

async function main() {

    const parallelOps : Array<Promise<void> > = new Array<Promise< void> >();

    for (var i = 0; i < 100; i++) {
        parallelOps.push(manyConnections(100));
    }

    await Promise.all(parallelOps);

}

const promise = main();
promise.then(() => {console.log("success")}, (e) => console.log(e));