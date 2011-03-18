/**********************************************************\ 
Original Author: Richard Bateman (taxilian)

Created:    Dec 9, 2009
License:    Dual license model; choose one of two:
            New BSD License
            http://www.opensource.org/licenses/bsd-license.php
            - or -
            GNU Lesser General Public License, version 2.1
            http://www.gnu.org/licenses/lgpl-2.1.html

Copyright 2010 Facebook, Inc and the Firebreath development team
\**********************************************************/

#pragma once
#ifndef H_FB_DOM_WINDOW
#define H_FB_DOM_WINDOW

#include <string>
#include <boost/shared_ptr.hpp>
#include "Node.h"
#include "Document.h"

namespace FB { 
    namespace DOM {

    class JSObject;
    /**
     * Window
     *
     * Provides a wrapper around a JSObject * that represents a DOM element
     **/
    class Window;

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// @typedef    boost::shared_ptr<Window> WindowPtr
    ///
    /// @brief  Defines an alias representing a pointer to a DOM::Window
    ////////////////////////////////////////////////////////////////////////////////////////////////////
    typedef boost::shared_ptr<Window> WindowPtr;

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// @class  Window
    ///
    /// @brief  DOM Window abstraction for manipulating and accessing the javascript window object that
    ///         the plugin is contained in
    /// @since 1.3
    /// Most of the time you will want to call BrowserHost::getDOMWindow() to get the reference to
    /// the containing window
    ////////////////////////////////////////////////////////////////////////////////////////////////////
    class Window : public virtual Node
    {
    public:

        ////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @fn Window::Window(const JSObjectPtr& element)
        ///
        /// @brief  Constructor. DO NOT USE
        ///
        /// Instead of using this constructor, use Window::create(FB::JSObjectPtr &api).  This way it will
        /// create the correct type.  In IE an AXDOM::Window object which extends this one is created
        /// to handle some special cases in ActiveX.
        ///
        /// @param  element The element. 
        ////////////////////////////////////////////////////////////////////////////////////////////////////
        Window(const JSObjectPtr& element);

        ////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @fn virtual Window::~Window()
        ///
        /// @brief  Finaliser. 
        ////////////////////////////////////////////////////////////////////////////////////////////////////
        virtual ~Window();

        ////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @fn WindowPtr Window::window()
        ///
        /// @brief  Gets the shared_ptr for the window (FB::DOM::WindowPtr) 
        ///
        /// @return shared_ptr<FB::DOM::Window> (alias as FB::DOM::WindowPtr)
        ////////////////////////////////////////////////////////////////////////////////////////////////////
        WindowPtr window() { return boost::dynamic_pointer_cast<Window>(node()); }

        ////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @fn static WindowPtr Window::create(const FB::JSObjectPtr &api)
        ///
        /// @brief  Creates a FB::DOM::Window object from a JSObjectPtr representing a DOM object.  This
        ///         will probably throw an exception and definitely not work right if the DOM object
        ///         represented is not a window
        ///
        /// @param  api The FB::JSObjectAPI object containing the window to wrap
        ///
        /// @return WindowPtr of the new Window
        ////////////////////////////////////////////////////////////////////////////////////////////////////
        static WindowPtr create(const FB::JSObjectPtr &api) { return api->getHost()->_createWindow(api); }

    public:

        ////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @fn virtual DocumentPtr Window::getDocument() const
        ///
        /// @brief  Gets the DOM Document contained in this window
        ///
        /// @return The document. 
        ////////////////////////////////////////////////////////////////////////////////////////////////////
        virtual DocumentPtr getDocument() const;

        ////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @fn virtual void Window::alert(const std::string& str) const
        ///
        /// @brief  Pop up a JS alert box
        ///
        /// @param  str The alert string
        ////////////////////////////////////////////////////////////////////////////////////////////////////
        virtual void alert(const std::string& str) const;
        ////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @overload virtual void Window::alert(const std::wstring& str) const
        ////////////////////////////////////////////////////////////////////////////////////////////////////
        virtual void alert(const std::wstring& str) const;

        ////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @fn virtual FB::JSObjectPtr Window::createArray() const
        ///
        /// @brief  Creates a javascript array object and returns it
        ///
        /// @return new empty javascript array
        ////////////////////////////////////////////////////////////////////////////////////////////////////
        virtual FB::JSObjectPtr createArray() const;

        ////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @fn virtual FB::JSObjectPtr Window::createMap() const
        ///
        /// @brief  Creates a javascript object and returns it
        ///
        /// @return new empty javascript object
        ////////////////////////////////////////////////////////////////////////////////////////////////////
        virtual FB::JSObjectPtr createMap() const;

        ////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @fn virtual FB::JSObjectPtr Window::createDate(const std::string& datestring) const
        ///
        /// @brief  Creates a javascript Date object and returns it
        ///
        /// @return new javascript Date object
        ////////////////////////////////////////////////////////////////////////////////////////////////////
//        virtual FB::JSObjectPtr createDate(const std::string& datestring) const;

        ////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @fn virtual std::string Window::getLocation() const
        ///
        /// @brief  Returns window.location for the containing page
        ///
        /// @return The location as a strin
        ///         g. 
        ////////////////////////////////////////////////////////////////////////////////////////////////////
        virtual std::string getLocation() const;
    };

} };

#endif // H_FB_DOM_WINDOW

