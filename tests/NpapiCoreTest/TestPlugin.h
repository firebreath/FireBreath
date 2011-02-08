/**********************************************************\
Original Author: Richard Bateman (taxilian)

Created:    Sep 10, 2010
License:    Dual license model; choose one of two:
            New BSD License
            http://www.opensource.org/licenses/bsd-license.php
            - or -
            GNU Lesser General Public License, version 2.1
            http://www.gnu.org/licenses/lgpl-2.1.html

Copyright 2009 Richard Bateman, Firebreath development team
\**********************************************************/
#ifndef H_TESTPLUGIN
#define H_TESTPLUGIN

#include "PluginCore.h"
#include "TestJSAPI.h"

class TestPlugin : public FB::PluginCore
{
public:
    static int init;
    static void StaticInitialize() { init = 1; }
    static void StaticDeinitialize() { init = 0; }

public:
    TestPlugin() { };
    virtual ~TestPlugin() {};

public:
    virtual FB::JSAPIPtr createJSAPI() { return FB::JSAPIPtr(new TestObjectJSAPI()); }
    bool HandleEvent(FB::PluginEvent *,FB::PluginEventSource *) { return false; }

};

int TestPlugin::init = 0;

#endif

