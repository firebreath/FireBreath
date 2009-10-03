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

    FB::AutoPtr<TestObjectJSAPI> test1 = new TestObjectJSAPI();

    test1->Invoke("setValue", vector_list<FB::variant>(0, "This is a test"));

    try {
        FB::variant retVal = test1->Invoke("getValue", vector_list<FB::variant>(0));
        CHECK(retVal.convert_cast<std::string>() == "This is a test");
    } catch (...) {
        CHECK(false);
    }
}