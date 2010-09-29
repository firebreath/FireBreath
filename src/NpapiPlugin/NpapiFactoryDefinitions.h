/**********************************************************\ 
Original Author: Richard Bateman (taxilian)

Created:    Sep 11, 2010
License:    Dual license model; choose one of two:
            New BSD License
            http://www.opensource.org/licenses/bsd-license.php
            - or -
            GNU Lesser General Public License, version 2.1
            http://www.gnu.org/licenses/lgpl-2.1.html

Copyright 2010 Richard Bateman, Firebreath development team
\**********************************************************/

#pragma once
#ifndef H_NpapiFactoryDefinitions
#define H_NpapiFactoryDefinitions

#include <boost/shared_ptr.hpp>
#include "NpapiBrowserHost.h"
#include "NpapiPlugin.h"

FB::Npapi::NpapiPluginPtr _getNpapiPlugin(FB::Npapi::NpapiBrowserHostPtr &host);

#endif // H_NpapiFactoryDefinitions

