/**********************************************************\

  Auto-generated @{PLUGIN.ident}API.h

\**********************************************************/

#include <string>
#include <sstream>
#include "JSAPIAuto.h"
#include "BrowserHostWrapper.h"

#ifndef H_@{PLUGIN.ident}API
#define H_@{PLUGIN.ident}API

class @{PLUGIN.ident}API : public FB::JSAPIAuto
{
public:
    @{PLUGIN.ident}API(FB::BrowserHost host);
    virtual ~@{PLUGIN.ident}API();

    // Read/Write property ${PROPERTY.ident}
    std::string get_testString();
    void set_testString(const std::string& val);

    // Read-only property ${PROPERTY.ident}
    std::string get_version();

    // Method echo
    FB::variant echo(const FB::variant& msg);
    
    // Method test-event
    void testEvent(const FB::variant& s);

private:
    FB::BrowserHost m_host;

    std::string m_testString;
};

#endif // H_@{PLUGIN.ident}API
