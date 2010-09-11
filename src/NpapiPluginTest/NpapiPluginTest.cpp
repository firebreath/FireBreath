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

#include "UnitTest++.h"

#define PRINT_TESTNAME  printf("Running unit test %s::%s...\n", UnitTestSuite::GetSuiteName(), \
    m_details.testName); fflush(stdout)

#include "TestPlugin.h"
#include "NPJavascriptObjectTest.h"
#include "NpapiPlugin.h"

using namespace std;

int main()
{
    return UnitTest::RunAllTests();

    return 0;
}

static const char *pluginName = "UnitTestMockPlugin";
static const char *pluginDesc = "It's a mock plugin for a unit test";

const char *_getPluginName()
{
    return pluginName;
}

const char *_getPluginDesc()
{
    return pluginDesc;
}

FB::JSAPI *_getRootJSAPI()
{
    return new FB::JSAPISimple();
}

FB::Npapi::NpapiPluginPtr _getNpapiPlugin(NpapiBrowserHostPtr host)
{
    return FB::Npapi::NpapiPluginPtr(new FB::Npapi::NpapiPlugin(host));
}

FB::PluginCore *_getMainPlugin()
{
    return new TestPlugin();
}

void GlobalPluginInitialize()
{
    TestPlugin::StaticInitialize();
}

void GlobalPluginDeinitialize()
{
    TestPlugin::StaticDeinitialize();
}