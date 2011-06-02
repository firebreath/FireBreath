#ifndef SystemProxyDetectorMac_h__
#define SystemProxyDetectorMac_h__

#include "SystemProxyDetector.h"

namespace FB {
    
    class SystemProxyDetectorMac : public SystemProxyDetector {
    private:
        SystemProxyDetectorMac() {}
        
    public:
        ~SystemProxyDetectorMac() {}
        SystemProxyDetector* get();
        
        virtual bool detectProxy(std::map<std::string, std::string>& proxyMap, const std::string& URL);
        friend class SystemProxyDetector;
    };
    
};

#endif // SystemProxyDetectorMac_h__