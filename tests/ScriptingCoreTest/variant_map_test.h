/**********************************************************\ 
Original Author: Georg Fritzsche

Created:    Dec 13, 2009
License:    Dual license model; choose one of two:
            New BSD License
            http://www.opensource.org/licenses/bsd-license.php
            - or -
            GNU Lesser General Public License, version 2.1
            http://www.gnu.org/licenses/lgpl-2.1.html

Copyright 2009 Georg Fritzsche, Firebreath development team
\**********************************************************/


#include <map>
#include "APITypes.h"
#include "variant_map.h"

TEST(VariantMapTest)
{
    PRINT_TESTNAME;

    using FB::convert_variant_map;
    using FB::make_variant_map;
    using FB::VariantMap;

    typedef std::vector<std::string> StringVec;
        
    {
        StringVec names{ "a", "b", "c", "d", "e" };
        VariantMap values{ { "a", "a" }, { "b", "b" }, { "c", "c" }, { "d", "d" }, { "e", "e" } };

        auto itn = names.begin();
        auto itv = values.begin();

        for( ; itn!=names.end() && itv!=values.end(); ++itn, ++itv)
        {
            CHECK(*itn == itv->first);
            CHECK(*itn == itv->second.convert_cast<std::string>());
        }
    }
    {
        StringVec names{ "a", "b", "c", "d", "e" };
        std::map<std::string, std::string> sm{ { "a", "a" }, { "b", "b" }, { "c", "c" }, { "d", "d" }, { "e", "e" } };
        VariantMap vm = make_variant_map(sm);

        StringVec::const_iterator  itn = names.begin();
        VariantMap::const_iterator itv = vm.begin();

        for( ; itn!=names.end() && itv!=vm.end(); ++itn, ++itv)
        {
            CHECK(*itn == itv->first);
            CHECK(*itn == itv->second.convert_cast<std::string>());
        }

        CHECK((sm == convert_variant_map< std::map<std::string, std::string> >(vm)));

        std::map<std::string, std::string> sm2;
        convert_variant_map(vm, sm2);
        CHECK(sm == sm2);
    }
}

TEST(VariantMapWithListTest)
{
    PRINT_TESTNAME;

    using FB::VariantMap;
    using FB::VariantList;

    VariantMap testMap{ { "num", 12 }, { "bobb", "billy" }, { "list", VariantList{ 1, 2, "asdf" } } };
    CHECK(testMap["num"].convert_cast<int>() == 12);
    CHECK(testMap["list"].cast<VariantList>()[0].convert_cast<int>() == 1);
    CHECK(testMap["list"].cast<VariantList>()[1].convert_cast<int>() == 2);
}
