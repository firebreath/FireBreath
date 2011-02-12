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


#include "TypeIDMap.h"

TEST(TypeIDMapTest_voidptr)
{
    PRINT_TESTNAME;

    using namespace FB;

    TypeIDMap<void *> idMap((void*)0x1);

    void *tmp = idMap.getIdForValue("This is a test");
    void *tmp2 = idMap.getIdForValue(17);
    CHECK(tmp == (void *)0x1);
    CHECK(idMap.idIsType<std::string>(tmp));
    CHECK(tmp2 == (void *)0x2);
    CHECK(idMap.idIsType<int>(tmp2));

    std::string val = idMap.getValueForId<std::string>(tmp);
    int val2 = idMap.getValueForId<int>(tmp2);

    CHECK(val == "This is a test");
    CHECK(val2 == 17);
}

TEST(TypeIDMapTest_int)
{
    PRINT_TESTNAME;
    using namespace FB;

    TypeIDMap<int> idMap(1);

    int tmp = idMap.getIdForValue("This is a test");
    int tmp2 = idMap.getIdForValue(17);
    CHECK(tmp == 1);
    CHECK(idMap.idIsType<std::string>(tmp));
    CHECK(tmp2 == 2);
    CHECK(idMap.idIsType<int>(tmp2));

    std::string val = idMap.getValueForId<std::string>(tmp);
    int val2 = idMap.getValueForId<int>(tmp2);

    CHECK(val == "This is a test");
    CHECK(val2 == 17);
}

TEST(TypeIDMapTest_setid)
{
    PRINT_TESTNAME;
    using namespace FB;

    TypeIDMap<int> idMap(1);

    std::string val1("This is a string");
    std::string val2("This is also a string");
    int val3(42);
    double val4(34.55);

    idMap.setIdForValue(-55, val1);
    idMap.setIdForValue(-54, val2);
    idMap.setIdForValue(-53, val3);
    idMap.setIdForValue(-52, val4);
    
    CHECK(idMap.getIdForValue(val1) == -55);
    CHECK(idMap.getIdForValue(val2) == -54);
    CHECK(idMap.getIdForValue(val3) == -53);
    CHECK(idMap.getIdForValue(val4) == -52);
}

TEST(TypeIDMapTest_testlong)
{
    PRINT_TESTNAME;
    using namespace FB;

    TypeIDMap<long> idMap(100);
    idMap.setIdForValue(0, "value");
    idMap.setIdForValue(-3, "propertyput");
    idMap.setIdForValue(-4, "newenum");
    idMap.setIdForValue(-5, "evaluate");
    idMap.setIdForValue(-6, "constructor");
    idMap.setIdForValue(-7, "destructor");
    idMap.setIdForValue(-8, "collect");
    idMap.setIdForValue(-500, "autosize");
    idMap.setIdForValue(-501, "backcolor");
}

