/**********************************************************\ 
 Original Author: Georg Fritzsche
 
 Created:    Aug 2, 2010
 License:    Dual license model; choose one of two:
 New BSD License
 http://www.opensource.org/licenses/bsd-license.php
 - or -
 GNU Lesser General Public License, version 2.1
 http://www.gnu.org/licenses/lgpl-2.1.html
 
 Copyright 2010 Georg Fritzsche, Firebreath development team
 \**********************************************************/

#include <vector>
#include <sstream>
#include <numeric>
#include <boost/assign.hpp>
#include "TestJSAPIAuto.h"
#include "JSCallback.h"
#include "variant_list.h"


TEST(JSCallback_Basics)
{
    PRINT_TESTNAME;
    
    using boost::assign::list_of;
    using namespace FB;
    
    boost::shared_ptr<TestObjectJSAPIAuto> backing(new TestObjectJSAPIAuto);
    
    {
        FB::JSAPIPtr test = FB::make_callback(backing, &TestObjectJSAPIAuto::returnString);
        const std::string method("");
        CHECK(test->HasMethod(method));
        const std::string value("foo");
        FB::VariantList args = list_of(value);        
        FB::variant ret = test->Invoke(method, args);        
        CHECK(ret.cast<std::string>() == value);
    }    
    
    {
        FB::JSAPIPtr test = FB::make_callback(backing, &TestObjectJSAPIAuto::concatenate);
        
        const std::string method("");
        CHECK(test->HasMethod(method));
        const std::string a("push "), b("me "), c("please");
        FB::VariantList args = list_of(a)(b)(c);
        FB::variant ret = test->Invoke(method, args);
        CHECK(ret.cast<std::string>() == (a+b+c));
    }
    
    {
        // test catching all remaining params with CatchAll as the last argument
        
        FB::JSAPIPtr test = FB::make_callback(backing, &TestObjectJSAPIAuto::concatMany);
        
        const std::string method("");
        CHECK(test->HasMethod(method));
        
        const unsigned max_args = 20;
        std::vector<std::string> strings(max_args);
        std::generate(strings.begin(), strings.end(), helper::IncNumber());
        
        for(unsigned i=1; i<=max_args; ++i)
        {
            FB::VariantList args(strings.begin(), strings.begin()+i);
            FB::variant ret = test->Invoke(method, args);
            const std::string expected = std::accumulate(strings.begin(), strings.begin()+i, std::string(""));
            const std::string result   = ret.cast<std::string>();
            CHECK(result == expected);
        }
    }
}

