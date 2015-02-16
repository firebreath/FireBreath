/**********************************************************\
Original Author: Richard Bateman (taxilian)

Created:    Sept 21, 2009
License:    Dual license model; choose one of two:
            New BSD License
            http://www.opensource.org/licenses/bsd-license.php
            - or -
            GNU Lesser General Public License, version 2.1
            http://www.gnu.org/licenses/lgpl-2.1.html

Copyright 2009 Richard Bateman, Firebreath development team
\**********************************************************/


#include "APITypes.h"
#include <boost/optional.hpp>
#include "variant_list.h"
#include "variant_map.h"
#include "fake_jsarray.h"
#include "fake_jsmap.h"

TEST(VariantBasicTest)
{
    PRINT_TESTNAME;
    using namespace FB;

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

    a = 0;
    CHECK(a.get_type() == typeid(int));
}
TEST(VariantArrayConversionTest)
{
    PRINT_TESTNAME;
    using namespace FB;

    // scripting style array conversion
    {
        typedef std::vector<std::string> StringVec;
        FB::VariantList values{ "1", 2, 3.0 };
        std::shared_ptr<FakeJsArray> jsarr(new FakeJsArray(values));
        
        variant varJsArr = std::dynamic_pointer_cast<JSObject>(jsarr);
        StringVec vs1 = varJsArr.convert_cast<StringVec>();
        StringVec vs2 = FB::convert_variant_list<StringVec>(values);
        
        CHECK(vs1 == vs2);
    }
}
TEST(VariantScriptMapConversionTest)
{
    PRINT_TESTNAME;
    using namespace FB;

    // scripting style map conversion
    {
        typedef std::vector<std::string> StringVec;
        typedef std::map<std::string, FB::variant> VariantMap;
        typedef VariantMap::value_type StringVariantPair;
        typedef std::map<std::string, std::string> StringStringMap;

        VariantMap values{ { "a", "a" }, { "b", "b" }, { "c", "c" } };
        std::shared_ptr<FakeJsMap> jsmap(new FakeJsMap(values));
        variant varJsMap = std::dynamic_pointer_cast<JSObject>(jsmap);
        VariantMap result = varJsMap.convert_cast<VariantMap>();

        VariantMap::const_iterator itval = values.begin();
        VariantMap::const_iterator itres = result.begin();

        for( ; itval != values.end(); ++itval, ++itres) 
        {
            CHECK(itres != result.end());
            const StringVariantPair& value  = *itval;
            const StringVariantPair& result = *itres;
            CHECK(value.first == result.first);
            CHECK(value.second.convert_cast<std::string>() == result.second.convert_cast<std::string>());
        }
    }
}
TEST(VariantWideStringTest)
{
    PRINT_TESTNAME;
    using namespace FB;

    // wstring type handling and conversion
    {
        variant wstr1 = L"This is a test string";
        CHECK(wstr1.convert_cast<std::wstring>() == L"This is a test string");
        CHECK(wstr1.convert_cast<std::string>() == "This is a test string");
        variant str1 = "This is a single byte string";
        CHECK(str1.convert_cast<std::wstring>() == L"This is a single byte string");

        // Check another language
        variant wstr2 = L"это работает?";
        CHECK(wstr2.convert_cast<std::wstring>() == L"это работает?");

        // Try it when wide data accidently gets stuck in a single byte string
        variant str2 = "это работает?";
        CHECK(wstr2.convert_cast<std::wstring>() == L"это работает?");

        std::wstring lstr(L"скажи");
        variant str4(lstr);
        std::string tmp = str4.convert_cast<std::string>();
#ifdef FB_MACOSX
        CHECK(tmp == "скажи");
#endif
    }
}
TEST(VariantIsOfTypeTest)
{
    PRINT_TESTNAME;
    using namespace FB;

    // is_of_type<>()
    {
        const std::string stringVal = "foo";
        const long longVal = 1;
        const char charArrVal[] = "bar";
        
        FB::variant stringVar(stringVal);
        CHECK(( stringVar.is_of_type<std::string>() ));

        FB::variant longVar(longVal);
        CHECK(( longVar.is_of_type<long>() ));

        FB::variant charArrVar(charArrVal);
        CHECK(( charArrVar.is_of_type<std::string>() ));
    }
}
TEST(VariantOptionalTest)
{
    PRINT_TESTNAME;
    using namespace FB;

    // is_of_type<>()
    {
        boost::optional<int> iVal;
        boost::optional<std::string> strVal;
        boost::optional<double> dblVal;

        variant a = 15;
        variant b = "A string";
        variant c = 23.55f;
        iVal = a.convert_cast<boost::optional<int> >();
        CHECK(iVal && *iVal == a.convert_cast<int>());
        
        strVal = b.convert_cast<boost::optional<std::string> >();
        CHECK(strVal && *strVal == b.convert_cast<std::string>());
        
        dblVal = c.convert_cast<boost::optional<double> >();
        CHECK(dblVal && *dblVal == c.convert_cast<double>());

        iVal = variant().convert_cast<boost::optional<int> >();
        CHECK(!iVal);
        strVal = variant().convert_cast<boost::optional<std::string> >();
        CHECK(!strVal);
        dblVal = variant().convert_cast<boost::optional<double> >();
        CHECK(!dblVal);
    }
}
TEST(VariantTriBool)
{
	PRINT_TESTNAME;
	using namespace FB;

	variant e;
	variant i = boost::tribool(boost::indeterminate);
	variant t = boost::tribool(true);
	variant f = boost::tribool(false);

	// boost::indeterminate is converted to an empty variant
	// empty variants can be converted back to boost::tribool
	CHECK(i.empty());	
	CHECK(boost::indeterminate(i.convert_cast<boost::tribool>()));
	CHECK(boost::indeterminate(e.convert_cast<boost::tribool>()));
	CHECK(t.convert_cast<boost::tribool>() == boost::tribool(true));
	CHECK(f.convert_cast<boost::tribool>() == boost::tribool(false));
	CHECK(t.convert_cast<bool>() == true);
	CHECK(f.convert_cast<bool>() == false);

	variant bt = bool(true);
	variant bf = bool(false);
	CHECK(bt.convert_cast<boost::tribool>());
	CHECK(!bf.convert_cast<boost::tribool>());
}
class SomeTest
{
    char isCtored;
public:
    SomeTest() : isCtored(true) {};
    ~SomeTest() { isCtored = false; }
    SomeTest& operator=(SomeTest const& other) {
        CHECK(isCtored);
        return *this;
    }
    bool operator<(const SomeTest& rh) const {
        return ((void*)this) < ((void*)&rh);
    }
};

TEST(TestVariantWithFunkyAssignmentOperator)
{
    PRINT_TESTNAME;
    using namespace FB;

    SomeTest t1;
    SomeTest t2;
    SomeTest t3;

    FB::variant a(t1, true);
    FB::variant b(t2, true);
    FB::variant c;
    c.assign(t3, true);

    a.assign(t3, true);
    b.assign(t1, true);
    c.assign(t2, true);

    a = b;
    b = c;
    c = a;
}