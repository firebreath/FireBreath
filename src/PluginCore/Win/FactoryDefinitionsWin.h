/**********************************************************\ 
Original Author: Richard Bateman (taxilian)

Created:    Oct 19, 2009
License:    Eclipse Public License - Version 1.0
            http://www.eclipse.org/legal/epl-v10.html

Copyright 2009 Richard Bateman, Firebreath development team
\**********************************************************/

#ifndef H_FactoryDefinitions
#define H_FactoryDefinitions

#include "Win/win_common.h"

namespace FB {
    class JSAPI;
    class PluginCore;
    class PluginWindowWin;
    namespace Npapi {
        class NpapiPlugin;
        class NpapiBrowserHost;
    };
}

// These global functions *must* be defined in your main plugin project -- that's the one that
// actually builds into a DLL that everything links to.  These are used to determine what 
// JSAPI object is used as the root JSAPI, what type of Plugin object to use, etc, as well
// as providing a way for the plugin to find out such information as MIME type, plugin name, etc

FB::PluginWindowWin *createPluginWindow(HWND hWnd);

#endif