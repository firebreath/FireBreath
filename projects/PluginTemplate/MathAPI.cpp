
#include "MathAPI.h"

MathAPI::MathAPI()
{
    registerMethod("returnString",  make_method(this, &MathAPI::returnString));
    registerMethod("intToString",   make_method(this, &MathAPI::intToString));
    registerMethod("sumOf",         make_method(this, &MathAPI::sumOf));
    registerMethod("concatenate",   make_method(this, &MathAPI::concatenate));
    registerMethod("concatMany",    make_method(this, &MathAPI::concatenate2));
    registerMethod("getType",       make_method(this, &MathAPI::getType));

    registerProperty("message",
                     make_property(this, 
                        &MathAPI::get_Message,
                        &MathAPI::set_Message));
    registerProperty("readOnlyMessage", 
                     make_property(this,
                        &MathAPI::get_ReadOnlyMessage));
}

MathAPI::~MathAPI()
{
}

std::string MathAPI::returnString(const std::string& s)
{
    return s;
}

std::string MathAPI::intToString(int i)
{
    std::stringstream ss;
    ss << i;
    return ss.str();
}

int MathAPI::sumOf(int a, int b)
{
    return a+b;
}

std::string MathAPI::concatenate(const std::string& a, const std::string& b, const std::string& c)
{
    return a+b+c;
}

std::string MathAPI::concatenate2(const std::string& a, const FB::VariantList& list)
{
    FB::VariantList::const_iterator it(list.begin());
    std::string s(a);

    for( ; it != list.end(); ++it)
        s += it->convert_cast<std::string>();

    return s;
}

void MathAPI::set_Message(const std::string& s)
{
    m_message = s;
}

std::string MathAPI::get_Message()
{
    return m_message;
}

std::string MathAPI::get_ReadOnlyMessage()
{
    return m_readOnlyMessage;
}

std::string MathAPI::getType(const FB::variant a)
{
    return a.get_type().name();
}