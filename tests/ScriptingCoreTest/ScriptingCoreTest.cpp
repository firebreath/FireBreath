/**********************************************************\ 
Original Author: Richard Bateman (taxilian)

Created:    Sept 25, 2009
License:    Dual license model; choose one of two:
            New BSD License
            http://www.opensource.org/licenses/bsd-license.php
            - or -
            GNU Lesser General Public License, version 2.1
            http://www.gnu.org/licenses/lgpl-2.1.html

Copyright 2009 Richard Bateman, Firebreath development team
\**********************************************************/

#include "UnitTest++.h"

#define PRINT_TESTNAME  printf("Running unit test %s::%s...\n", UnitTestSuite::GetSuiteName(), m_details.testName); \
    fflush(stdout)

#include "meta_test.h"
#include "variant_test.h"
#include "variant_list_test.h"
#include "variant_map_test.h"
// preferred to comment that out rather then 
// fixing as JSAPISimple is deprecated anyway
//#include "jsapi_test.h" 
#include "jsapiauto_test.h"
#include "jsarray_test.h"
#include "TypeIDMap_test.h"
#include "jscallback_test.h"

int main()
{
    return UnitTest::RunAllTests();

    return 0;
}

