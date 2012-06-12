#pragma once
#ifndef typesafe_event_h__
#define typesafe_event_h__

/**********************************************************\ 
Original Author: Richard Bateman (taxilian)

Created:    Apr 8, 2011
License:    Dual license model; choose one of two:
            New BSD License
            http://www.opensource.org/licenses/bsd-license.php
            - or -
            GNU Lesser General Public License, version 2.1
            http://www.gnu.org/licenses/lgpl-2.1.html

Copyright 2009 Richard Bateman, Firebreath development team
\**********************************************************/

#include <boost/preprocessor/repetition.hpp>
#include <boost/preprocessor/cat.hpp>

#define __FB_EVTFUNC_NAME(evt) BOOST_PP_CAT(fire_, evt)
#define __FB_EVTPARAMMACRO(z, n, args) BOOST_PP_ARRAY_ELEM(n, args) p##n

#define __FB_EVTADDTOLIST(z, n, name) name.push_back(p##n);

#define FB_JSAPI_EVENT(evt, argCount, argList)                                       \
    void __FB_EVTFUNC_NAME(evt)(                                                     \
            BOOST_PP_ENUM(argCount, __FB_EVTPARAMMACRO, (argCount, argList))         \
        ) {                                                                          \
        FB::VariantList list;                                                        \
        BOOST_PP_REPEAT(argCount, __FB_EVTADDTOLIST, list)                           \
        FireEvent(BOOST_PP_STRINGIZE(BOOST_PP_CAT(on, evt)), list);                  \
    }

#endif // typesafe_event_h__
