{
  'targets': [
    {
        'target_name': 'mediainfolib',
        "cflags!": [ "-fno-exceptions" ],
        "cflags_cc!": [ "-fno-exceptions" ],
        'sources': [
            'src/node-mediainfolib.cpp'
        ],
        'include_dirs': ["<!@(node -p \"require('node-addon-api').include\")"],
        'dependencies': ["<!(node -p \"require('node-addon-api').gyp\")"],
        'conditions': [
            ['OS=="win"', {
                "msvs_settings": {
                    "VCCLCompilerTool": {
                        "ExceptionHandling": 1
                        }
                    }
            }],
            ['OS=="mac"', {
                "xcode_settings": {
                    "CLANG_CXX_LIBRARY": "libc++",
                    'GCC_ENABLE_CPP_EXCEPTIONS': 'YES',
                    'MACOSX_DEPLOYMENT_TARGET': '10.7'
                },
                'link_settings': {
                    'libraries': [
                    '-lmediainfo'
                    ]
                }
            }],
            ['OS=="linux"', {
                'link_settings': {
                    'libraries': [
                    '-lmediainfo'
                    ]
                }
            }]
        ]
    }
  ]
}