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

#ifdef FB_WIN
HINSTANCE gInstance = NULL;
std::string g_dllPath = "";
#endif

class TestFactory : public FB::FactoryBase
{
public:
    std::string getPluginName() { return pluginName; }
    std::string getPluginDesc() { return pluginDesc; }
    
    FB::PluginCorePtr createPlugin(const std::string&) { return boost::make_shared<TestPlugin>(); }
    
    void globalPluginInitialize() { 
        TestPlugin::StaticInitialize(); 
    }
    
    void globalPluginDeinitialize() {
        TestPlugin::StaticDeinitialize();
    }
    
    FB::Npapi::NpapiPluginPtr createNpapiPlugin(const NpapiBrowserHostPtr& host, std::string mimetype)
    {
        return boost::make_shared<FB::Npapi::NpapiPlugin>(host, mimetype);
    }
};

FB::FactoryBasePtr getFactoryInstance()
{
    static FB::FactoryBasePtr factory = boost::make_shared<TestFactory>();
    return factory;
}

// Needed for FSPath on Unix
#ifndef FB_WIN
#  if FB_MACOSX
extern "C" NPError NP_Initialize(NPNetscapeFuncs*) 
#  elif FB_X11
extern "C" NPError NP_Initialize(NPNetscapeFuncs*, NPPluginFuncs*)
#  endif
{
    return 0;
}
#endif


