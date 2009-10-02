/**********************************************************\ 
Original Author: Richard Bateman (taxilian)

Created:    Sept 25, 2009
License:    Eclipse Public License - Version 1.0
            http://www.eclipse.org/legal/epl-v10.html

Copyright 2009 Richard Bateman, Firebreath development team
\**********************************************************/

#include "UnitTest++.h"

#define PRINT_TESTNAME  printf("Running unit test %s::%s...\n", UnitTestSuite::GetSuiteName(), m_details.testName); \
    fflush(stdout)

#include "variant_test.h"
#include "autoptr_test.h"
#include "jsapi_test.h"

using namespace std;
using namespace cdiggins;

int main()
{
    return UnitTest::RunAllTests();

    return 0;
}
