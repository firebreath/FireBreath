/**********************************************************\
Original Author: Richard Bateman (taxilian)

Created:    Dec 11, 2010
License:    Dual license model; choose one of two:
            New BSD License
            http://www.opensource.org/licenses/bsd-license.php
            - or -
            GNU Lesser General Public License, version 2.1
            http://www.gnu.org/licenses/lgpl-2.1.html

Copyright 2009 Richard Bateman, Firebreath development team
\**********************************************************/

#ifndef H_AXPLUGINDEFS
#define H_AXPLUGINDEFS

#include "FBControl.h"

namespace FB { namespace ActiveX {
@@foreach (FBControl_GUID CURGUID)
    char FBCONTROL${i}_MIMETYPE[];
    typedef FB::ActiveX::CFBControl<&CLSID_FBControl${i}, FBCONTROL${i}_MIMETYPE, IFBControl, &DIID_IFBComEventSource, &FB_LIBID> CFBControl${i};
@@endforeach
} }

@@foreach (FBControl_GUID CURGUID)
typedef FB::ActiveX::CFBControl${i} CFBControl${i};
OBJECT_ENTRY_AUTO(__uuidof(FBControl${i}), CFBControl${i});
@@endforeach
#endif

