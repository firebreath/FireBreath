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
#include "BrowserHost.h"
#include "DOM/Window.h"
#include "variant_list.h"
#include "precompiled_headers.h" // On windows, everything above this line in PCH

#include "JSArray.h"

FB::JSArray::JSArray(const FB::JSObjectPtr& obj)
  : m_obj(obj)
{
    if(!m_obj->HasProperty("length"))
        throw FB::script_error("object does not have property length");
}

FB::JSArray::JSArray(BrowserHostPtr host, const FB::VariantList& values)
{
    m_obj = host->getDOMWindow()->createArray();
    for (FB::VariantList::const_iterator it = values.begin(); it != values.end(); ++it)
    {
        push(*it);
    }
    RegisterMethods();
}


void FB::JSArray::push(FB::variant value)
{
    m_obj->Invoke("push", FB::variant_list_of(value));
}

size_t FB::JSArray::GetLength() const
{
    return m_obj->GetProperty("length").convert_cast<size_t>();
}

FB::variant FB::JSArray::Get(size_t index)
{
    return m_obj->GetProperty(index);
}

void FB::JSArray::Set(const size_t index, const FB::variant& value)
{
    m_obj->SetProperty(index, value);
}

FB::variant FB::JSArray::operator[](size_t index)
{
    return m_obj->GetProperty(index);
}

FB::VariantList FB::JSArray::Values()
{
    FB::VariantList output;
    for (size_t i = 0; i < GetLength(); i++) {
        output.push_back((*this)[i]);
    }
    return output;
}

