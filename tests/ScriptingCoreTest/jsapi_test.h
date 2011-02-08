/**********************************************************\ 
Original Author: Richard Bateman (taxilian)

Created:    Oct 1, 2009
License:    Dual license model; choose one of two:
            New BSD License
            http://www.opensource.org/licenses/bsd-license.php
            - or -
            GNU Lesser General Public License, version 2.1
            http://www.gnu.org/licenses/lgpl-2.1.html

Copyright 2009 Richard Bateman, Firebreath development team
\**********************************************************/


#include <boost/shared_ptr.hpp>
#include "variant_list.h"
#include "JSAPI.h"
#include "TestJSAPI.h"

TEST(JSAPI_Methods)
{
    PRINT_TESTNAME;

    using namespace FB;

    boost::shared_ptr<FB::JSAPI> test1(new TestObjectJSAPI());

    // Test setting and then getting a string value
    {
        const std::string msg("This is a test");
        VariantList args = variant_list_of(0)(msg);
        test1->Invoke("setValue", args);

        try {
            FB::variant retVal = test1->Invoke("getValue", variant_list_of(0));
            CHECK(retVal.convert_cast<const std::string>() == msg);
        } catch (...) {
            CHECK(false);
        }
    }

    // Test setting and then getting integer values
    {
        for (int i = 0; i < TESTOBJECTJSAPI_ACCESSLISTLENGTH; ++i) {
            FB::VariantList args = variant_list_of(i)(i);
            test1->Invoke("setValue", args);

            FB::variant retVal = test1->Invoke("getValue", variant_list_of(i));
            CHECK(retVal.convert_cast<int>() == i);
        }
    }

    // Test setting and then getting double values
    {
        for (int i = 0; i < TESTOBJECTJSAPI_ACCESSLISTLENGTH; i++) {
            FB::VariantList args(variant_list_of(i)((double)(i * 1.5)));
            test1->Invoke("setValue", args);

            FB::variant retVal = test1->Invoke("getValue", variant_list_of(i));
            CHECK(retVal.convert_cast<double>() == (double)(i * 1.5));
        }
    }

    // Test invalid parameters
    {
        try {
            test1->Invoke("setValue", FB::VariantList());
            CHECK(false);
        } catch (const invalid_arguments& e) {
            CHECK(true);
        } catch (const object_invalidated& e) {
            CHECK(false);
        } catch (const invalid_member& e) {
            CHECK(false);
        } catch (const script_error& e) {
            CHECK(false);
        }
    }

    // Test invalid method name
    {
        try {
            test1->Invoke("someMethodThatDoesntExist", FB::VariantList());
            CHECK(false);
        } catch (const invalid_arguments& e) {
            CHECK(false);
        } catch (const object_invalidated& e) {
            CHECK(false);
        } catch (const invalid_member& e) {
            CHECK(true);
        } catch (const script_error& e) {
            CHECK(false);
        }
    }

    // Test invalidated object
    {
        test1->invalidate();
        try {
            test1->Invoke("setValue", variant_list_of(0)("This is a test"));
            CHECK(false);
        } catch (const invalid_arguments& e) {
            CHECK(false);
        } catch (const object_invalidated& e) {
            CHECK(true);
        } catch (const invalid_member& e) {
            CHECK(false);
        } catch (const script_error& e) {
            CHECK(false);
        }
    }

    // Test that all exceptions are caught by script_error as well
    {
        try {
            test1->Invoke("setValue", variant_list_of(0)("This is a test"));
            CHECK(false);
        } catch (script_error e) {
            CHECK(true);
        }

        test1 = boost::make_shared<TestObjectJSAPI>();
        try {   // invalid parameters
            test1->Invoke("setValue", variant_list_of(0));
            CHECK(false);
        } catch (const script_error& e) {
            CHECK(true);
        }
    }
}

TEST(JSAPI_Properties)
{
    PRINT_TESTNAME;

    using namespace FB;

    FB::JSAPIPtr test1(new TestObjectJSAPI());
    // Test setting and getting many different datatypes with properties
    try {
        for (int i = 0; i < 10000; i++) {
            test1->SetProperty("value", i);
            CHECK(test1->GetProperty("value").convert_cast<int>() == i);

            test1->SetProperty("value", (double)(i * 3.74));
            CHECK(test1->GetProperty("value").convert_cast<double>() == (double)(i * 3.74));

            std::string tmp = FB::variant((double)(i * 653.251235)).convert_cast<std::string>();
            test1->SetProperty("value", tmp);
            CHECK(test1->GetProperty("value").convert_cast<std::string>() == FB::variant((double)(i * 653.251235)).convert_cast<std::string>());
        }
    } catch (...) {
        CHECK(false);
    }

    // Test exception handling
    try {
        test1->GetProperty("SomePropertyThatDoesntExist");
        CHECK(false);
    } catch (const invalid_arguments& e) {
        CHECK(false);
    } catch (const object_invalidated& e) {
        CHECK(false);
    } catch (const invalid_member& e) {
        CHECK(true);
    } catch (const script_error& e) {
        CHECK(false);
    }

    try {
        test1->GetProperty("SomePropertyThatDoesntExist");
        CHECK(false);
    } catch (const script_error& e) {
        CHECK(true);
    }
}

