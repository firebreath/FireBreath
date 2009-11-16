/**********************************************************\ 
Original Author: Georg Fritzsche

Created:    November 7, 2009
License:    Eclipse Public License - Version 1.0
            http://www.eclipse.org/legal/epl-v10.html

Copyright 2009 Georg Fritzsche, Firebreath development team
\**********************************************************/

#include <string>
#include <sstream>
#include "JSAPIAuto.h"

class TestObjectJSAPIAuto : public JSAPIAuto
{
public:
	virtual ~TestObjectJSAPIAuto() {}

	TestObjectJSAPIAuto()
	  : m_readOnlyMessage("read only message")
	{
		registerMethod("returnString",	make_method(this, &TestObjectJSAPIAuto::returnString));
		registerMethod("intToString",	make_method(this, &TestObjectJSAPIAuto::intToString));
		registerMethod("sumOf",			make_method(this, &TestObjectJSAPIAuto::sumOf));
		registerMethod("concatenate",	make_method(this, &TestObjectJSAPIAuto::concatenate));
		registerMethod("concatenate2",	make_method(this, &TestObjectJSAPIAuto::concatenate2));
		registerMethod("getType",		make_method(this, &TestObjectJSAPIAuto::getType));

		registerProperty("message",
						 make_property(this, 
							&TestObjectJSAPIAuto::get_Message,
							&TestObjectJSAPIAuto::set_Message));
		registerProperty("readOnlyMessage", 
						 make_property(this,
							&TestObjectJSAPIAuto::get_ReadOnlyMessage));
	}

	std::string returnString(const std::string& s)
	{
		return s;
	}

	std::string intToString(int i)
	{
		std::stringstream ss;
		ss << i;
		return ss.str();
	}

	int sumOf(int a, int b)
	{
		return a+b;
	}

	std::string concatenate(const std::string& a, const std::string& b, const std::string& c)
	{
		return a+b+c;
	}

	std::string concatenate2(const std::string& a, const FB::VariantList& list)
	{
		FB::VariantList::const_iterator it(list.begin());
		std::string s(a);

		for( ; it != list.end(); ++it)
			s += it->convert_cast<std::string>();

		return s;
	}

	void set_Message(const std::string& s)
	{
		m_message = s;
	}

	std::string get_Message()
	{
		return m_message;
	}

	std::string get_ReadOnlyMessage()
	{
		return m_readOnlyMessage;
	}

    std::string getType(const FB::variant a)
    {
        return a.get_type().name();
    }

private:
	std::string m_message;
	const std::string m_readOnlyMessage;
};
