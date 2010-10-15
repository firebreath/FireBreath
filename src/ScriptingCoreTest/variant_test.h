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
#include "variant_list.h"
#include "variant_map.h"
#include "fake_jsarray.h"
#include "fake_jsmap.h"

TEST(VariantTest)
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

    // scripting style array conversion
    {
        typedef std::vector<std::string> StringVec;
        FB::VariantList values = variant_list_of("1")(2)(3.0);
        boost::shared_ptr<FakeJsArray> jsarr(new FakeJsArray(values));
        
        variant varJsArr = ptr_cast<JSObject>(jsarr);
        StringVec vs1 = varJsArr.convert_cast<StringVec>();
        StringVec vs2 = FB::convert_variant_list<StringVec>(values);
        
        CHECK(vs1 == vs2);
    }

    // scripting style map conversion
    {
        typedef std::vector<std::string> StringVec;
        typedef std::map<std::string, FB::variant> VariantMap;
        typedef VariantMap::value_type StringVariantPair;
        typedef std::map<std::string, std::string> StringStringMap;

        VariantMap values = variant_map_of<std::string>("a","a")("b","b")("c","c");
        boost::shared_ptr<FakeJsMap> jsmap(new FakeJsMap(values));
        variant varJsMap = ptr_cast<JSObject>(jsmap);
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

        // Check it with UTF8 source data
        /*unsigned char utf8str[] = {0xc3, 0x91,
                          0xc2, 0x81,
                          0xc3, 0x90,
                          0xc2, 0xba,
                          0xc3, 0x90,
                          0xc2, 0xb0,
                          0xc3, 0x90,
                          0xc2, 0xb6,
                          0xc3, 0x90,
                          0xc2, 0xb8,
                          0x00, 0x00};
        std::string tmpstr((char *)utf8str);

        CHECK(tmpstr == tmp);

        variant str3(tmpstr);
        std::wstring widestr( str3.convert_cast<std::wstring>() );
        CHECK(widestr == lstr);*/
    }

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
