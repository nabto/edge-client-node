{
  "targets": [
    {
      "target_name": "nabto_client",
      "include_dirs": [ 
        "<!(node -p \"require('node-addon-api').include_dir\")", 
        "./native_libraries/include" 
      ],
      "sources": [ "native_code/nabto_client.cc", "native_code/node_nabto_client.cc", "native_code/connection.cc", "native_code/coap.cc" ],
      'link_settings': {
        'libraries': [
            '-lnabto_client',
        ],
        'library_dirs': [
            '../native_libraries/lib/linux-x86_64',
        ]
      },
      "copies":[
            { 
                'destination': './build/Release',
                'files':[
                    './native_libraries/lib/linux-x86_64/libnabto_client.so'
                ]
            }
      ],
      "ldflags": [
        "-Wl,-rpath,./native_libraries/lib/linux-x86_64"
      ],
       'defines': [ 'NAPI_DISABLE_CPP_EXCEPTIONS' ]
    }
  ]
}