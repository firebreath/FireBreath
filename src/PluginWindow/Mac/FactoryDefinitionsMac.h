#ifndef H_FactoryDefinitionsMac
#define H_FactoryDefinitionsMac

/**********************************************************\
Original Author: Richard Bateman (taxilian)

Created:    Oct 19, 2009
License:    Dual license model; choose one of two:
            New BSD License
            http://www.opensource.org/licenses/bsd-license.php
            - or -
            GNU Lesser General Public License, version 2.1
            http://www.gnu.org/licenses/lgpl-2.1.html

Copyright 2009 Richard Bateman, Firebreath development team
\**********************************************************/

#include "npapi.h"

#ifndef __LP64__
#include <Carbon/Carbon.h>
#endif

namespace FB {
    class JSAPI;
    class PluginCore;
    class PluginWindowMacCarbonQD;
    class PluginWindowMacCarbonCG;
    class PluginWindowMacCocoaCG;
    class PluginWindowMacCocoaCA;
    class PluginWindowMacCocoaICA;
    namespace Npapi {
        class NpapiPlugin;
        class NpapiBrowserHost;
    };
};

// These global functions *must* be defined in your main plugin project -- that's the one that
// actually builds into a DLL that everything links to.  These are used to determine what
// JSAPI object is used as the root JSAPI, what type of Plugin object to use, etc, as well
// as providing a way for the plugin to find out such information as MIME type, plugin name, etc

FB::PluginWindowMacCarbonQD* _createPluginWindowCarbonQD(CGrafPtr port, int x, int y);

FB::PluginWindowMacCarbonCG* _createPluginWindowCarbonCG(NP_CGContext* context);

FB::PluginWindowMacCocoaCG* _createPluginWindowCocoaCG(NP_CGContext* context);

FB::PluginWindowMacCocoaCA* _createPluginWindowCocoaCA();

FB::PluginWindowMacCocoaICA* _createPluginWindowCocoaICA();

#endif
