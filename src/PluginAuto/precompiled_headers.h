
#if !defined(FB_UNITTEST)

#ifdef _WIN32
#include "win_targetver.h"
#include "win_common.h"
#include "global/resource.h"
#include "global/COM_config.h"
#include "FireBreathWin_i.h"
#include "FBControl.h"
#include "COMJavascriptObject.h"
#include "ActiveXBrowserHost.h"
#include "ActiveXFactoryDefinitions.h"
#include "global/axplugin_defs.h"
#include "axutil.h"
#include "Win/NpapiPluginWin.h"
#include "Win/PluginWindowWin.h"
#include "Win/PluginWindowlessWin.h"
#include "ConstructDefaultPluginWindows.h"
#include "Win/KeyCodesWin.h"
#include "PluginEvents/WindowsEvent.h"
#include <ShlGuid.h>
#endif

#include "global/config.h"
#include <map>
#include <string>
#include <vector>
#include <boost/algorithm/string.hpp>
#include <boost/scoped_ptr.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/make_shared.hpp>

#include "NpapiTypes.h"
#include "NpapiPluginModule.h"
#include "NpapiPluginFactory.h"
#include "FactoryBase.h"
#include "logging.h"
#include "utf8_tools.h"
#include "PluginInfo.h"
#include "PluginCore.h"
#include "PluginEvents/GeneralEvents.h"
#include "PluginEvents/DrawingEvents.h"
#include "PluginEvents/MouseEvents.h"
#include "PluginEvents/KeyboardEvents.h"
#include "AsyncFunctionCall.h"

#endif

