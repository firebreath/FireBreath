/**********************************************************\
  Default X11 factory methods

  These are used to create the Npapi plugin type and
  the PluginWindow type on x11 (*nix); if you want to extend
  these classses, do so and update these functions to
  instantiate the correct types.
\**********************************************************/

#include <gtk/gtk.h>
#include "NpapiPlugin.h"
#include "X11/PluginWindowX11.h"
#include "X11/NpapiPluginX11.h"

FB::Npapi::NpapiPluginPtr _getNpapiPlugin(FB::Npapi::NpapiBrowserHostPtr& host)
{
    return FB::Npapi::NpapiPluginPtr(new FB::Npapi::NpapiPluginX11(host));
}

FB::PluginWindowX11 *_createPluginWindow(GdkNativeWindow win)
{
    return new FB::PluginWindowX11(win);
}
