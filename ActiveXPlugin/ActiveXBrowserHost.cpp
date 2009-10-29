#include "ActiveXBrowserHost.h"

ActiveXBrowserHost::ActiveXBrowserHost(void)
{
}

ActiveXBrowserHost::~ActiveXBrowserHost(void)
{
}
void ActiveXBrowserHost::ScheduleAsyncCall(void (*func)(void *), void *userData)
{
}

void *ActiveXBrowserHost::getContextID()
{
    return (void*)this;
}
