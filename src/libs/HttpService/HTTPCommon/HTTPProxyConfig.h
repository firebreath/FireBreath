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
#ifndef H_HTTPPROXYCONFIG
#define H_HTTPPROXYCONFIG

namespace HTTP {
        
    struct HTTPProxyConfig {
      enum Type {
        kNoProxy = 0,
        kHTTPProxy = 1,
        kSOCKS4Proxy = 2
      } type;
      std::string hostname;
      unsigned short port;
      
      HTTPProxyConfig() : type(kNoProxy) {}
    };
    
};

#endif

