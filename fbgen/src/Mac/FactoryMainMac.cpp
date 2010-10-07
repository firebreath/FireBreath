/**********************************************************\
  Default Mac factory methods

  These are used to create the Npapi plugin type and
  the PluginWindow type on mac; if you want to extend
  these classses, do so and update these functions to
  instantiate the correct types.
\**********************************************************/

#include "NpapiPlugin.h"
#include "config.h"
#include "Mac/NpapiPluginMac.h"
#include "Mac/PluginWindowMacCarbonQD.h"
#include "Mac/PluginWindowMacCarbonCG.h"
#include "Mac/PluginWindowMacCocoaCG.h"
#include "Mac/PluginWindowMacCocoaCA.h"
#include "NpapiBrowserHost.h"

FB::Npapi::NpapiPluginPtr _getNpapiPlugin(FB::Npapi::NpapiBrowserHostPtr& host)
{
    return FB::Npapi::NpapiPluginPtr(new FB::Npapi::NpapiPluginMac(host));
}

#if FBMAC_USE_CARBON
# if FBMAC_USE_QUICKDRAW
FB::PluginWindowMacCarbonQD* _createPluginWindowCarbonQD(CGrafPtr port, int x, int y)
{
    return new FB::PluginWindowMacCarbonQD(port, x, y);
}
# endif
# if FBMAC_USE_COREGRAPHICS
FB::PluginWindowMacCarbonCG *_createPluginWindowCarbonCG(NP_CGContext* context)
{
    return new FB::PluginWindowMacCarbonCG(context);
}
# endif
#endif

#if FBMAC_USE_COCOA
# if FBMAC_USE_COREGRAPHICS
FB::PluginWindowMacCocoaCG *_createPluginWindowCocoaCG(NP_CGContext* context)
{
    return new FB::PluginWindowMacCocoaCG(context);
}
# endif
# if FBMAC_USE_COREANIMATION
FB::PluginWindowMacCocoaCA* _createPluginWindowCocoaCA() {
    return new FB::PluginWindowMacCocoaCA();
}
# endif
#endif
