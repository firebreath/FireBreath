/**********************************************************\ 
Original Author: Dan Weatherford

Imported into FireBreath:    Oct 4, 2010
License:    Dual license model; choose one of two:
            New BSD License
            http://www.opensource.org/licenses/bsd-license.php
            - or -
            GNU Lesser General Public License, version 2.1
            http://www.gnu.org/licenses/lgpl-2.1.html

Copyright 2010 Dan Weatherford and Facebook, Inc
\**********************************************************/

#pragma once
#ifndef H_HTTPSERVICE
#define H_HTTPSERVICE

#include <boost/shared_ptr.hpp>
#include <boost/weak_ptr.hpp>
#include <boost/enable_shared_from_this.hpp>
#include <boost/thread/recursive_mutex.hpp>
#include <string>
#include <vector>
#include <map>
#include "URI.h"
#include "HTTPService/HTTPHandler.h"

namespace HTTP {
    class HTTPService : public boost::enable_shared_from_this<HTTPService>
    {
    public:
        static boost::shared_ptr<HTTPService> create(const std::string ipaddr = "127.0.0.1", const int port = 0, const std::string hostname = "localhost");
        HTTPService() { };
        virtual ~HTTPService() { };

        // should only be called directly before module unload, since
        // it may leave instances in an unusable state
        static void terminateAllInstances();

        // Should old be called when the plugin using the service unloads
        virtual void terminate() = 0;

        virtual void registerHandler(boost::shared_ptr<HTTPHandler> hnd) = 0;
        virtual void deregisterHandler(boost::shared_ptr<HTTPHandler> hnd) = 0;

        // While this is enabled, the service will hold a reference to itself that it will release when
        // the builtin URI "/shutdown" is accessed.
        virtual void setDeferShutdown(bool val) = 0;

        virtual FB::URI getBaseUri() const = 0;
        // Add a signature to a URI (via a GET param). Only the path is signed, so you can still
        // change GET params however you like without causing the verification to fail.
        virtual void sign_uri(FB::URI& in_uri) const = 0;

    protected:
        virtual void init() = 0;

    protected:
        static const size_t threadpool_size = 2;
        static boost::recursive_mutex instance_set_lock;
        static std::list<boost::weak_ptr<HTTPService> > instances;
    };
};

#endif // H_HTTPSERVICE

