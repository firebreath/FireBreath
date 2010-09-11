/**********************************************************\
  Default Mac factory methods

  These are used to create the Npapi plugin type and
  the PluginWindow type on windows; if you want to extend
  these classses, do so and update these functions to
  instantiate the correct types.
\**********************************************************/

#include "Mac/NpapiPluginMac.h"
#include "Mac/PluginWindowMacCarbonQD.h"

FB::Npapi::NpapiPluginPtr _getNpapiPlugin(FB::Npapi::NpapiBrowserHostPtr &host)
{
    return FB::Npapi::NpapiPluginPtr(new FB::Npapi::NpapiPluginMac(host));
}

FB::PluginWindowMacCarbonQD* _createPluginWindowCarbonQD(CGrafPtr port, int x, int y)
{
    return new FB::PluginWindowMacCarbonQD(port, x, y);
}
