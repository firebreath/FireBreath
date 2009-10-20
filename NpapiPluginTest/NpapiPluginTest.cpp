/**********************************************************\ 
Original Author: Richard Bateman (taxilian)

Created:    Oct 19, 2009
License:    Eclipse Public License - Version 1.0
            http://www.eclipse.org/legal/epl-v10.html

Copyright 2009 Richard Bateman, Firebreath development team
\**********************************************************/

#include "UnitTest++.h"

#define PRINT_TESTNAME  printf("Running unit test %s::%s...\n", UnitTestSuite::GetSuiteName(), m_details.testName); \
    fflush(stdout)

#include "NPJavascriptObjectTest.h"

using namespace std;

int main()
{
    return UnitTest::RunAllTests();

    return 0;
}
