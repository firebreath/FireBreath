/**********************************************************\ 
Original Author: Georg Fritzsche

Created:    November 7, 2009
License:    Eclipse Public License - Version 1.0
            http://www.eclipse.org/legal/epl-v10.html

Copyright 2009 Georg Fritzsche, Firebreath development team
\**********************************************************/

#include <vector>
#include <sstream>
#include <boost/assign.hpp>
#include "TestJSAPIAuto.h"

namespace helper
{
	template<class T>
	std::string toString(const T t)
	{
		std::stringstream ss;
		ss << t;
		return ss.str();
	}
}

TEST(JSAPIAuto_Methods)
{
	PRINT_TESTNAME;

	using boost::assign::list_of;
	FB::AutoPtr<FB::JSAPI> test = new TestObjectJSAPIAuto;

	{
		const std::string method("returnString");
		CHECK(test->HasMethod(method));
		const std::string value("foo");
		FB::VariantList args = list_of(value);		
		FB::variant ret = test->Invoke(method, args);		
		CHECK(ret.cast<std::string>() == "foo");
	}

	{
		const std::string method("intToString");
		CHECK(test->HasMethod(method));
		const int value = 23;
		FB::VariantList args = list_of(value);		
		FB::variant ret = test->Invoke(method, args);		
		CHECK(ret.cast<std::string>() == helper::toString(value));
	}

	{
		const std::string method("sumOf");
		CHECK(test->HasMethod(method));
		int values[2] = {0, 42};
		for(int i=0; i<5; ++i, ++values[0], --values[1])
		{
			FB::VariantList args(values, values + sizeof(values)/sizeof(int));
			FB::variant ret = test->Invoke(method, args);		
			CHECK(ret.cast<int>() == (values[0]+values[1]));
		}
	}

	{
		const std::string method("concatenate");
		CHECK(test->HasMethod(method));
		const std::string a("push "), b("me "), c("please");
		FB::VariantList args = list_of(a)(b)(c);
		FB::variant ret = test->Invoke(method, args);
		CHECK(ret.cast<std::string>() == (a+b+c));
	}

	{
		const std::string method("concatMany");
		CHECK(test->HasMethod("concatMany"));
		const std::string a("a"), b("b"), c("c"), d("d");
		FB::VariantList args = list_of(a)(b)(c)(d);
		FB::variant ret = test->Invoke(method, args);
		CHECK(ret.cast<std::string>() == (a+b+c+d));
	}

	{
		const std::string prop("message");
		CHECK(test->HasProperty(prop));
		const std::string value("hi there");
		test->SetProperty(prop, value);
		FB::variant ret = test->GetProperty(prop);
		CHECK(ret.cast<std::string>() == value);
	}

	{
		const std::string prop("readOnlyMessage");
		CHECK(test->HasProperty(prop));
		const FB::variant ret1 = test->GetProperty(prop);
		test->SetProperty(prop, ret1.convert_cast<std::string>()+"foo");
		const FB::variant ret2 = test->GetProperty(prop);
		CHECK(ret1.cast<std::string>() == ret2.convert_cast<std::string>());
	}

	{
		// Test variant parameter type
		const std::string method("getType");
		CHECK(test->HasMethod(method));
        
		FB::variant ret = test->Invoke(method, FB::VariantList(list_of((long)12)));
		CHECK(ret.cast<std::string>() == "long");

		ret = test->Invoke(method, FB::VariantList(list_of((double)12.4)));
		CHECK(ret.cast<std::string>() == "double");

		ret = test->Invoke(method, FB::VariantList(list_of((bool)true)));
		CHECK(ret.cast<std::string>() == "bool");

		ret = test->Invoke(method, FB::VariantList(list_of((void *)0x12)));
		CHECK(ret.cast<std::string>() == "void *");
	}
}
