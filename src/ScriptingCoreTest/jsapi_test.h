/**********************************************************\ 
Original Author: Richard Bateman (taxilian)

Created:    Oct 1, 2009
License:    Eclipse Public License - Version 1.0
            http://www.eclipse.org/legal/epl-v10.html

Copyright 2009 Richard Bateman, Firebreath development team
\**********************************************************/


#include "JSAPI.h"
#include "TestJSAPI.h"
#include "vector_list.h"

TEST(JSAPI_Methods)
{
    PRINT_TESTNAME;

    FB::AutoPtr<FB::JSAPI> test1 = new TestObjectJSAPI();

    // Test setting and then getting a string value
    test1->Invoke("setValue", vector_list<FB::variant>(0, "This is a test"));

    try {
        FB::variant retVal = test1->Invoke("getValue", vector_list<FB::variant>(0));
        CHECK(retVal.convert_cast<std::string>() == "This is a test");
    } catch (...) {
        CHECK(false);
    }

    // Test setting and then getting integer values
    for (int i = 0; i < TESTOBJECTJSAPI_ACCESSLISTLENGTH; i++) {
        test1->Invoke("setValue", vector_list<FB::variant>(i, i));

        FB::variant retVal = test1->Invoke("getValue", vector_list<FB::variant>(i));
        CHECK(retVal.convert_cast<int>() == i);
    }

    // Test setting and then getting double values
    for (int i = 0; i < TESTOBJECTJSAPI_ACCESSLISTLENGTH; i++) {
        test1->Invoke("setValue", vector_list<FB::variant>(i, (double)(i * 1.5)));

        FB::variant retVal = test1->Invoke("getValue", vector_list<FB::variant>(i));
        CHECK(retVal.convert_cast<double>() == (double)(i * 1.5));
    }

    // Test invalid parameters
    try {
        test1->Invoke("setValue", std::vector<FB::variant>());
        CHECK(false);
    } catch (invalid_arguments e) {
        CHECK(true);
    } catch (object_invalidated e) {
        CHECK(false);
    } catch (invalid_member e) {
        CHECK(false);
    } catch (script_error e) {
        CHECK(false);
    }

    // Test invalid method name
    try {
        test1->Invoke("someMethodThatDoesntExist", std::vector<FB::variant>());
        CHECK(false);
    } catch (invalid_arguments e) {
        CHECK(false);
    } catch (object_invalidated e) {
        CHECK(false);
    } catch (invalid_member e) {
        CHECK(true);
    } catch (script_error e) {
        CHECK(false);
    }

    // Test invalidated object
    test1->invalidate();
    try {
        test1->Invoke("setValue", vector_list<FB::variant>(0, "This is a test"));
        CHECK(false);
    } catch (invalid_arguments e) {
        CHECK(false);
    } catch (object_invalidated e) {
        CHECK(true);
    } catch (invalid_member e) {
        CHECK(false);
    } catch (script_error e) {
        CHECK(false);
    }

    // Test that all exceptions are caught by script_error as well
    try {
        test1->Invoke("setValue", vector_list<FB::variant>(0, "This is a test"));
        CHECK(false);
    } catch (script_error e) {
        CHECK(true);
    }
    test1 = new TestObjectJSAPI();
    try {   // invalid parameters
        test1->Invoke("setValue", vector_list<FB::variant>(0));
        CHECK(false);
    } catch (script_error e) {
        CHECK(true);
    }
}

TEST(JSAPI_Properties)
{
    PRINT_TESTNAME;

    FB::AutoPtr<FB::JSAPI> test1 = new TestObjectJSAPI();
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
    } catch (invalid_arguments e) {
        CHECK(false);
    } catch (object_invalidated e) {
        CHECK(false);
    } catch (invalid_member e) {
        CHECK(true);
    } catch (script_error e) {
        CHECK(false);
    }

    try {
        test1->GetProperty("SomePropertyThatDoesntExist");
        CHECK(false);
    } catch (script_error e) {
        CHECK(true);
    }
}