// Copyright 2013 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef CHROME_BROWSER_MEDIA_FAKE_DESKTOP_MEDIA_LIST_H_
#define CHROME_BROWSER_MEDIA_FAKE_DESKTOP_MEDIA_LIST_H_

#include <vector>

#include "chrome/browser/media/desktop_media_list.h"

class FakeDesktopMediaList : public DesktopMediaList {
 public:
  FakeDesktopMediaList();
  virtual ~FakeDesktopMediaList();

  void AddSource(int id);
  void RemoveSource(int index);
  void MoveSource(int old_index, int new_index);
  void SetSourceThumbnail(int index);
  void SetSourceName(int index, base::string16 name);

  // DesktopMediaList implementation:
  virtual void SetUpdatePeriod(base::TimeDelta period) override;
  virtual void SetThumbnailSize(const gfx::Size& thumbnail_size) override;
  virtual void SetViewDialogWindowId(
      content::DesktopMediaID::Id dialog_id) override;
  virtual void StartUpdating(DesktopMediaListObserver* observer) override;
  virtual int GetSourceCount() const override;
  virtual const Source& GetSource(int index) const override;

 private:
  std::vector<Source> sources_;
  DesktopMediaListObserver* observer_;
  gfx::ImageSkia thumbnail_;
};

#endif  // CHROME_BROWSER_MEDIA_FAKE_DESKTOP_MEDIA_LIST_H_
