# Edge Client Node

Warning: This wrapper is not yet production ready, use it at your own risk.

Node JS wrapper for Nabto Edge Client.


## Notes

Below usage has caused issues if typescript is not a dependency of the package you are installing it into. We should clearify if typescript should be a dependency, or if ts-node should be elevated from dev-dependency or what is what.

## Usage

This package can be installed directly from github using npm:

```
npm install --save git@github.com:nabto/edge-client-node.git
```

This references the main branch directly, however, when installed, the package is given an entry in the `package-lock.json` file where it is fixed to a specific commit. This means if this repo is updated, rerunning `npm install` will not update this package. Either remove the `package-lock.json` entry, or update the package explicitly.

This package provides both a commonJs module and an ESM module as well as typescript types for both.

To use the client, import the `NabtoClientFactory` and create a client:


```
import { NabtoClientFactory } from 'edge-client-node'

let client = NabtoClientFactory.create();
let key = client.createPrivateKey();
let connection = client.createConnection();
connection.setOptions({ProductId: "pr-SomeProduct", DeviceId: "de-SomeDevice", PrivateKey: key});
await conn.connect();
```
