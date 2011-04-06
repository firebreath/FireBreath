
#include "BlankFactories.h"
#include "FactoryBase.h"
#include <boost/shared_ptr.hpp>
#ifdef FB_WIN
#include "Win/WindowContextWin.h"
#endif

namespace FB { namespace Npapi 
{ 
    class NpapiPlugin; 
    class NpapiBrowserHost;
    typedef boost::shared_ptr<NpapiPlugin> NpapiPluginPtr;
    typedef boost::shared_ptr<NpapiBrowserHost> NpapiBrowserHostPtr;
    
    NpapiPluginPtr createNpapiPlugin(const NpapiBrowserHostPtr& host, const std::string& mimetype)
    {
        return NpapiPluginPtr();
    }
} }

namespace FB
{
#ifdef FB_WIN
    class PluginWindowWin;
    class PluginWindowlessWin;
    FB::PluginWindowWin* createPluginWindowWin(const WindowContextWin&) { return NULL; }  
    FB::PluginWindowlessWin* createPluginWindowless(const WindowContextWindowless&) { return NULL; }
#endif
    
#ifdef FB_MACOSX
    FB::PluginWindowMacCarbonQD* createPluginWindowCarbonQD(const WindowContextQuickDraw&) { return NULL; }
    FB::PluginWindowMacCarbonCG* createPluginWindowCarbonCG(const WindowContextCoreGraphics&) { return NULL; }
    FB::PluginWindowMacCocoaCG* createPluginWindowCocoaCG() { return NULL; }    
    FB::PluginWindowMacCocoaCA* createPluginWindowCocoaCA() { return NULL; }
    FB::PluginWindowMacCocoaICA* createPluginWindowCocoaICA() { return NULL; }
#endif
    
#ifdef FB_X11
    FB::PluginWindowX11* createPluginWindowX11(const FB::WindowContextX11&) { return NULL; }
#endif
    
}

std::string g_dllPath = "UnitTest!";
HINSTANCE gInstance = 0;
#define MIDL_DEFINE_GUID(type,name,l,w1,w2,b1,b2,b3,b4,b5,b6,b7,b8) \
        const type name = {l,w1,w2,{b1,b2,b3,b4,b5,b6,b7,b8}}
// Global interface for all firebreath plugins
MIDL_DEFINE_GUID(CLSID, CLSID_FBComJavascriptObject,0x0861570f,0x19df,0x54f3,0xa7,0xed,0x4b,0x6d,0xb6,0xf9,0x43,0xbc);
MIDL_DEFINE_GUID(IID, LIBID_FireBreathWinLib,0x43a2a216,0x7fea,0x549e,0xb0,0x63,0xe4,0x90,0xe5,0xe1,0xa4,0x9a);
MIDL_DEFINE_GUID(IID, DIID_IFBComEventSource,0xabc530a7,0x7627,0x5b5c,0x92,0x85,0xf4,0x3d,0x6c,0x23,0xc9,0x57);
#undef MIDL_DEFINE_GUID

static const char *pluginName = "UnitTestMockPlugin";
static const char *pluginDesc = "It's a mock plugin for a unit test";

class TestFactory : public FB::FactoryBase
{
public:
    std::string getPluginName() { return pluginName; }
    std::string getPluginDescription() { return pluginDesc; }
    
    FB::PluginCorePtr createPlugin(const std::string&) { return FB::PluginCorePtr(); }
};

FB::FactoryBasePtr getFactoryInstance()
{
    static FB::FactoryBasePtr factory = boost::make_shared<TestFactory>();
    return factory;
}

IDispatchEx* _getCOMJSWrapper( const FB::BrowserHostPtr& host, const FB::JSAPIWeakPtr& api, bool autoRelease )
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
    return S_OK;
}

