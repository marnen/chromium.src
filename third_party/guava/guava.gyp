# Copyright (c) 2012 The Chromium Authors. All rights reserved.
# Use of this source code is governed by a BSD-style license that can be
# found in the LICENSE file.

{
  'targets': [
    {
      'target_name': 'guava_javalib',
      'type': 'none',
      'variables': {
        'package_name': '<(_target_name)',
        'java_in_dir': 'src/guava',
      },
      'dependencies': [
        '../../third_party/jsr-305/jsr-305.gyp:jsr_305_javalib',
      ],
      'includes': [ '../../build/java.gypi' ],
    },
  ],
}
