
#ifdef _WIN32
#include "win_targetver.h"
#include "win_common.h"
#endif
#include <boost/bind.hpp>
#include <boost/smart_ptr/enable_shared_from_this.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/make_shared.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/scoped_array.hpp>
#include <boost/scoped_ptr.hpp>
#include <boost/noncopyable.hpp>
#include <boost/thread/recursive_mutex.hpp>
#include <boost/thread/mutex.hpp>
#include <boost/thread.hpp>
#include <boost/config.hpp>
#include <boost/algorithm/string.hpp>
#include <vector>
#include <stdexcept>
#include <memory>
#include <assert.h>
#include <cassert>
#include <typeinfo>

#include "NpapiPluginModule.h"
#include "NpapiPlugin.h"
#include "NpapiBrowserHost.h"
#include "NpapiStream.h"
#include "NPObjectAPI.h"
#include "NPJavascriptObject.h"
#include "FactoryBase.h"
#include "PluginCore.h"
#include "PluginInfo.h"
#include "JSObject.h"
#include "logging.h"
#include "BrowserHost.h"
#include "NpapiTypes.h"
#include "APITypes.h"
#include "NpapiPluginModule.h"
#include "NPJavascriptObject.h"
#include "NPObjectAPI.h"
#include "DOM/Document.h"
#include "DOM/Window.h"
#include "variant_list.h"


