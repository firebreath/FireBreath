/**********************************************************\
Original Author: Richard Bateman (taxilian)

Created:    Oct 19, 2009
License:    Dual license model; choose one of two:
            New BSD License
            http://www.opensource.org/licenses/bsd-license.php
            - or -
            GNU Lesser General Public License, version 2.1
            http://www.gnu.org/licenses/lgpl-2.1.html

Copyright 2009 PacketPass, Inc and the Firebreath development team
\**********************************************************/

#pragma once
#ifndef H_FB_PLUGINEVENTSINK
#define H_FB_PLUGINEVENTSINK

#include "PluginEventSource.h"
#include <boost/enable_shared_from_this.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/noncopyable.hpp>
#include <string>

namespace FB {

    class PluginEvent;
    
    class PluginEventSink;
    typedef boost::shared_ptr<PluginEventSink> PluginEventSinkPtr;
    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// @class  PluginEventSink
    ///
    /// @brief  Plugin event sink; all objects that can receive PluginEvent s should inherit from this
    ///         class.  Most notably, PluginCore extends this class
    ////////////////////////////////////////////////////////////////////////////////////////////////////
    class PluginEventSink : public boost::enable_shared_from_this<PluginEventSink>, boost::noncopyable
    {
    public:
        PluginEventSink() { };
        virtual ~PluginEventSink() { };

        ////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @fn PluginEventSinkPtr shared_ptr()
        ///
        /// @brief  Get a shared_ptr to the current class
        ///         
        /// To get a shared_ptr to a child class of this, such as PluginCore, you can use ptr_cast:
        /// @code
        ///      FB::ptr_cast<FB::PluginCore>(shared_ptr());
        /// @endcode
        ///
        /// @return shared_ptr for "this" pointer
        ////////////////////////////////////////////////////////////////////////////////////////////////////
        PluginEventSinkPtr shared_ptr() { return shared_from_this(); }

    public:

        ////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @fn virtual bool HandleEvent(PluginEvent *event, PluginEventSource *source) = 0
        ///
        /// @brief  Called by the browser when a system event needs to be handled, such as a mouse event,
        ///         a keyboard event, or a drawing event.
        ///
        /// The recommended way to implement HandleEvent is with the PLUGINEVENT_TYPE macros, like so:
        /// 
        /// @code
        ///     BEGIN_PLUGIN_EVENT_MAP()
        ///         EVENTTYPE_CASE(FB::MouseDownEvent, onMouseDown, FB::PluginWindow)
        ///         EVENTTYPE_CASE(FB::MouseUpEvent, onMouseUp, FB::PluginWindow)
        ///         EVENTTYPE_CASE(FB::MouseMoveEvent, onMouseMove, FB::PluginWindow)
        ///     END_PLUGIN_EVENT_MAP()
        /// @endcode
        /// - BEGIN_PLUGIN_EVENT_MAP() will open the HandleEvent function
        /// - END_PLUGIN_EVENT_MAP() will close the HandleEvent function
        /// 
        /// - EVENTTYPE_CASE(eventType, function, eventSourceType) dispatches events of type "eventType"
        ///   and calls "function" on the current object.  The EventSource is casted as "eventSourceType".
        /// 
        /// If you use these macros, you must also create the matching methods in your class.  For
        /// example, the methods needed for the map above are:
        /// 
        /// @code
        ///     virtual bool onMouseDown(FB::MouseDownEvent *evt, FB::PluginWindow*);
        ///     virtual bool onMouseUp(FB::MouseUpEvent *evt, FB::PluginWindow*);
        ///     virtual bool onMouseMove(FB::MouseMoveEvent *evt, FB::PluginWindow*);
        /// @endcode
        ///         
        /// @author Richard Bateman
        ///
        /// @param event PluginEvent to handle
        /// @param source The PluginEventSource that the event originated from
        ///
        /// @return true if the event was handled, false if not. 
        ////////////////////////////////////////////////////////////////////////////////////////////////////
        virtual bool HandleEvent(PluginEvent *event, PluginEventSource *source) = 0;
    };
};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// @def    BEGIN_PLUGIN_EVENT_MAP
/// PluginEventSource *win)
///
/// @brief  Begins a plugin event map.
///         
/// This will create an implementation of the HandleEvent function.  It must be matched with
/// a END_PLUGIN_EVENT_MAP() call
///
/// @remarks    Richard Bateman, 10/15/2010. 
/// @see EVENTTYPE_CASE
////////////////////////////////////////////////////////////////////////////////////////////////////
#define BEGIN_PLUGIN_EVENT_MAP() virtual bool HandleEvent(FB::PluginEvent *evt, FB::PluginEventSource *src) { \
                                          if (0) { }

////////////////////////////////////////////////////////////////////////////////////////////////////
/// @def    EVENTTYPE_CASE(eventType, methodName, winType)
///
/// @brief  Dispatches events to the given event handler, does typecasting as needed
///
/// @remarks    Richard Bateman, 10/15/2010. 
///
/// Note that methodName must be a valid method in the class that accepts an eventType* and 
/// a winType* as arguments
/// 
/// @param  eventType   Type of the event to dispatch
/// @param  methodName  Name of the event handler method
/// @param  srcType     Type of the PluginEventSource
/// 
/// @see PluginEventSink::HandleEvent
/// @see BEGIN_PLUGIN_EVENT_MAP
/// @see END_PLUGIN_EVENT_MAP
////////////////////////////////////////////////////////////////////////////////////////////////////
#define EVENTTYPE_CASE(eventType, methodName, srcType) else if (evt->validType<eventType>() && src->validType<srcType>()) { \
                                                return methodName(evt->get<eventType>(), src->get_as<srcType>()); }

#define PLUGIN_EVENT_MAP_CASCADE(super) else return super::HandleEvent(evt, src);

////////////////////////////////////////////////////////////////////////////////////////////////////
/// @def    END_PLUGIN_EVENT_MAP()
///
/// @brief  Ends a plugin event map started with BEGIN_PLUGIN_EVENT_MAP
///
/// @remarks    Richard Bateman, 10/15/2010. 
/// @see EVENTTYPE_CASE
////////////////////////////////////////////////////////////////////////////////////////////////////
#define END_PLUGIN_EVENT_MAP() return false; }

#endif

