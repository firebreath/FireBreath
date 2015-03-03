/**********************************************************\
Original Author: Gil Gonen

Created:    Jan 3, 2013
License:    Dual license model; choose one of two:
            New BSD License
            http://www.opensource.org/licenses/bsd-license.php
            - or -
            GNU Lesser General Public License, version 2.1
            http://www.gnu.org/licenses/lgpl-2.1.html

Copyright 2013 Gil Gonen and the Firebreath development team
\**********************************************************/

#pragma once
#ifndef H_FB_ASYNCDRAWSERVICE
#define H_FB_ASYNCDRAWSERVICE

#include "FBPointers.h"
#include <cstdint>
#include <memory>

namespace FB {
    FB_FORWARD_PTR(AsyncDrawService);

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// @class  AsyncDrawService
    ///
    /// @brief  asynchronous drawing service base class. 
    ///
    /// This is the base class for all asynchronous drawing services that are used in the plugin.  
    ////////////////////////////////////////////////////////////////////////////////////////////////////
    class AsyncDrawService : 
        public std::enable_shared_from_this<AsyncDrawService>
    {
    public:
        virtual ~AsyncDrawService() {};
        virtual void resized(uint32_t width, uint32_t height) = 0;
    };
};

#endif
