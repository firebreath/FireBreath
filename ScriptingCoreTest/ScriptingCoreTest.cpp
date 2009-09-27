// ScriptingCoreTest.cpp : Defines the entry point for the console application.
//

#include "UnitTest++.h"

#include "variant_test.h"
#include "autoptr_test.h"

using namespace std;
using namespace cdiggins;

int main()
{
    return UnitTest::RunAllTests();

    return 0;
}
