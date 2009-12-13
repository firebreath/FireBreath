/**********************************************************\ 
Original Author: Georg Fritzsche

Created:    December 13, 2009
License:    Dual license model; choose one of two:
            Eclipse Public License - Version 1.0
            http://www.eclipse.org/legal/epl-v10.html
            - or -
            GNU Lesser General Public License, version 2.1
            http://www.gnu.org/licenses/lgpl-2.1.html

Copyright 2009 Georg Fritzsche, Firebreath development team
\**********************************************************/

#include <vector>
#include <sstream>
#include <numeric>
#include <boost/assign/list_of.hpp>
#include <boost/lexical_cast.hpp>
#include "variant_list.h"
#include "Util/JSArray.h"

TEST(JSArrayTest)
{
    PRINT_TESTNAME;

	using boost::assign::list_of;
	using FB::variant_list_of;

	{
		// test C++ acess 

		FB::VariantList values = variant_list_of(1)("3")(2.3);
		FB::AutoPtr<FB::JSArray> test = new FB::JSArray(values);
		
		std::vector<FB::VariantList> copies = boost::assign::list_of
			(test->CopyValues())
			(FB::VariantList())
			(test->ToVectorOf<FB::variant>())
			(test->ToContainer<FB::VariantList>());
		test->ToContainer(copies[1]);

		for(size_t i=0; i<values.size(); ++i) 
		{
			std::vector<FB::variant> to_test = variant_list_of((*test)[i])(test->Values()[i]);			
			for(size_t c=0; c<copies.size(); ++c) 
				to_test.push_back(copies[c][i]);

			for(size_t t=0; t<to_test.size(); ++t) 
			{
				CHECK(values[i].get_type() == to_test[t].get_type());
				CHECK(values[i].convert_cast<std::string>() == to_test[t].convert_cast<std::string>());
			}
		}

		// test scripting acess

		CHECK(values.size() == test->GetProperty("length").convert_cast<size_t>());

		for(size_t i=0; i<values.size(); ++i)
		{
			CHECK(values[i].get_type() == test->GetProperty(i).get_type());
			CHECK(values[i].convert_cast<std::string>() == test->GetProperty(i).convert_cast<std::string>());
		}
	}
}
