#ifndef H_ACTIVEXBROWSERHOST
#define H_ACTIVEXBROWSERHOST

#include "BrowserHostWrapper"

class ActiveXBrowserHost :
    public FB::BrowserHostWrapper
{
public:
    ActiveXBrowserHost(void);
    virtual ~ActiveXBrowserHost(void);
};

#endif // H_ACTIVEXBROWSERHOST