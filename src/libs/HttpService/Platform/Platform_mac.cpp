/*
 *  platform_mac.cpp
 *  fbplugin
 *
 */

#include <stdlib.h>
#include <dlfcn.h>
#include <CoreServices/CoreServices.h>
#include <CoreFoundation/CoreFoundation.h>
#include <ApplicationServices/ApplicationServices.h>
#include <SystemConfiguration/SystemConfiguration.h>
#include <SystemConfiguration/SCDynamicStore.h>
#include "cf_scoped_ptr.h"
#include "Platform.h"
#include "mac_common.h"
#include <sys/xattr.h>
#include "APITypes.h"

using std::string;

using namespace HTTP;

std::string Platform::getArchitectureName() {
  return "macosx";
}

/*
string findUserFolder(OSType folderType) {
    FSRef ref;
    OSErr err = FSFindFolder(kUserDomain, folderType, kDontCreateFolder, &ref);
    if (err != noErr) return string();
    char path[1024];
    err = FSRefMakePath(&ref, (UInt8*)path, 1024);
    if (err != noErr) return string();
    return string(path);
}
*/

/*PluginJSDict* Platform::getLocalBrowseRoots() {
    PluginJSDict* d = new PluginJSDict;
    
    // first, grab the user document folders: Pictures and Movies
    string res;
    res = findUserFolder(kPictureDocumentsFolderType);
    if (!res.empty()) (*d)[res] = new BrowseRoot("Pictures", res, BrowseRoot::PATH_DOCUMENT_FOLDER, BrowseRoot::CONTENT_TYPE_PHOTOS);
    res = findUserFolder(kMovieDocumentsFolderType);
    if (!res.empty()) (*d)[res] = new BrowseRoot("Movies", res, BrowseRoot::PATH_DOCUMENT_FOLDER, BrowseRoot::CONTENT_TYPE_VIDEOS);
    res = findUserFolder(kDesktopFolderType);
  if (!res.empty()) (*d)[res] = new BrowseRoot("Desktop", res, BrowseRoot::PATH_DOCUMENT_FOLDER, BrowseRoot::CONTENT_TYPE_MIXED);
  res = findUserFolder(kDomainTopLevelFolderType);
  if (!res.empty()) (*d)[res] = new BrowseRoot("Home", res, BrowseRoot::PATH_DOCUMENT_FOLDER, BrowseRoot::CONTENT_TYPE_MIXED);
  
  
    // now enum all mounts
    for (ItemCount volIdx = 1; ; ++volIdx) {
        FSVolumeRefNum actualVolume;
        FSVolumeInfo info; // unused
        HFSUniStr255 volumeName;
        FSRef rootDirectory;
        
        OSErr err = FSGetVolumeInfo(kFSInvalidVolumeRefNum, volIdx, &actualVolume, kFSVolInfoNone, &info, &volumeName, &rootDirectory);
        if (err == nsvErr) {
            break; // iteration terminated
        }
        if (err != noErr) {
            continue; // I/O error or something
        }
        
        char outbuf[1024]; // UTF8
        ByteCount realInputLen, realOutputLen;
        TECObjectRef tec;
        OSStatus st = TECCreateConverter(&tec, CreateTextEncoding(kTextEncodingUnicodeDefault, kTextEncodingDefaultVariant, kUnicode16BitFormat),
                                     CreateTextEncoding(kTextEncodingUnicodeDefault, kTextEncodingDefaultVariant, kUnicodeUTF8Format));
        
        if (st != noErr) {
            continue;
        }
        
        st = TECConvertText(tec, (ConstTextPtr) volumeName.unicode, volumeName.length*2 / *bytes* /, &realInputLen, 
                        (TextPtr) outbuf, 1024, &realOutputLen);
        
        TECDisposeConverter(tec);
        if (st != noErr) {
            continue;
        }
        
        string volName(outbuf, realOutputLen);
        err = FSRefMakePath(&rootDirectory, (UInt8*)outbuf, 1024);
        if (err != noErr) {
            continue;
        }
    
        (*d)[string(outbuf)] = new BrowseRoot(volName, string(outbuf), BrowseRoot::PATH_VOLUME_ROOT, BrowseRoot::CONTENT_TYPE_MIXED);
    }
    return d;
}*/

bool Platform::pathIsHidden(const boost::filesystem::wpath& path_to_investigate) {
  // Same in Linux and Mac FYI
  std::wstring fn = path_to_investigate.filename();
  return (fn.empty() || fn[0] == L'.');
}


// If this directory doesn't have the flag set, try all the parents
bool Platform::pathIsSystem(const boost::filesystem::wpath& path_to_investigate) {
    string utf8_file_path = FB::wstring_to_utf8(path_to_investigate.file_string());

  FSRef ref;
  if (FSPathMakeRef(reinterpret_cast<const UInt8*>(utf8_file_path.data()), &ref, NULL) != 0) return false; // path convertion failed?

  cf_scoped_ptr<CFBooleanRef> hiddenAttr;
  if (LSCopyItemAttribute(&ref, kLSRolesViewer, kLSItemIsInvisible, (const void**) &hiddenAttr) != 0) return false;
  return hiddenAttr == kCFBooleanTrue;
}

bool Platform::pathIsShortcut(const boost::filesystem::wpath& path_to_investigate) {
  return false;
}

boost::filesystem::wpath Platform::resolveShortcut(const boost::filesystem::wpath& path_to_shortcut) {
  return path_to_shortcut;
}

HTTPProxyConfig Platform::getSystemProxyConfig() {
  
  cf_scoped_ptr<CFDictionaryRef> proxySettings(SCDynamicStoreCopyProxies(NULL));
  if (! proxySettings) return HTTPProxyConfig(); // NULL = no proxies configured
/*
 * -- relevant dictionary keys
 *   kSCPropNetProxiesHTTPEnable                        "HTTPEnable"                   CFNumber (0 or 1)
 *   kSCPropNetProxiesHTTPPort                          "HTTPPort"                     CFNumber
 *   kSCPropNetProxiesHTTPProxy                         "HTTPProxy"                    CFString
 *   kSCPropNetProxiesSOCKSEnable                       "SOCKSEnable"                  CFNumber (0 or 1)
 *   kSCPropNetProxiesSOCKSPort                         "SOCKSPort"                    CFNumber
 *   kSCPropNetProxiesSOCKSProxy                        "SOCKSProxy"                   CFString
 */
 
  // HTTP proxy?
  if (cfnumber_to_int((CFNumberRef) CFDictionaryGetValue(proxySettings, kSCPropNetProxiesHTTPEnable))) {
    HTTPProxyConfig res;
    res.type = HTTPProxyConfig::kHTTPProxy;
    res.hostname = conv_cfstring((CFStringRef) CFDictionaryGetValue(proxySettings, kSCPropNetProxiesHTTPProxy));
    res.port = cfnumber_to_int((CFNumberRef) CFDictionaryGetValue(proxySettings, kSCPropNetProxiesHTTPPort));
    return res;
  }
  
  // SOCKS proxy?
  if (cfnumber_to_int((CFNumberRef) CFDictionaryGetValue(proxySettings, kSCPropNetProxiesSOCKSEnable))) {
    HTTPProxyConfig res;
    res.type = HTTPProxyConfig::kSOCKS4Proxy;
    res.hostname = conv_cfstring((CFStringRef) CFDictionaryGetValue(proxySettings, kSCPropNetProxiesSOCKSProxy));
    res.port = cfnumber_to_int((CFNumberRef) CFDictionaryGetValue(proxySettings, kSCPropNetProxiesSOCKSPort));
    return res;
  }
  
  // no proxy.
  return HTTPProxyConfig();
}

/*
class QuickLookThumbnailGenerator : public VideoThumbnailGenerator {
public:
    QuickLookThumbnailGenerator() : qlframework(NULL) {
        // We dynamically resolve the reference to the QuickLook framework, since it's
        // not available on Mac OS X 10.4 (and we want to work there with only one binary)
        qlframework = dlopen("/System/Library/Frameworks/QuickLook.framework/QuickLook", RTLD_NOW);
    if (! qlframework) return; // QuickLook framework isn't available or doesn't exist.
        QLThumbnailImageCreate_ptr = reinterpret_cast<CGImageRef(*)(CFAllocatorRef,CFURLRef,CGSize,CFDictionaryRef)>(dlsym(qlframework, "QLThumbnailImageCreate"));
        if (QLThumbnailImageCreate_ptr) generators.push_back(this);
    }
  ~QuickLookThumbnailGenerator() {
    if (qlframework) dlclose(qlframework);
  }
    virtual bool generate(const std::string& filename, const ImageProcessor::Dimension& dim, ImageProcessor::Result& res) {
    if (! QLThumbnailImageCreate_ptr) return false; // Shouldn't happen, since we're not supposed to be registered if this failed

    cf_scoped_ptr<CFURLRef> url(CFURLCreateFromFileSystemRepresentation(kCFAllocatorDefault, reinterpret_cast<const UInt8*>(filename.data()), filename.size(), false));
    if (! url) return false;
    
    cf_scoped_ptr<CGImageRef> srcImage(QLThumbnailImageCreate_ptr(kCFAllocatorDefault, url, CGSizeMake(dim.x, dim.y), NULL));
    if (! srcImage) return false;

    cf_scoped_ptr<CFMutableDataRef> target(CFDataCreateMutable(kCFAllocatorDefault, 0));
    if (! target) return false;
    
    cf_scoped_ptr<CGImageDestinationRef> targetImage(CGImageDestinationCreateWithData(target, kUTTypeJPEG, 1, NULL));
    if (! targetImage) return false;
    
    CGImageDestinationAddImage(targetImage, srcImage, NULL);
    CGImageDestinationFinalize(targetImage);
    
    // copy target data to our result structure
    res.length = CFDataGetLength(target);
    res.data = boost::shared_array<char>(new char[res.length]);
    CFDataGetBytes(target, CFRangeMake(0, res.length), reinterpret_cast<UInt8*>(res.data.get()));
        return true;
    }
protected:
    CGImageRef(*QLThumbnailImageCreate_ptr)(CFAllocatorRef,CFURLRef,CGSize,CFDictionaryRef);
  void* qlframework;
};

QuickLookThumbnailGenerator _static_qlg;*/

