/**********************************************************\ 
Original Author: Richard Bateman (taxilian)

Created:    Sept 21, 2009
License:    Dual license model; choose one of two:
            Eclipse Public License - Version 1.0
            http://www.eclipse.org/legal/epl-v10.html
            - or -
            GNU Lesser General Public License, version 2.1
            http://www.gnu.org/licenses/lgpl-2.1.html

Copyright 2009 Richard Bateman, Firebreath development team
\**********************************************************/


#include "APITypes.h"
#include "variant_list.h"
#include "fake_jsarray.h"

using namespace FB;

TEST(VariantTest)
{
    PRINT_TESTNAME;

    variant a = 23.23;
    
    CHECK(a.convert_cast<int>() == 23);
    CHECK(a.convert_cast<std::string>() == "23.23");
    
    a = 23.23f;
    CHECK(a.get_type() == typeid(float));

    CHECK(a.convert_cast<float>() == 23.23f);
    CHECK(a.convert_cast<std::string>() == "23.23");

    a = "This is a test";

    CHECK(a.cast<std::string>() == "This is a test");

    a = 47;
    CHECK(a.convert_cast<std::string>() == "47");
    
    a = "37.23";
    CHECK(a.convert_cast<double>() == 37.23);
    CHECK(a.convert_cast<int>() == 37);
    CHECK(a.convert_cast<long>() == 37);
    CHECK(a.convert_cast<short>() == 37);

    a = "true";
    CHECK(a.convert_cast<bool>());
    a = "t";
    CHECK(a.convert_cast<bool>());
    a = "1";
    CHECK(a.convert_cast<bool>());
    a = "yes";
    CHECK(a.convert_cast<bool>());
    a = "y";
    CHECK(a.convert_cast<bool>());
    a = "f";
    CHECK(!a.convert_cast<bool>());

    a = "This is a test";
    variant b = a;
    variant c = b;
    variant d = c;
    CHECK(d.convert_cast<std::string>() == "This is a test");

    a = NULL;
    CHECK(a.get_type() == typeid(int));

    {
        typedef std::vector<std::string> StringVec;
        FB::VariantList values = variant_list_of("1")(2)(3.0);
        FB::AutoPtr<FakeJsArray> jsarr(new FakeJsArray(values));
        
        variant varJsArr = FB::AutoPtr<BrowserObjectAPI>(jsarr);
        StringVec vs1 = varJsArr.convert_cast<StringVec>();
        StringVec vs2 = FB::convert_variant_list<StringVec>(values);
        
        CHECK(vs1 == vs2);
    }
}