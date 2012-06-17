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
#include "precompiled_headers.h" // On windows, everything above this line in PCH

typedef FB::ActiveX::COMJavascriptObject<&CLSID_FBComJavascriptObject, IFBComJavascriptObject, &DIID_IFBComEventSource, &FB_LIBID> COMJSObject;

// Definitions used for creating the plugins
#include "global/axplugin_defs.inc"

IDispatchEx* _getCOMJSWrapper( const FB::BrowserHostPtr& host, const FB::JSAPIWeakPtr& api, bool autoRelease /*= false*/ )
{
    return COMJSObject::NewObject(FB::ptr_cast<FB::ActiveX::ActiveXBrowserHost>(host), api, autoRelease);
}
const FB::WeakIDispatchExRef _getWeakRefFromCOMJSWrapper(IDispatchEx* wrapper)
{
    assert(wrapper != NULL);
    COMJSObject* ptr(dynamic_cast<COMJSObject*>(wrapper));
    assert(ptr != NULL); // If this assertion fails, the wrong type of object was passed in!
    return ptr->getWeakReference();
}

HRESULT _updateRegistry( bool res ) {
    return CFBControl0::UpdateRegistry(res);
}

