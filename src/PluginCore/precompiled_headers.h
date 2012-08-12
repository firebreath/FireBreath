
#ifdef _WIN32
#include "win_targetver.h"
#include <boost/asio.hpp>
#include "win_common.h"
#else
#include <xlocale.h>
#include <wctype.h>
#endif
#include <boost/bind.hpp>
#include <boost/smart_ptr/enable_shared_from_this.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/make_shared.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/scoped_array.hpp>
#include <boost/scoped_ptr.hpp>
#include <boost/noncopyable.hpp>
#include <boost/thread/recursive_mutex.hpp>
#include <boost/thread/mutex.hpp>
#include <vector>
#include <stdexcept>

#include "APITypes.h"
#include "variant.h"
#include "variant_list.h"
#include "JSAPI.h"
#include "JSAPIAuto.h"
#include "JSAPIProxy.h"
#include "JSFunction.h"
#include "JSEvent.h"
#include "JSExceptions.h"
#include "URI.h"
#include "utf8_tools.h"
#include "JSObject.h"
#include "DOM/Window.h"
#include "variant_list.h"
#include "logging.h"
#include "BrowserHost.h"
#include "BrowserPlugin.h"
#include "FactoryBase.h"
#include "PluginCore.h"
#include "BrowserStream.h"
#include "BrowserStreamManager.h"
#include "ConstructDefaultPluginWindows.h"
#include "NpapiPluginFactory.h"
#include "PluginWindow.h"
#include "PluginEvent.h"
#include "PluginEventSink.h"
#include "PluginEventSource.h"
#include "PluginEvents/AttachedEvent.h"
#ifdef FB_WIN
#include "ActiveXFactoryDefinitions.h"
#include "Win/SystemProxyDetectorWin.h"
#endif
#include "PluginInfo.h"

