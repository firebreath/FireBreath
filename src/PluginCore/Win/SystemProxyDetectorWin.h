#ifndef SystemProxyDetectorWin_h__
#define SystemProxyDetectorWin_h__

#include "SystemProxyDetector.h"

namespace FB {

    class SystemProxyDetectorWin : public SystemProxyDetector {
    private:
        SystemProxyDetectorWin() {}

    public:
        ~SystemProxyDetectorWin() {}
        SystemProxyDetector* get();

        virtual bool detectProxy(std::map<std::string, std::string>& proxyMap, const std::string& URL);
        friend class SystemProxyDetector;
    };

};

#endif // SystemProxyDetectorWin_h__