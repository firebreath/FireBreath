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

#include "JSArray.h"

FB::JSArray::JSArray()
  : m_values()
{
	RegisterMethods();
}

FB::JSArray::JSArray(FB::AutoPtr<FB::BrowserObjectAPI> obj)
  : m_values()
{
	if(!obj->HasProperty("length"))
		throw FB::script_error("object does not have property length");

	try
	{
		int length = obj->GetProperty("length").convert_cast<int>();
		std::back_insert_iterator<FB::VariantList> inserter(m_values);

		for(int i=0; i<length; ++i) {
			inserter = obj->GetProperty(i);
		}
	}
	catch(FB::script_error& e) 
	{
		throw e;
	}

	RegisterMethods();
}

FB::JSArray::JSArray(const FB::VariantList& values)
{
	m_values = values;
	RegisterMethods();
}

void FB::JSArray::RegisterMethods()
{
	registerProperty("length", make_property(this, &JSArray::GetLength));
}

size_t FB::JSArray::GetLength() const
{
	return m_values.size();
}

FB::variant FB::JSArray::GetProperty(int index)
{
	return (*this)[index];
}

FB::variant& FB::JSArray::operator[](size_t index)
{
	return m_values[index];
}

const FB::variant& FB::JSArray::operator[](size_t index) const
{
	return m_values[index];
}

FB::VariantList& FB::JSArray::Values()
{
	return m_values;
}

const FB::VariantList& FB::JSArray::Values() const
{
	return m_values;
}

FB::VariantList FB::JSArray::CopyValues() const
{
	return m_values;
}
