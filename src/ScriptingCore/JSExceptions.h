/**********************************************************\ 
Original Author: Richard Bateman (taxilian)

Created:    Dec 18, 2010
License:    Dual license model; choose one of two:
            New BSD License
            http://www.opensource.org/licenses/bsd-license.php
            - or -
            GNU Lesser General Public License, version 2.1
            http://www.gnu.org/licenses/lgpl-2.1.html
            
Copyright 2009 Richard Bateman, Firebreath development team
\**********************************************************/

#pragma once
#ifndef JSExceptions_h__
#define JSExceptions_h__
#include <string>
#include <stdexcept>
#include <exception>

namespace FB {
    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// @exception script_error
    ///
    /// @brief  Exception type; when thrown in a JSAPI method, a javascript exception will be thrown. 
    ////////////////////////////////////////////////////////////////////////////////////////////////////
    struct script_error : std::exception
    {
        script_error(const std::string& error)
            : m_error(error)
        { }
        virtual ~script_error() throw() { }
        virtual const char* what() const throw() { 
            return m_error.c_str(); 
        }
        std::string m_error;
    };

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// @exception invalid_arguments
    ///
    /// @brief  Thrown by a JSAPI object when the argument(s) provided to a SetProperty or Invoke
    ///         call are found to be invalid.  JSAPIAuto will throw this automatically if the argument
    ///         cannot be convert_cast to the type expected by the function.
    ////////////////////////////////////////////////////////////////////////////////////////////////////
    struct invalid_arguments : script_error
    {
        invalid_arguments()
            : script_error("Invalid Arguments")
        { }
        ~invalid_arguments() throw() { }

        invalid_arguments(const std::string& error)
            : script_error(error)
        { }
    };

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// @exception object_invalidated
    ///
    /// @brief  Thrown by a JSAPI object when a call is made on it after the object has been
    ///         invalidated.
    ///         
    /// This is particularly useful when you want to invalidate the object
    /// when the plugin gets released, as the PluginCore-derived Plugin object will usually get
    /// released before the JSAPI object
    ////////////////////////////////////////////////////////////////////////////////////////////////////
    struct object_invalidated : script_error
    {
        object_invalidated()
            : script_error("This object is no longer valid")
        { }
        ~object_invalidated() throw() { }
    };

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// @exception invalid_member
    ///
    /// @brief  Thrown when an Invoke, SetProperty, or GetProperty call is made for a member that is
    ///         invalid (does not exist, not accessible, only supports Get or Set, etc) 
    ////////////////////////////////////////////////////////////////////////////////////////////////////
    struct invalid_member : script_error
    {
        invalid_member(const std::string& memberName)
            : script_error("The specified member does not exist: " + memberName)
        { }
        ~invalid_member() throw() { }
    };
}

#endif // JSExceptions_h__

