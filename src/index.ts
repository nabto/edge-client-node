import { NabtoClient, LogMessage } from './NabtoClient/NabtoClient'
import { NabtoClientImpl } from './NabtoClient/impl/NabtoClientImpl';

const testDevice = {
    productId: "pr-fatqcwj9",
    deviceId: "de-avmqjaje",
    url: "https://pr-fatqcwj9.clients.nabto.net",
    key: "sk-72c860c244a6014248e64d5273e3e0ec",
    fp: "fcb78f8d53c67dbc4f72c36ca6cd2d5fc5592d584222059f0d76bdb514a9340c",
    ServerKey: "sk-72c860c244a6014248e64d5273e3e0ec"
}

async function main() {
    const client = new NabtoClientImpl();
    client.setLogLevel("trace");
    client.setLogCallback((logMessage: LogMessage) => {
        console.log(`${logMessage.severity}: ${logMessage.message}`);
    });

    const connection = client.createConnection();
    connection.setOptions({PrivateKey: client.createPrivateKey(), ProductId: testDevice.productId, DeviceId: testDevice.deviceId, ServerKey: testDevice.ServerKey});
    await connection.connect();

    const coapRequest = connection.createCoapRequest("GET", "/hello-world");
    const coapResponse = await coapRequest.execute();

    client.stop();

}

const promise = main();
promise.then(() => {console.log("success")}, (e) => console.log(e));