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
#include "HTTPProxyConfig.h"
#include <boost/scoped_array.hpp>
#include <boost/filesystem/path.hpp>

class BrowseRoot {
public:
	enum PathType {
		PATH_VOLUME_ROOT,
		PATH_DOCUMENT_FOLDER
	};
	static const char* pathtype_names[];
	enum ContentType {
		CONTENT_TYPE_MIXED,
		CONTENT_TYPE_PHOTOS,
		CONTENT_TYPE_VIDEOS
	};
	static const char* contenttype_names[];
	
	BrowseRoot(const std::string& _name, const std::string& _full_path, PathType _p_type, ContentType _c_type = CONTENT_TYPE_MIXED) :
  name(_name), full_path(_full_path), p_type(_p_type), c_type(_c_type) {
		
	}
	virtual ~BrowseRoot() {}
	
	std::string convertToJSON() const;
	
	std::string name;
	std::string full_path;
	PathType p_type;
	ContentType c_type;
};

namespace Platform {
  // Implement these in platform_*.cpp
  std::string getArchitectureName();
  void unRegisterPlugins();
  //void registerPlugin(const std::string& plugin_path, const Version& version);
  bool pathIsHidden(const boost::filesystem::wpath& path_to_investigate);
  bool pathIsSystem(const boost::filesystem::wpath& path_to_investigate);
  bool pathIsShortcut(const boost::filesystem::wpath& path_to_investigate);
  boost::filesystem::wpath resolveShortcut(const boost::filesystem::wpath& path_to_shortcut);

  HTTPProxyConfig getSystemProxyConfig();
  
  // These functions defined in platform_common.cpp
  boost::filesystem::wpath resolveShortcutsInPath(const std::wstring& path_to_object);
  boost::filesystem::wpath resolveShortcutsInPath(const boost::filesystem::wpath& path_to_object);
  char* getRandomBytes(size_t bytecount); // caller delete[]s result.
  /*bool generateVideoThumbnail(const std::string& filename, const ImageProcessor::Dimension& dim, ImageProcessor::Result& res);*/
  
}; // end namespace platform
