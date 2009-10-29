#ifndef H_ACTIVEXBROWSERHOST
#define H_ACTIVEXBROWSERHOST

#include "BrowserHostWrapper.h"

class ActiveXBrowserHost :
    public FB::BrowserHostWrapper
{
public:
    ActiveXBrowserHost(void);
    virtual ~ActiveXBrowserHost(void);
    virtual void ScheduleAsyncCall(void (*func)(void *), void *userData);

    virtual void *getContextID();
};

#endif // H_ACTIVEXBROWSERHOST