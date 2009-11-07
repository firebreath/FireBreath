/**********************************************************\ 
Original Author: Georg Fritzsche

Created:    November 7, 2009
License:    Eclipse Public License - Version 1.0
            http://www.eclipse.org/legal/epl-v10.html

Copyright 2009 Georg Fritzsche, Firebreath development team
\**********************************************************/

#include <vector>
#include "TestJSAPIAuto.h"

TEST(JSAPIAuto_Methods)
{
	PRINT_TESTNAME;

	FB::AutoPtr<FB::JSAPI> test = new TestObjectJSAPIAuto;

	{
		std::vector<FB::variant> args;
		args.push_back("foo");
		
		FB::variant ret = test->Invoke("returnString", args);		
		CHECK(ret.convert_cast<std::string>() == "foo");
	}

	{
		std::vector<FB::variant> args;
		args.push_back(23);
		
		FB::variant ret = test->Invoke("intToString", args);		
		CHECK(ret.convert_cast<std::string>() == "23");
	}

	{
		std::vector<FB::variant> args;
		args.push_back(17);
		args.push_back(25);
		
		FB::variant ret = test->Invoke("sumOf", args);		
		CHECK(ret.convert_cast<int>() == 42);
	}

	{
		std::vector<FB::variant> args;
		args.push_back("push ");
		args.push_back("me");

		FB::variant ret = test->Invoke("concatenate", args);
		CHECK(ret.convert_cast<std::string>() == "push me");
	}
}