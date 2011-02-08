
#include "win_targetver.h"
#include "APITypes.h"
#include "JSAPI.h"
#include "global/resource.h"
#include "global/COM_config.h"
#include "FireBreathWin_i.h"
#include "COMJavascriptObject.h"
#include "ActiveXBrowserHost.h"
#include "global/axplugin_defs.h"
#include "FBControl.h"
#include "Win/PluginWindowWin.h"
#include "Win/PluginWindowlessWin.h"

#include "ActiveXFactoryDefinitions.h"

typedef FB::ActiveX::COMJavascriptObject<&CLSID_FBComJavascriptObject, IFBComJavascriptObject, &DIID_IFBComEventSource, &FB_LIBID> COMJSObject;

// Definitions used for creating the plugins
#include "global/axplugin_defs.inc"

IDispatchEx* _getCOMJSWrapper( FB::BrowserHostPtr host, FB::JSAPIWeakPtr api )
{
    return COMJSObject::NewObject(FB::ptr_cast<FB::ActiveX::ActiveXBrowserHost>(host), api);
}

HRESULT _updateRegistry( bool res ) {
    return CFBControl0::UpdateRegistry(true);
}

