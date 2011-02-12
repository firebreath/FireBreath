
/**********************************************************\ 
Original Author: Dan Weatherford

Imported into FireBreath:    Oct 5, 2010
License:    Dual license model; choose one of two:
            New BSD License
            http://www.opensource.org/licenses/bsd-license.php
            - or -
            GNU Lesser General Public License, version 2.1
            http://www.gnu.org/licenses/lgpl-2.1.html

Copyright 2010 Dan Weatherford and Facebook, Inc
\**********************************************************/

#include "HTTPService.h"
#include "HTTPService/BasicService.h"
#include "logging.h"

using namespace HTTP;

boost::recursive_mutex HTTPService::instance_set_lock;
std::list<boost::weak_ptr<HTTPService> > HTTPService::instances;

boost::shared_ptr<HTTPService> HTTPService::create(const std::string ipaddr, const int port, const std::string hostname) {
    boost::shared_ptr<BasicService> svc(new BasicService(ipaddr, port, hostname));
    svc->init();
    {
        boost::recursive_mutex::scoped_lock _l(instance_set_lock);
        instances.push_back(svc);
    }
    return svc;
}

void HTTPService::terminateAllInstances() {
    boost::recursive_mutex::scoped_lock _l(instance_set_lock);
    while (!instances.empty()) {
        boost::shared_ptr<HTTPService> srv = boost::dynamic_pointer_cast<BasicService>(instances.front().lock());
        instances.erase(instances.begin());
        if (srv) srv->terminate();
    }
    instances.clear();
}

