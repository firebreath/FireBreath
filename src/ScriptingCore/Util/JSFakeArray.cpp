/**********************************************************\ 
Original Author: Georg Fritzsche

Created:    December 13, 2009
License:    Dual license model; choose one of two:
            New BSD License
            http://www.opensource.org/licenses/bsd-license.php
            - or -
            GNU Lesser General Public License, version 2.1
            http://www.gnu.org/licenses/lgpl-2.1.html

Copyright 2009 Georg Fritzsche, Firebreath development team
\**********************************************************/

#include <iterator>
#include "precompiled_headers.h"

#include "JSFakeArray.h"

FB::JSFakeArray::JSFakeArray()
  : m_values()
{
    RegisterMethods();
}

FB::JSFakeArray::JSFakeArray(const FB::JSObjectPtr& obj)
  : m_values()
{
    if(!obj->HasProperty("length"))
        throw FB::script_error("object does not have property length");

    ExtractList(obj);
    RegisterMethods();
}

FB::JSFakeArray::JSFakeArray(const FB::VariantList& values)
{
    m_values = values;
    RegisterMethods();
}

void FB::JSFakeArray::RegisterMethods()
{
    registerProperty("length", make_property(this, &JSFakeArray::GetLength));
}

void FB::JSFakeArray::ExtractList(const FB::JSObjectPtr& obj)
{
    try
    {
        int length = obj->GetProperty("length").convert_cast<int>();
        std::back_insert_iterator<FB::VariantList> inserter(m_values);

        for(int i=0; i<length; ++i) {
            inserter = obj->GetProperty(i);
        }
    }
    catch(const FB::script_error& e) 
    {
        throw e;
    }
}

size_t FB::JSFakeArray::GetLength() const
{
    return m_values.size();
}

FB::variant FB::JSFakeArray::GetProperty(int index)
{
    return (*this)[index];
}

FB::variant& FB::JSFakeArray::operator[](size_t index)
{
    return m_values[index];
}

const FB::variant& FB::JSFakeArray::operator[](size_t index) const
{
    return m_values[index];
}

FB::VariantList& FB::JSFakeArray::Values()
{
    return m_values;
}

const FB::VariantList& FB::JSFakeArray::Values() const
{
    return m_values;
}

