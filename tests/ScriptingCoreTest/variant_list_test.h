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


#include "APITypes.h"
#include "variant_list.h"
#include "variant_map.h"

TEST(VariantListTest)
{
    PRINT_TESTNAME;

    using FB::make_variant_list;
    using FB::convert_variant_list;

    {
        FB::VariantList vl;
        vl.emplace_back(1);
        vl.emplace_back("foo");
        vl.emplace_back(25.74);
        vl.emplace_back(-1);

        std::vector<FB::VariantList> to_test{
            make_variant_list(vl), FB::VariantList{ 1, "foo", 25.74, -1 }
        };

        for(size_t i=0; i<vl.size(); ++i)
        {
            for(size_t t=0; t<to_test.size(); ++t)
            {
                CHECK(vl[i].get_type() == to_test[t][i].get_type());
                CHECK(vl[i].convert_cast<std::string>() == to_test[t][i].convert_cast<std::string>());
            }
        }
    }

    {
        std::vector<std::string> sl{ "1", "2", "3", "4" };
        FB::VariantList vl = make_variant_list(sl);

        for(size_t i=0; i<vl.size(); ++i) 
        {
            CHECK(vl[i].get_type() == typeid(std::string));
            CHECK(vl[i].convert_cast<std::string>() == sl[i]);
        }

        CHECK(sl == convert_variant_list< std::vector<std::string> >(vl));

        std::vector<std::string> sl2;
        convert_variant_list(vl, sl2);
        CHECK(sl == sl2);
    }
}

TEST(VariantListWithMapTest)
{
    PRINT_TESTNAME;

    using FB::VariantList;
    using FB::VariantMap;

    VariantList testList = VariantList{ 23, "asdf", VariantMap{ { "num", 12 }, { "bobb", "billy" } } };
    CHECK(testList[0].convert_cast<int>() == 23);
    VariantMap tmp = testList[2].convert_cast<VariantMap>();
    CHECK(tmp["bobb"].convert_cast<std::string>() == "billy");
    CHECK(tmp["num"].convert_cast<int>() == 12);
}
