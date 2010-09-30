/**********************************************************\

  Auto-generated @{PLUGIN_ident}API.h

\**********************************************************/

#include <string>
#include <sstream>
#include "JSAPIAuto.h"
#include "BrowserHost.h"

#ifndef H_@{PLUGIN_ident}API
#define H_@{PLUGIN_ident}API

class @{PLUGIN_ident}API : public FB::JSAPIAuto
{
public:
    @{PLUGIN_ident}API(FB::BrowserHostPtr host);
    virtual ~@{PLUGIN_ident}API();

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
    FB::BrowserHostPtr m_host;

    std::string m_testString;
};

#endif // H_@{PLUGIN_ident}API
