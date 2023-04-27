import 'mocha'
import { NabtoClient } from '../src/NabtoClient/NabtoClient'
import { NabtoClientImpl } from '../src/NabtoClient/impl/NabtoClientImpl';
import { streamDevice } from './test_devices';
import { expect } from 'chai';


describe('Streaming', () => {

  var client: NabtoClient;

  beforeEach(async () => {
      client = new NabtoClientImpl();
  })

  afterEach(async() => {
      client.stop();
  })

  it('create open stream', async () => {
    const connection = client.createConnection();
    client.setLogLevel("info");
    client.setLogCallback((logMessage) => {
        console.log(logMessage.message);
    });
    connection.setOptions(streamDevice);
    connection.setOptions({PrivateKey: client.createPrivateKey()});
    let data = "Hwllo World";
    let buf = new ArrayBuffer(data.length);
    let bufView = new Uint8Array(buf);
    for (let i = 0; i  < data.length; i++) {
      bufView[i] = data.charCodeAt(i);
    }
    await connection.connect();
    const stream = connection.createStream();
    await stream.open(42);
    await stream.write(buf);
    let echoBuf = await stream.readAll(data.length);
    let echoBufView = new Uint8Array(echoBuf);
//    expect(echoBuf).to.equal(buf);
    for (let i = 0; i < echoBuf.byteLength; i++) {
      expect(echoBufView[i]).to.equal(bufView[i]);
    }
    await stream.close();
    stream.abort();
  });
});
