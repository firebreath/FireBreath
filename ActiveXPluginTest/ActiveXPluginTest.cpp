/**********************************************************\ 
Original Author: Richard Bateman (taxilian)

Created:    Oct 19, 2009
License:    Eclipse Public License - Version 1.0
            http://www.eclipse.org/legal/epl-v10.html

Copyright 2009 Richard Bateman, Firebreath development team
\**********************************************************/

#include "UnitTest++.h"

#define PRINT_TESTNAME  printf("Running unit test %s::%s...\n", UnitTestSuite::GetSuiteName(), \
    m_details.testName); fflush(stdout)

#include "IDispatchObjectTest.h"

using namespace std;

namespace FB { namespace Npapi {
    class NpapiPlugin;
    class NpapiBrowserHost;
}; };

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

FB::Npapi::NpapiPlugin *_getNpapiPlugin(FB::Npapi::NpapiBrowserHost *host)
{
    return NULL;
    //return new FB::Npapi::NpapiPlugin(host);
}