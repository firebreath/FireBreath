/*
 *  Platform.h
 *  fbplugin
 *
 *  Definitions for platform-specific functions
 *
 */

#pragma once
#include "windows_defs.h"
#include <string>
#include <list>
#include "../HTTPCommon/HTTPProxyConfig.h"
#include <boost/scoped_array.hpp>
#include <boost/filesystem/path.hpp>

namespace HTTP { namespace Platform {
  // Implement these in platform_*.cpp
  std::string getArchitectureName();
  void unRegisterPlugins();
  //void registerPlugin(const std::string& plugin_path, const Version& version);
  bool pathIsHidden(const boost::filesystem::wpath& path_to_investigate);
  bool pathIsSystem(const boost::filesystem::wpath& path_to_investigate);
  bool pathIsShortcut(const boost::filesystem::wpath& path_to_investigate);
  boost::filesystem::wpath resolveShortcut(const boost::filesystem::wpath& path_to_shortcut);

  HTTP::HTTPProxyConfig getSystemProxyConfig();
  
  // These functions defined in platform_common.cpp
  boost::filesystem::wpath resolveShortcutsInPath(const std::wstring& path_to_object);
  boost::filesystem::wpath resolveShortcutsInPath(const boost::filesystem::wpath& path_to_object);
  char* getRandomBytes(size_t bytecount); // caller delete[]s result.
  /*bool generateVideoThumbnail(const std::string& filename, const ImageProcessor::Dimension& dim, ImageProcessor::Result& res);*/
  
}; }; // end namespace platform

