
#include <memory>
#include <map>
#include <string>

using std::map;
using std::string;

#include "X11/SystemProxyDetectorX11.h"

FB::SystemProxyDetector* FB::SystemProxyDetector::get()
{
    static std::unique_ptr<FB::SystemProxyDetector> _inst(new FB::SystemProxyDetectorX11());
    return _inst.get();
}

bool FB::SystemProxyDetectorX11::detectProxy( map<string, string>& proxyMap, string URL )
{
    return false;
}

