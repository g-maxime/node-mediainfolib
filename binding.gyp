{
  'targets': [
    {
        'target_name': 'mediainfolib',
        'include_dirs': [
            'src'
        ],
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