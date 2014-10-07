// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef CHROME_BROWSER_MEDIA_GALLERIES_GALLERY_WATCH_MANAGER_H_
#define CHROME_BROWSER_MEDIA_GALLERIES_GALLERY_WATCH_MANAGER_H_

#include <map>
#include <string>

#include "base/basictypes.h"
#include "base/callback_forward.h"
#include "base/files/file_path.h"
#include "base/files/file_path_watcher.h"
#include "base/memory/linked_ptr.h"
#include "base/memory/scoped_ptr.h"
#include "base/memory/weak_ptr.h"
#include "base/time/time.h"
#include "chrome/browser/media_galleries/media_galleries_preferences.h"
#include "components/storage_monitor/removable_storage_observer.h"

class GalleryWatchManagerObserver;

namespace content {
class BrowserContext;
}

namespace extensions {
class Extension;
}

// The GalleryWatchManager is owned by MediaFileSystemRegistry, which is global.
// This class manages all watches on media galleries, regardless of profile.
// It tracks outstanding watch requests and creates one FilePathWatcher per
// watched directory. This class lives and is called on the UI thread.
class GalleryWatchManager
    : public MediaGalleriesPreferences::GalleryChangeObserver,
      public storage_monitor::RemovableStorageObserver {
 public:
  // On success, |error| is empty.
  typedef base::Callback<void(const std::string& /* error */)> ResultCallback;

  static const char kInvalidGalleryIDError[];
  static const char kNoPermissionError[];
  static const char kCouldNotWatchGalleryError[];

  GalleryWatchManager();
  virtual ~GalleryWatchManager();

  // Add or remove observer of change events - this is the only way to
  // get the result of the file watches. There can only be one observer per
  // browser context.
  void AddObserver(content::BrowserContext* browser_context,
                   GalleryWatchManagerObserver* observer);
  void RemoveObserver(content::BrowserContext* browser_context);

  // Must be called when |browser_context| is shut down.
  void ShutdownBrowserContext(content::BrowserContext* browser_context);

  // Add a watch for |gallery_id|.
  void AddWatch(content::BrowserContext* browser_context,
                const extensions::Extension* extension,
                MediaGalleryPrefId gallery_id,
                const ResultCallback& callback);

  // Remove the watch for |gallery_id|. It is valid to call this method on
  // non-existent watches.
  void RemoveWatch(content::BrowserContext* browser_context,
                   const std::string& extension_id,
                   MediaGalleryPrefId gallery_id);

  // Remove all the watches for |extension_id|.
  void RemoveAllWatches(content::BrowserContext* browser_context,
                        const std::string& extension_id);

  // Return the set of galleries being watched for |extension_id|.
  MediaGalleryPrefIdSet GetWatchSet(content::BrowserContext* browser_context,
                                    const std::string& extension_id);

 private:
  class FileWatchManager;

  // Used to track the gallery watches connected to a specific path.
  struct WatchOwner {
    WatchOwner(content::BrowserContext* browser_context,
               const std::string& extension_id,
               MediaGalleryPrefId gallery_id);

    content::BrowserContext* browser_context;
    const std::string extension_id;
    MediaGalleryPrefId gallery_id;

    // Needed to support storage in STL set, as well as usage as map key.
    bool operator<(const WatchOwner& other) const;
  };

  struct NotificationInfo {
    NotificationInfo();
    ~NotificationInfo();

    std::set<WatchOwner> owners;
    base::Time last_notify_time;
    bool delayed_notification_pending;
  };

  typedef std::map<WatchOwner, base::FilePath> WatchesMap;
  typedef std::map<base::FilePath, NotificationInfo> WatchedPaths;
  typedef std::map<content::BrowserContext*, GalleryWatchManagerObserver*>
      ObserverMap;

  // Stop the FilePathWatcher for |path|. Updates |watched_paths_| but not
  // |registered_watches_|.
  void DeactivateFileWatch(const WatchOwner& owner, const base::FilePath& path);

  // Called by FilePathWatcher on the UI thread to respond to a request to
  // watch the path.
  void OnFileWatchActivated(const WatchOwner& owner,
                            const base::FilePath& path,
                            const ResultCallback& callback,
                            bool success);

  // Called by FilePathWatcher on the UI thread on a change event for |path|.
  void OnFilePathChanged(const base::FilePath& path, bool error);

  // MediaGalleriesPreferences::GalleryChangeObserver implementation.
  virtual void OnPermissionRemoved(MediaGalleriesPreferences* pref,
                                   const std::string& extension_id,
                                   MediaGalleryPrefId pref_id) override;
  virtual void OnGalleryRemoved(MediaGalleriesPreferences* pref,
                                MediaGalleryPrefId pref_id) override;

  // storage_monitor::RemovableStorageObserver implementation.
  virtual void OnRemovableStorageDetached(
      const storage_monitor::StorageInfo& info) override;

  // True if the we are already observing the storage monitor.
  bool storage_monitor_observed_;

  // MediaGalleriesPreferences we are currently observing.
  std::set<MediaGalleriesPreferences*> observed_preferences_;

  // All registered watches, keyed by WatchOwner.
  WatchesMap watches_;

  // Reverse mapping of watched paths to the set of owning WatchOwners.
  WatchedPaths watched_paths_;

  // Things that want to hear about gallery changes.
  ObserverMap observers_;

  // Helper that does the watches on the FILE thread.
  scoped_ptr<FileWatchManager> watch_manager_;

  base::WeakPtrFactory<GalleryWatchManager> weak_factory_;

  DISALLOW_COPY_AND_ASSIGN(GalleryWatchManager);
};

#endif  // CHROME_BROWSER_MEDIA_GALLERIES_GALLERY_WATCH_MANAGER_H_
