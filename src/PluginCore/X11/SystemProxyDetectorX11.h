#ifndef SystemProxyDetectorX11_h__
#define SystemProxyDetectorX11_h__

#include "SystemProxyDetector.h"

namespace FB {

    class SystemProxyDetectorX11 : public SystemProxyDetector {
    private:
        SystemProxyDetectorX11() {}

    public:
        ~SystemProxyDetectorX11() {}
        SystemProxyDetector* get();

        virtual bool detectProxy(std::map<std::string, std::string>& proxyMap, const std::string& URL);
        friend class SystemProxyDetector;
    };

};

#endif // SystemProxyDetectorX11_h__

