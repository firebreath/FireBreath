/**********************************************************\
Original Author: Richard Bateman (taxilian)

Created:    Oct 22, 2009
License:    Dual license model; choose one of two:
            New BSD License
            http://www.opensource.org/licenses/bsd-license.php
            - or -
            GNU Lesser General Public License, version 2.1
            http://www.gnu.org/licenses/lgpl-2.1.html

Copyright 2009 Richard Bateman, Firebreath development team
\**********************************************************/

#pragma once
#ifndef H_FB_NPAPI_ASYNCFUNCTIONCALL
#define H_FB_NPAPI_ASYNCFUNCTIONCALL

#ifdef FB_WIN
#define WM_ASYNCTHREADINVOKE    WM_USER + 1
#endif

#include <boost/shared_ptr.hpp>

namespace FB {

    struct AsyncFunctionCall
    {
        AsyncFunctionCall(void (*f)(void *), void *ud) 
            : func(f), userData(ud) { }
        void (*func)(void *);
        void *userData;
    };
    typedef boost::shared_ptr<AsyncFunctionCall> AsyncFunctionCallPtr;

};

#endif


