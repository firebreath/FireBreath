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
    @{PLUGIN.ident}API(FB::BrowserHostWrapper *host);
    virtual ~@{PLUGIN.ident}API();

    // Read/Write property ${PROPERTY.ident}
    std::string get_testString();
    void set_testString(std::string val);

    // Read-only property ${PROPERTY.ident}
    std::string get_version();

    // Method echo
    FB::variant echo(FB::variant msg);

private:
    FB::AutoPtr<FB::BrowserHostWrapper> m_host;

    std::string m_testString;
};

#endif // H_@{PLUGIN.ident}API
