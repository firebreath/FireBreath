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

#include "Win/win_common.h"
#include "UnitTest++.h"

#define PRINT_TESTNAME  printf("Running unit test %s::%s...\n", UnitTestSuite::GetSuiteName(), \
    m_details.testName); fflush(stdout)

#include "COMJavascriptObjectTest.h"

using namespace std;

namespace FB { namespace Npapi {
    class NpapiPlugin;
    class NpapiBrowserHost;
    typedef boost::shared_ptr<NpapiBrowserHost> NpapiBrowserHostPtr;
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

FB::Npapi::NpapiPlugin *_getNpapiPlugin(FB::Npapi::NpapiBrowserHostPtr host)
{
    return NULL;
    //return new FB::Npapi::NpapiPlugin(host);
}