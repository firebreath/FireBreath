/**********************************************************\
Original Author: Anson MacKeracher 

Created:    Sept 17, 2010
License:    Dual license model; choose one of two:
            New BSD License
            http://www.opensource.org/licenses/bsd-license.php
            - or -
            GNU Lesser General Public License, version 2.1
            http://www.gnu.org/licenses/lgpl-2.1.html

Copyright 2010 Anson MacKeracher, Firebreath development team
\**********************************************************/

#ifndef H_PLUGINWINDOWMAC_COCOAINVALIDATINGCA_H__
#define H_PLUGINWINDOWMAC_COCOAINVALIDATINGCA_H__

#include "PluginWindowMacCocoaCA.h"

// This class implements the modifications to the CoreAnimation
// drawing model in order to implement the InvalidatingCoreAnimation 
// model.
//
// MDC: https://wiki.mozilla.org/NPAPI:InvalidatingCoreAnimation

namespace FB {
    class PluginWindowMacCocoaICA : public PluginWindowMacCocoaCA {
        public:
            PluginWindowMacCocoaICA();
            ~PluginWindowMacCocoaICA();
    };
};

#endif

