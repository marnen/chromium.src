// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "components/bookmarks/common/android/bookmark_id.h"

#include "jni/BookmarkId_jni.h"

namespace bookmarks {
namespace android {

long JavaBookmarkIdGetId(JNIEnv* env, jobject obj) {
  return Java_BookmarkId_getId(env, obj);
}

int JavaBookmarkIdGetType(JNIEnv* env, jobject obj) {
  return Java_BookmarkId_getType(env, obj);
}

bool RegisterBookmarkId(JNIEnv* env) {
  return RegisterNativesImpl(env);
}

}  // namespace android
}  // namespace bookmarks
