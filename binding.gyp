{
  'targets': [
    {
        'target_name': 'mediainfolib',
        "cflags!": [ "-fno-exceptions" ],
        "cflags_cc!": [ "-fno-exceptions" ],
        'include_dirs': [
            'src',
            "<!@(node -p \"require('node-addon-api').include\")"
        ],
        'dependencies': ["<!(node -p \"require('node-addon-api').gyp\")"],
        'defines': [ 'NAPI_DISABLE_CPP_EXCEPTIONS' ],
        'sources': [
            'src/node-mediainfolib.cpp'
        ],
        'link_settings': {
            'libraries': [
            '-lmediainfo'
            ]
        }
    }
  ]
}