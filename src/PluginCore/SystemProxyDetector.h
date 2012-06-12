#ifndef SystemProxyDetector_h__
#define SystemProxyDetector_h__

#include <map>
#include <string>
#include <boost/noncopyable.hpp>

namespace FB {

    class SystemProxyDetector : boost::noncopyable {
    protected:
        SystemProxyDetector() {}

    public:
        virtual ~SystemProxyDetector() {}
        static SystemProxyDetector* get();

        virtual bool detectProxy(std::map<std::string, std::string>& proxyMap, const std::string& URL) = 0;
    };

};
#endif // SystemProxyDetector_h__
