/*
 *  platform_common.cpp
 *  fbplugin
 *
 *  To be included on all platforms.
 *
 */

#include "Platform.h"
#include "openssl/rand.h"
#include "openssl/err.h"
#include "openssl/crypto.h"
#include <boost/filesystem.hpp>
#include <boost/thread/shared_mutex.hpp>

using std::string;

using namespace HTTP;

//std::list<VideoThumbnailGenerator*> VideoThumbnailGenerator::generators;

boost::filesystem::wpath Platform::resolveShortcutsInPath(const std::wstring& path_to_object) {
  return resolveShortcutsInPath(boost::filesystem::wpath(path_to_object));
}

boost::filesystem::wpath Platform::resolveShortcutsInPath(const boost::filesystem::wpath& original_path) {
  if (boost::filesystem::exists(original_path)) {
    if (pathIsShortcut(original_path)) return resolveShortcut(original_path);
    else return original_path;
  }

  boost::filesystem::wpath path_to_object = original_path;

  std::list<std::wstring> components;
  while (path_to_object.has_parent_path()) {
    components.push_front(path_to_object.filename());
    path_to_object = path_to_object.parent_path();
    if (boost::filesystem::exists(path_to_object)) {
      if (pathIsShortcut(path_to_object)) {
        path_to_object = resolveShortcut(path_to_object);
        while (!components.empty()) {
          path_to_object /= components.front();
          components.pop_front();
        }
        if (boost::filesystem::exists(path_to_object)) return path_to_object;
      } else {
        throw boost::filesystem::wfilesystem_error("resolveShortcutsInPath: first existent parent is not a shortcut file", original_path, path_to_object, boost::system::errc::make_error_code(boost::system::errc::bad_address));
      }
    }
  }
  // put the resolved path back together for our exception
  while (!components.empty()) {
    path_to_object /= components.front();
    components.pop_front();
  }
  throw boost::filesystem::wfilesystem_error("resolveShortcutsInPath: target does not exist after resolving all shortcuts.", original_path, path_to_object, boost::system::errc::make_error_code(boost::system::errc::bad_address));

}

//string BrowseRoot::convertToJSON() const {
//  PluginJSDict d;
//  
//  d.insert(std::make_pair(string("name"), name));
//  d.insert(std::make_pair(string("full_path"), full_path));
//  d.insert(std::make_pair(string("path_type"), string(pathtype_names[p_type])));
//  d.insert(std::make_pair(string("content_type"), string(contenttype_names[c_type])));
//  d.insert(std::make_pair(string("traversable"), PluginVariant(true)));
//           
//  return d.convertToJSON();
//}

//bool Platform::generateVideoThumbnail(const std::string& filename, const ImageProcessor::Dimension& dim, ImageProcessor::Result& res) {
//  for (std::list<VideoThumbnailGenerator*>::iterator it = VideoThumbnailGenerator::generators.begin(); it != VideoThumbnailGenerator::generators.end(); ++it) {
//      if ((*it)->generate(filename, dim, res)) return true;
//  }
//  return false; // no handlers worked...
//}

char* Platform::getRandomBytes(size_t bytecount) {
  if (! bytecount) return NULL;
  char* b = new char[bytecount];
  if (! RAND_bytes(reinterpret_cast<unsigned char*>(b), bytecount)) {
    throw std::runtime_error((string("RAND_bytes(): ") + ERR_error_string(ERR_get_error(), NULL)).c_str());
  }
  return b;
}

struct CRYPTO_dynlock_value {
  boost::shared_mutex m;
};

extern "C" struct CRYPTO_dynlock_value* dynlock_create(const char* file, int line) {
  return new CRYPTO_dynlock_value();
}
extern "C" void dynlock_do_lock(int mode, struct CRYPTO_dynlock_value* l, const char* file, int line) {
  switch (mode) {
    case (CRYPTO_LOCK|CRYPTO_READ):
      l->m.lock_shared();
      break;
    case (CRYPTO_LOCK|CRYPTO_WRITE):
      l->m.lock();
      break;
    case (CRYPTO_UNLOCK|CRYPTO_READ):
      l->m.unlock_shared();
      break;
    case (CRYPTO_UNLOCK|CRYPTO_WRITE):
      l->m.unlock();
      break;
    default:
      assert(0 && "CRYPTO_dynlock_value::do_lock(): unrecognized mode");
  };
}
extern "C" void dynlock_destroy(struct CRYPTO_dynlock_value* l, const char* file, int line) {
  delete l;
}

CRYPTO_dynlock_value* crypto_static_locks = NULL;
extern "C" void crypto_lock_cb(int mode, int n, const char* file, int line) {
  CRYPTO_dynlock_value* v = &crypto_static_locks[n];
  dynlock_do_lock(mode, v, file, line);
}

class _openssl_locking_init_t {
public:
  _openssl_locking_init_t() {
    if (CRYPTO_get_locking_callback()) {
#ifndef NDEBUG
      printf("fbplugin init: OpenSSL has a locking callback already.\n");
#endif
      return;
      
    } else {
    
#ifndef NDEBUG
      printf("fbplugin init: Setting up OpenSSL locking.\n");
#endif

    }
    crypto_static_locks = new CRYPTO_dynlock_value[CRYPTO_num_locks()];
    CRYPTO_set_locking_callback(crypto_lock_cb);
    CRYPTO_set_dynlock_create_callback(dynlock_create);
    CRYPTO_set_dynlock_lock_callback(dynlock_do_lock);
    CRYPTO_set_dynlock_destroy_callback(dynlock_destroy);
  }
  ~_openssl_locking_init_t() {
  
  }
};

static _openssl_locking_init_t openssl_locking_init;

