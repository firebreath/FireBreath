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

FB::Npapi::NpapiPluginPtr _getNpapiPlugin(int a, FB::Npapi::NpapiBrowserHostPtr &host)
{
    return FB::Npapi::NpapiPluginPtr(new FB::Npapi::NpapiPluginMac(host));
}

#if FBMAC_USE_CARBON
#if FBMAC_USE_COREGRAPHICS
FB::PluginWindowCarbonCG *_createPluginWindow(NP_CGContext* context)
{
    return new FB::PluginWindowMacCarbonCG(context);
}
#endif
#endif

#if FBMAC_USE_COCOA
#if FBMAC_USE_COREGRAPHICS
FB::PluginWindowMacCocoaCG *_createPluginWindowCocoaCG(NP_CGContext* context)
{
    return new FB::PluginWindowMacCocoaCG(context);
}
#endif
#endif
