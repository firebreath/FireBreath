/**********************************************************\ 
Original Author: Richard Bateman (taxilian)

Created:    Sept 25, 2009
License:    Dual license model; choose one of two:
            Eclipse Public License - Version 1.0
            http://www.eclipse.org/legal/epl-v10.html
            - or -
            GNU Lesser General Public License, version 2.1
            http://www.gnu.org/licenses/lgpl-2.1.html

Copyright 2009 Richard Bateman, Firebreath development team
\**********************************************************/

#include "UnitTest++.h"

#define PRINT_TESTNAME  printf("Running unit test %s::%s...\n", UnitTestSuite::GetSuiteName(), m_details.testName); \
    fflush(stdout)

#include "variant_test.h"
#include "autoptr_test.h"
#include "jsapi_test.h"
#include "jsapiauto_test.h"
#include "jsarray_test.h"
#include "TypeIDMap_test.h"

using namespace std;
using namespace FB;

int main()
{
    return UnitTest::RunAllTests();

    return 0;
}
