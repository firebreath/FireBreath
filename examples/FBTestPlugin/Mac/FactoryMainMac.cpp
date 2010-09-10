/**********************************************************\
  Default Mac factory methods

  These are used to create the Npapi plugin type and
  the PluginWindow type on mac; if you want to extend
  these classses, do so and update these functions to
  instantiate the correct types.
\**********************************************************/

#include "NpapiPlugin.h"
#include "Mac/NpapiPluginMac.h"
#include "Mac/PluginWindowMacCarbonQD.h"
#include "Mac/PluginWindowMacCarbonCG.h"
#include "Mac/PluginWindowMacCocoaCA.h"

FB::Npapi::NpapiPlugin *_getNpapiPlugin(FB::Npapi::NpapiBrowserHostPtr host)
{
    return new FB::Npapi::NpapiPluginMac(host);
}

FB::PluginWindowMacCarbonCG *_createPluginWindow(NP_CGContext* context)
{
    return new FB::PluginWindowMacCarbonCG(context);
}
