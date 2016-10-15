{
    "targets": [
        {
            "target_name": "U2fDevices",
            "sources": [
                "cpp/u2f-devices.cc",
                "cpp/devices_mac.cc",
                "cpp/devices_win.cc",
                "cpp/devices_linux.cc"
            ],
            "include_dirs" : [
 	 			"<!(node -e \"require('nan')\")"
			],
            'conditions': [
                [ 'OS=="mac"', {
                    'LDFLAGS': [
                        '-framework IOKit',
                        '-framework CoreFoundation'
                    ],
                    'xcode_settings': {
                        'GCC_ENABLE_CPP_EXCEPTIONS': 'YES',
                        'OTHER_LDFLAGS': [
                            '-framework IOKit',
                            '-framework CoreFoundation'
                        ],
                    }
                }],
                [ 'OS=="linux"', {
                }],
                [ 'OS=="win"', {
                }]
            ],
        }
    ],
}
