/**********************************************************\
Original Author: Richard Bateman (taxilian)

Created:    Oct 19, 2009
License:    Dual license model; choose one of two:
            Eclipse Public License - Version 1.0
            http://www.eclipse.org/legal/epl-v10.html
            - or -
            GNU Lesser General Public License, version 2.1
            http://www.gnu.org/licenses/lgpl-2.1.html

Copyright 2009 Richard Bateman, Firebreath development team
\**********************************************************/

#ifndef H_FactoryDefinitionsX11
#define H_FactoryDefinitionsX11

#include <gdk/gdk.h>
#include <gdk/gdkx.h>
#include <gtk/gtk.h>

namespace FB {
    class JSAPI;
    class PluginCore;
    class PluginWindowX11;
    namespace Npapi {
        class NpapiPlugin;
        class NpapiBrowserHost;
    };
}

// These global functions *must* be defined in your main plugin project -- that's the one that
// actually builds into a DLL that everything links to.  These are used to determine what
// JSAPI object is used as the root JSAPI, what type of Plugin object to use, etc, as well
// as providing a way for the plugin to find out such information as MIME type, plugin name, etc

FB::PluginWindowX11 *_createPluginWindow(Window win);

#endif
