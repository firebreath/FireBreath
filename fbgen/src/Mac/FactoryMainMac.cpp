/**********************************************************\
  Default Mac factory methods

  These are used to create the Npapi plugin type and
  the PluginWindow type on windows; if you want to extend
  these classses, do so and update these functions to
  instantiate the correct types.
\**********************************************************/

#include "NpapiPlugin.h"
#include "Mac/PluginWindowMacQuickDraw.h"

FB::Npapi::NpapiPlugin *_getNpapiPlugin(FB::Npapi::NpapiBrowserHost *host)
{
    return new FB::Npapi::NpapiPlugin(host);
}

FB::PluginWindowMacQuickDraw* _createPluginWindow(CGrafPtr port, int x, int y)
{
    return new FB::PluginWindowMacQuickDraw(port, x, y);
}
