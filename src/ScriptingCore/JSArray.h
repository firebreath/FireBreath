/**********************************************************\ 
Original Author: Georg Fritzsche

Created:    December 13, 2009
License:    Dual license model; choose one of two:
            New BSD License
            http://www.opensource.org/licenses/bsd-license.php
            - or -
            GNU Lesser General Public License, version 2.1
            http://www.gnu.org/licenses/lgpl-2.1.html

Copyright 2009 Georg Fritzsche, Firebreath development team
\**********************************************************/

#pragma once
#ifndef H_JSARRAY
#define H_JSARRAY

#include "JSObject.h"
#include "JSAPIAuto.h"

namespace FB
{
    FB_FORWARD_PTR(JSArray);
    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// @class  JSArray
    ///
    /// @brief  Wraps a Javascript Array from a FB::JSObject.  Use this if your array is very large
    ///         or if you need to pass it by reference and change it in your function.
    /// @code
    ///      void handleArray(const FB::JSObjectPtr &in) {
    ///         FB::JSArray arr(in);
    ///         for (size_t i = 0; i < arr.GetLength(); i++) {
    ///             long i = arr[i].convert_cast<long>();
    ///         }
    ///      }
    /// @endcode
    ////////////////////////////////////////////////////////////////////////////////////////////////////
    class JSArray
    {
    public:
        JSArray();
        explicit JSArray(const FB::JSObjectPtr& obj);
        explicit JSArray(BrowserHostPtr host, const FB::VariantList& values);

        FB::variant Get(size_t index);
        void Set(const size_t index, const variant& value);
        size_t GetLength() const;
        
        FB::variant operator[](size_t index);

        FB::VariantList Values();

        void push(variant value);

    protected:
        FB::JSObjectPtr m_obj;
        void RegisterMethods();
    };
}

#endif

