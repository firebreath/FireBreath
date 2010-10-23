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
#include "FactoryBase.h"
#include <boost/make_shared.hpp>

using namespace std;

int main()
{
    return UnitTest::RunAllTests();

    return 0;
}

static const char *pluginName = "UnitTestMockPlugin";
static const char *pluginDesc = "It's a mock plugin for a unit test";

class TestFactory : public FB::FactoryBase
{
public:
    std::string getPluginName() { return pluginName; }
    std::string getPluginDesc() { return pluginDesc; }
    
    FB::PluginCorePtr createPlugin() { return boost::make_shared<TestPlugin>(); }
    
    void globalPluginInitialize() { 
        TestPlugin::StaticInitialize(); 
    }
    
    void globalPluginDeinitialize() {
        TestPlugin::StaticDeinitialize();
    }
    
    FB::Npapi::NpapiPluginPtr createNpapiPlugin(const NpapiBrowserHostPtr& host)
    {
        return boost::make_shared<FB::Npapi::NpapiPlugin>(host);
    }
};

FB::FactoryBasePtr getFactoryInstance()
{
    static FB::FactoryBasePtr factory = boost::make_shared<TestFactory>();
    return factory;
}

// Needed for FSPath on Unix
#ifndef FB_WIN
extern "C" NPError NP_Initialize(NPNetscapeFuncs*) 
{
    return 0;
}
#endif