/**********************************************************\
  Default X11 factory methods

  These are used to create the Npapi plugin type and
  the PluginWindow type on x11 (*nix); if you want to extend
  these classses, do so and update these functions to
  instantiate the correct types.
\**********************************************************/

#include "NpapiPlugin.h"
#include "X11/PluginWindowX11.h"
#include "X11/NpapiPluginX11.h"

FB::Npapi::NpapiPlugin *_getNpapiPlugin(FB::Npapi::NpapiBrowserHost *host)
{
    return new FB::Npapi::NpapiPluginX11(host);
}

FB::PluginWindowX11 *_createPluginWindow(Window win)
{
    return new FB::PluginWindowX11(win);
}
