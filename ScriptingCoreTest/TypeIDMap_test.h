/**********************************************************\ 
Original Author: Richard Bateman (taxilian)

Created:    Sept 21, 2009
License:    Eclipse Public License - Version 1.0
            http://www.eclipse.org/legal/epl-v10.html

Copyright 2009 Richard Bateman, Firebreath development team
\**********************************************************/


#include "TypeIDMap.h"

using namespace FB;

TEST(TypeIDMapTest_voidptr)
{
    PRINT_TESTNAME;

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