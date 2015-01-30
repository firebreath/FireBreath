/*********************************************************************\
    Original Author: Richard Bateman

    Created:    Jan 18, 2015
    License:    Dual license model; choose one of two:
                New BSD License
                http://www.opensource.org/licenses/bsd-license.php
                - or -
                GNU Lesser General Public License, version 2.1
                http://www.gnu.org/licenses/lgpl-2.1.html

    Copyright 2015 Richard Bateman and the Firebreath development team
\*********************************************************************/

#pragma once
#ifndef H_WYRMPLUGINMODULE
#define H_WYRMPLUGINMODULE

#include "FireWyrm.h"
#include <boost/noncopyable.hpp>
#include "APITypes.h"

namespace FB { namespace FireWyrm {
    FB_FORWARD_PTR(WyrmPlugin);
    class WyrmPluginModule : boost::noncopyable
    {
    public:
        WyrmPluginModule();
        virtual ~WyrmPluginModule();

        typedef std::map<const FW_INST,WyrmPluginModule*> Modules;
        static WyrmPluginModule* GetModule(const void*);
        static FW_RESULT ReleaseModule(const void*);

    protected:
        boost::thread::id m_threadId;
        static volatile uint32_t PluginModuleInitialized;
        static Modules m_modules;
    };

} };

#endif // H_WYRMPLUGINMODULE

