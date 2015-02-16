/**********************************************************\
 Original Author: Georg Fritzsche

 Created:    Oct 23, 2010
 License:    Dual license model; choose one of two:
             New BSD License
             http://www.opensource.org/licenses/bsd-license.php
             - or -
             GNU Lesser General Public License, version 2.1
             http://www.gnu.org/licenses/lgpl-2.1.html

 Copyright 2010 Georg Fritzsche, Firebreath development team
\**********************************************************/

#pragma once
#ifndef H_FB_NPAPIPLUGINFACTORY
#define H_FB_NPAPIPLUGINFACTORY

#include <memory>

namespace FB { namespace Npapi
{
    FB_FORWARD_PTR(NpapiPlugin);
    FB_FORWARD_PTR(NpapiBrowserHost);

    NpapiPluginPtr createNpapiPlugin(const NpapiBrowserHostPtr& host, std::string mimetype);
} }

#endif

