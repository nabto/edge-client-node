import 'mocha'

import { NabtoClient } from '../src/NabtoClient/NabtoClient'
import { NabtoClientImpl } from '../src/NabtoClient/impl/NabtoClientImpl'

describe('Ligecycle', () => {

    it('test client create destroy', () => {

        const client = new NabtoClientImpl();
        const version = client.version();

    });

})