/**********************************************************\ 
Original Author: Georg Fritzsche

Created:    November 7, 2009
License:    Dual license model; choose one of two:
            New BSD License
            http://www.opensource.org/licenses/bsd-license.php
            - or -
            GNU Lesser General Public License, version 2.1
            http://www.gnu.org/licenses/lgpl-2.1.html

Copyright 2009 Georg Fritzsche, Firebreath development team
\**********************************************************/

#include <vector>
#include <sstream>
#include <numeric>
#include <boost/assign.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/make_shared.hpp>
#include "TestJSAPIAuto.h"
#include "fake_jsarray.h"
#include "variant_list.h"

namespace helper
{
    template<class T>
    std::string toString(const T t)
    {
        std::stringstream ss;
        ss << t;
        return ss.str();
    }
    
    struct IncNumber
    {
        unsigned n;
        IncNumber() : n(1) {}
        std::string operator()() 
        { return boost::lexical_cast<std::string>(n++); }
    };
}

TEST(JSAPIAuto_Methods)
{
    PRINT_TESTNAME;

    using boost::assign::list_of;
    using namespace FB;

    FB::JSAPIPtr test(new TestObjectJSAPIAuto());
    {
        const std::string method("returnString");
        CHECK(test->HasMethod(method));
        const std::string value("foo");
        FB::VariantList args = list_of(value);        
        FB::variant ret = test->Invoke(method, args);        
        CHECK(ret.cast<std::string>() == "foo");
    }

    {
        const std::string method("intToString");
        CHECK(test->HasMethod(method));
        const int value = 23;
        FB::VariantList args = list_of(value);        
        FB::variant ret = test->Invoke(method, args);        
        CHECK(ret.cast<std::string>() == helper::toString(value));
    }

    {
        const std::string method("sumOf");
        CHECK(test->HasMethod(method));
        int values[2] = {0, 42};
        for(int i=0; i<5; ++i, ++values[0], --values[1])
        {
            FB::VariantList args(values, values + sizeof(values)/sizeof(int));
            FB::variant ret = test->Invoke(method, args);        
            CHECK(ret.cast<int>() == (values[0]+values[1]));
        }
    }

    {
        const std::string method("concatenate");
        CHECK(test->HasMethod(method));
        const std::string a("push "), b("me "), c("please");
        FB::VariantList args = list_of(a)(b)(c);
        FB::variant ret = test->Invoke(method, args);
        CHECK(ret.cast<std::string>() == (a+b+c));
    }

    {
        // test catching all remaining params with CatchAll as the last argument

        const std::string method("concatMany");
        CHECK(test->HasMethod("concatMany"));

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

    {
        // test that CatchAll doesn't break catching arrays as the last parameter

        const std::string method("concatMany2");
        CHECK(test->HasMethod("concatMany2"));

        const unsigned max_args = 20;
        std::vector<std::string> strings(max_args);
        std::generate(strings.begin(), strings.end(), helper::IncNumber());
                
        for(unsigned i=2; i<=max_args; ++i)
        {
            boost::shared_ptr<FakeJsArray> jsarr(new FakeJsArray(make_variant_list(strings.begin()+1, strings.begin()+i)));
            FB::VariantList params = variant_list_of(strings.front())(ptr_cast<JSObject>(jsarr));
            FB::variant ret = test->Invoke(method, params);
            const std::string expected = std::accumulate(strings.begin(), strings.begin()+i, std::string(""));
            const std::string result   = ret.cast<std::string>();
            CHECK(result == expected);
        }
    }

    {
        const std::string prop("message");
        CHECK(test->HasProperty(prop));
        const std::string value("hi there");
        test->SetProperty(prop, value);
        FB::variant ret = test->GetProperty(prop);
        CHECK(ret.cast<std::string>() == value);
    }

    {
        const std::string prop("readOnlyMessage");
        CHECK(test->HasProperty(prop));
        const FB::variant ret1 = test->GetProperty(prop);
        test->SetProperty(prop, ret1.convert_cast<std::string>()+"foo");
        const FB::variant ret2 = test->GetProperty(prop);
        CHECK(ret1.cast<std::string>() == ret2.convert_cast<std::string>());
    }

    {
        // Test variant parameter type
        const std::string method("getType");
        CHECK(test->HasMethod(method));
        
        FB::variant ret = test->Invoke(method, FB::variant_list_of((long)12));
        CHECK(ret.cast<std::string>() == typeid(long).name());

        ret = test->Invoke(method, FB::variant_list_of((double)12.4));
        CHECK(ret.cast<std::string>() == typeid(double).name());

        ret = test->Invoke(method, FB::variant_list_of((bool)true));
        CHECK(ret.cast<std::string>() == typeid(bool).name());

        // Test explicit assignment
        ret = test->Invoke(method, FB::variant_list_of(FB::variant((void *)0x12, true)));
        CHECK(ret.cast<std::string>() == typeid(void*).name());
    }

    {
        // test array conversions
        const std::string method("accumulate");
        std::vector<int> values = list_of((int)1)(2)(3)(42);
        boost::shared_ptr<FakeJsArray> jsarr(new FakeJsArray(make_variant_list(values)));
        FB::variant varJsArr = FB::JSObjectPtr(jsarr);

        FB::variant ret = test->Invoke(method, variant_list_of(varJsArr));
        CHECK(ret.cast<long>() == std::accumulate(values.begin(), values.end(), 0));
    }

    {
        // test returning container

        FB::VariantList values = FB::variant_list_of(1)(2)(3);
        FB::variant var = test->Invoke("container", values);
        CHECK(var.is_of_type<FB::VariantList>());
        
        typedef std::vector<long> LongVec;
        LongVec orig   = FB::convert_variant_list<LongVec>(values);
        LongVec result = FB::convert_variant_list<LongVec>(var.convert_cast<FB::VariantList>());        
        CHECK(orig == result);
    }
    
    {
        const std::string description = "asdf123541234mlkasdf";
        boost::shared_ptr<FB::JSAPIAuto> x = boost::make_shared<FB::JSAPIAuto>(description);
        CHECK_EQUAL(description, x->ToString());
        const std::string tmp(x->Invoke("toString", FB::VariantList()).convert_cast<std::string>());
        CHECK_EQUAL(description, tmp);
    }

    
    {
        const std::string method("callMethodWith11Parameters");
        CHECK(test->HasMethod(method));        
        FB::VariantList args = FB::variant_list_of(1)(2)(3)(4)(5)(6)(7)(8)(9)(10)(11);            
        const std::string ret ( test->Invoke(method, args).convert_cast<std::string>());       
        CHECK_EQUAL(ret, "66");        
    }

    {
        const std::string method("callMethodWith12Parameters");
        CHECK(test->HasMethod(method));        
        FB::VariantList args = FB::variant_list_of(1)(2)(3)(4)(5)(6)(7)(8)(9)(10)(11)(12);            
        const std::string ret ( test->Invoke(method, args).convert_cast<std::string>());       
        CHECK_EQUAL(ret, "78");        
    }

    {
        const std::string method("callMethodWith13Parameters");
        CHECK(test->HasMethod(method));        
        FB::VariantList args = FB::variant_list_of(1)(2)(3)(4)(5)(6)(7)(8)(9)(10)(11)(12)(12);            
        const std::string ret ( test->Invoke(method, args).convert_cast<std::string>());       
        CHECK_EQUAL(ret, "90");        
    }
    {
        const std::string method("callMethodWith14Parameters");
        CHECK(test->HasMethod(method));        
        FB::VariantList args = FB::variant_list_of(1)(2)(3)(4)(5)(6)(7)(8)(9)(10)(11)(12)(12)(10);            
        const std::string ret ( test->Invoke(method, args).convert_cast<std::string>());       
        CHECK_EQUAL(ret, "100");        
    }
    {
        const std::string method("callMethodWith15Parameters");
        CHECK(test->HasMethod(method));        
        FB::VariantList args = FB::variant_list_of(1)(2)(3)(4)(5)(6)(7)(8)(9)(10)(11)(12)(12)(10)(50);            
        const std::string ret ( test->Invoke(method, args).convert_cast<std::string>());       
        CHECK_EQUAL(ret, "150");        
    }
}

