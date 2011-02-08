/*
 *  Platform_linux.cpp
 *  fbplugin
 *
 */

#include <stdlib.h>
#include <mntent.h>
#include <string>
#include "Platform.h"
#include "PluginJSDict.h"
#include <boost/format.hpp>

using std::string;

std::string Platform::getArchitectureName() {
  #if defined(__x86_64)
    return "linux-x86_64";
  #elif defined(__i386)
    return "linux-x86";
  #else
    #error Implement Platform::getArchitectureName() please
  #endif
}

PluginJSDict* Platform::getLocalBrowseRoots() {
    PluginJSDict* d = new PluginJSDict;
  (*d)["/"] = new BrowseRoot("/", "/", BrowseRoot::PATH_VOLUME_ROOT);
  const char* homeenv = getenv("HOME");
  if (homeenv) (*d)["~"] = new BrowseRoot(homeenv, homeenv, BrowseRoot::PATH_DOCUMENT_FOLDER);

  FILE* mfp = setmntent(MOUNTED, "r");
  if (mfp) {
    struct mntent res;
    const size_t pbuf_size = 1024;
    char pbuf[pbuf_size];

    while (getmntent_r(mfp, &res, pbuf, pbuf_size)) {
      // mnt_fsname mnt_dir mnt_type
      // ignore mounts that don't represent a device
      if (res.mnt_fsname[0] == '\0') continue;
      if (strcmp(res.mnt_fsname, "none")==0) continue;
      // and some common types that we shouldn't care about
      if (strcmp(res.mnt_type, "swap")==0) continue;
      if (strcmp(res.mnt_type, "devpts")==0) continue;
      if (strcmp(res.mnt_type, "sysfs")==0) continue;
      if (strcmp(res.mnt_type, "devfs")==0) continue;
      if (strcmp(res.mnt_type, "tmpfs")==0) continue;
      if (strcmp(res.mnt_type, "proc")==0) continue;
      if (strcmp(res.mnt_type, "binfmt_misc")==0) continue;
      if (strcmp(res.mnt_type, "fuse.gvfs-fuse-daemon")==0) continue;

      (*d)[res.mnt_dir] = new BrowseRoot(str(boost::format("%1% (%2%)") % res.mnt_dir % res.mnt_fsname), res.mnt_dir, BrowseRoot::PATH_VOLUME_ROOT);
    }
    endmntent(mfp);
  }

  return d;
}

// no plugin registration on linux
void Platform::unRegisterPlugins() {

}

void Platform::registerPlugin(const std::string& plugin_path, const Version& version) {

}

bool Platform::pathIsHidden(const boost::filesystem::wpath& path_to_investigate) {
  // Same in Linux and Mac FYI
  std::wstring fn = path_to_investigate.filename();
  return (fn.empty() || fn[0] == L'.');
}

bool Platform::pathIsSystem(const boost::filesystem::wpath& path_to_investigate) {
  return false;
}

bool Platform::pathIsShortcut(const boost::filesystem::wpath& path_to_investigate) {
  return false;
}

boost::filesystem::wpath Platform::resolveShortcut(const boost::filesystem::wpath& path_to_shortcut) {
  return path_to_shortcut;
}

// TODO: STUB
HTTPProxyConfig Platform::getSystemProxyConfig() {
  return HTTPProxyConfig();
}

