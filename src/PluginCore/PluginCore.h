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
#ifndef H_FB_PLUGINCORE
#define H_FB_PLUGINCORE

#include "PluginEventSink.h"
#include "APITypes.h"
#include <string>
#include <set>
#include <boost/assign.hpp>
#include <boost/logic/tribool.hpp>

using boost::assign::list_of;

namespace FB {

    class PluginWindow;
    class PluginEvent;
    FB_FORWARD_PTR(PluginCore);
    FB_FORWARD_PTR(JSAPI);
    FB_FORWARD_PTR(BrowserHost);

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// @class  PluginCore
    ///
    /// @brief  Base class for all FireBreath Plugins 
    ///
    /// This class should be extended by all FireBreath plugin classes.  Each &lt;object&gt; tag in
    /// the web page corresponds to an instance of a class which extends PluginCore, and the
    /// basic plugin lifecycle is seen in this class.  Please note that JavaScript API calls
    /// are handled by JSAPI or JSAPIAuto.
    /// 
    /// All classes extending PluginCore must implement the createJSAPI() method, which creates an
    /// instance of the JSAPI-derived type which handles interaction with Javascript for this plugin.
    /// 
    /// @author Richard Bateman (taxilian)
    ////////////////////////////////////////////////////////////////////////////////////////////////////
    class PluginCore : public PluginEventSink
    {
    public:
        static int getActivePluginCount() { return ActivePluginCount; }
        static std::string& getOS() { return OS; }
        static std::string& getBrowser() { return Browser; }
    private:
        static volatile int ActivePluginCount;

        static std::string OS;
        static std::string Browser;

    public:
        ////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @fn static void PluginCore::setPlatform(const std::string& os, const std::string& browser)
        ///
        /// @brief  Called by the browser when the Operating System and Browser are known.
        ///
        /// @author Richard Bateman
        ///
        /// @param  os      The operating system. 
        /// @param  browser The browser. 
        ////////////////////////////////////////////////////////////////////////////////////////////////////
        static void setPlatform(const std::string& os, const std::string& browser);

    public:

        ////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @fn PluginCore::PluginCore()
        ///
        /// @brief  Default Constructor
        ///
        /// As of FireBreath 1.4 alpha 3, you no longer need to specify the param tags that you want
        /// to support, as these will now be automatically detected on all platforms.
        /// 
        /// You can access param values set in your HTML with &lt;param&gt; tags inside your
        /// object tag, like so:
        /// 
        /// @code
        ///     <object ....>
        ///       <param name="onload" value="onpluginloaded" />
        ///       <param name="starturl" value="http://wiki.firebreath.org" />
        ///       <param name="paramlist" value="width=100;height=100" />
        ///     </object>
        /// @endcode
        ///         
        /// Then from within your plugin class, you can access the value like so:
        /// 
        /// std::string paramlist = m_params["starturl"].convert_cast<std::string>();
        /// 
        /// Note that if "onload" is one of the param names added (which it is if you don't override this
        /// method), FireBreath will query the browser for a function matching the string passed in as
        /// this param and call that function when initialization is complete.  Similarly, all other params
        /// that start with "on" will be automatically resolved to matching function names if possible.
        /// 
        /// To get the function from an "onstart" param, you could use the following code:
        /// FB::JSObjectPtr func = m_params["onstart"].convert_cast<FB::JSObjectPtr>();
        /// 
        /// @see getSupportedParams
        ////////////////////////////////////////////////////////////////////////////////////////////////////
        PluginCore();

        ////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @fn virtual PluginCore::~PluginCore()
        ///
        /// @brief  Finaliser. 
        ////////////////////////////////////////////////////////////////////////////////////////////////////
        virtual ~PluginCore();

        ////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @fn virtual void PluginCore::SetHost(const FB::BrowserHostPtr& host)
        ///
        /// @brief  Called by the browser during startup to provide a Browser Host object. 
        ///
        /// @param  host    The boost::shared_ptr<BrowserHost> to the BrowserHost object for the current
        ///                 browser. 
        ////////////////////////////////////////////////////////////////////////////////////////////////////
        virtual void SetHost(const FB::BrowserHostPtr& host)
        {
            m_host = host;
        }

        ////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @fn virtual PluginWindow* PluginCore::GetWindow() const
        ///
        /// @brief  Gets the PluginWindow instance currently provided by the browser, or NULL if none
        ///
        /// @return null if it fails, else the window. 
        ////////////////////////////////////////////////////////////////////////////////////////////////////
        virtual PluginWindow* GetWindow() const
        {
            return m_Window;
        }

        ////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @fn virtual public void FB::PluginCore::setScriptingOnly(const bool so)
        ///
        /// @brief  Called by the browser to indicate that there is no DOM element associated
        ///         with this object
        ///
        /// @returns void
        /// @since 1.6
        ////////////////////////////////////////////////////////////////////////////////////////////////////
        virtual void setScriptingOnly(const bool so = true)
        {
            m_scriptingOnly = so;
        }

        ////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @fn virtual public bool FB::PluginCore::isScriptingOnly() const
        ///
        /// @brief  Returns true if there is no DOM element associated with the plugin
        ///
        /// @returns bool
        /// @since 1.6
        ////////////////////////////////////////////////////////////////////////////////////////////////////
        virtual bool isScriptingOnly() const { return m_scriptingOnly; }

        ////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @fn virtual void PluginCore::SetWindow(PluginWindow *)
        ///
        /// @brief  Called by the browser to set the window.  The default implementation fires the 
        ///         WindowAttached event in response to this.
        ///
        /// @param  win The new window
        ////////////////////////////////////////////////////////////////////////////////////////////////////
        virtual void SetWindow(PluginWindow *win);

        ////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @fn virtual void PluginCore::ClearWindow()
        ///
        /// @brief  Called by the browser to indicate that the window is no longer valid.  The default
        ///         implementation fires the WindowDetached event in response to this.
        ////////////////////////////////////////////////////////////////////////////////////////////////////
        virtual void ClearWindow();

        ////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @fn virtual void PluginCore::setReady()
        ///
        /// @brief  Called by the browser to indicate that the basic initialization is complete and the
        ///         plugin is now ready to interact with the Browser via Javascript.  This may or may not
        ///         occur before the Window (if any) is set.
        ////////////////////////////////////////////////////////////////////////////////////////////////////
        virtual bool setReady();

        ////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @fn virtual void PluginCore::onPluginReady()
        ///
        /// @brief  This is called when the plugin is ready.
        ///         
        /// This may or may not occur before the AttachedEvent is fired to indicate that the plugin
        /// is available.  Params from the browser will be set and the root JSAPI created before this
        /// is called.
        /// 
        /// @since 1.3 RC2 
        ////////////////////////////////////////////////////////////////////////////////////////////////////
        virtual void onPluginReady() {};
        ////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @fn virtual void PluginCore::shutdown()
        ///
        /// @brief  This is called when the plugin needs to shut down to give it a chance to stop all
        ///         threads; the destructor would be too late for thread shutdown
        ///         
        /// This should occur after the detachedEvent is fired. When this function returns there should be
        /// no shared_ptr references to the plugin object other than ones held by FireBreath core
        ///
        /// @since 1.5
        ////////////////////////////////////////////////////////////////////////////////////////////////////
        virtual void shutdown() {};

    protected:

        ////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @fn virtual JSAPIPtr PluginCore::createJSAPI() = 0
        ///
        /// @brief  Called the first time something calls getRootJSAPI() to get the Root JSAPI instance. 
        ///
        /// This must be implemented by each class extending PluginCore.  The JSAPI object returned from
        /// this function in a FB::JSAPIPtr (boost::shared_ptr&lt;JSAPI&gt;) provides the primary interface
        /// with javascript for calls made on the plugin &lt;object&gt; tag.
        ///
        /// @return FB::JSAPIPtr to the default JSAPI implementation for this plugin.
        ////////////////////////////////////////////////////////////////////////////////////////////////////
        virtual JSAPIPtr createJSAPI() = 0;

    public:
        ////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @fn virtual JSAPIPtr PluginCore::releaseRootJSAPI()
        ///
        /// @brief  Releases the root JSAPI object.
        ///
        /// This should only be called from your destructor; if called along with m_host->freeRetainedObjects
        /// it can be used to cause the root JSAPI object to be freed during the destructor of your
        /// plugin object instead of during the destructor of PluginCore
        ///
        /// @return The root jsapi. 
        /// @since 1.4
        /// @see getRootJSAPI
        ////////////////////////////////////////////////////////////////////////////////////////////////////
        void releaseRootJSAPI() { m_api.reset(); }
        ////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @fn virtual JSAPIPtr PluginCore::getRootJSAPI()
        ///
        /// @brief  Gets the root JSAPI object.  It is not recommended to call this from the constructor
        ///         or before setHost is called, as many JSAPI objects need the BrowserHost and a weak_ptr
        ///         to the Plugin class to function correctly.
        ///
        /// @return The root jsapi. 
        ////////////////////////////////////////////////////////////////////////////////////////////////////
        virtual JSAPIPtr getRootJSAPI()
        {
            if (!m_api) {
                m_api = createJSAPI();
            }

            return m_api;
        }

        ////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @fn virtual bool PluginCore::isWindowless() = 0
        ///
        /// @brief  Called by the browser to query if this plugin should be windowless. 
        ///
        /// The default implementation supports a "windowless" param and checks m_params["windowless"]
        /// to determine if the plugin should be windowless or not.  To force always windowless, override
        /// and return true. To force never windowless, override and return false.
        /// 
        /// @return true if windowless, false if not. 
        ////////////////////////////////////////////////////////////////////////////////////////////////////
        virtual bool isWindowless();

        ////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @fn virtual void PluginCore::setFSPath(const std::string& path)
        ///
        /// @brief  Called by the browser to set the file system path of the plugin module. 
        ///
        /// This is currently only supported on Windows.  The path and filename of the currently loaded
        /// plugin is stored in m_filesystemPath;
        /// 
        /// @param  path    Full pathname of the file. 
        ////////////////////////////////////////////////////////////////////////////////////////////////////
        virtual void setFSPath(const std::string& path) { m_filesystemPath = path; }

        ////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @fn virtual std::string& getFSPath()
        ///
        /// @brief  Returns the path and filename of the current plugin module
        ///
        /// On Windows, this returns the path to the plugin DLL file.  On Mac, it's a .dylib. On linux a .so
        /// 
        /// @return  Full pathname of the file. 
        /// @since 1.4
        ////////////////////////////////////////////////////////////////////////////////////////////////////
        virtual std::string& getFSPath() { return m_filesystemPath; }

        ////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @fn virtual void PluginCore::setParams(const FB::VariantMap& inParams)
        ///
        /// @brief  Called by the browser to store the values provided in param tags in m_params
        ///         
        /// This function is called once the &lt;param&gt; tag values from the HTML are available.  After this
        /// function is called they can be accessed through m_params.
        /// 
        /// See initDefaultParams() for more information
        ///
        /// @param  inParams    Options for controlling the in. 
        ////////////////////////////////////////////////////////////////////////////////////////////////////

        virtual void setParams(const FB::VariantMap& inParams);
        
        virtual boost::optional<std::string> getParam(const std::string& key);

    protected:
        /// The BrowserHost object for the current session
        BrowserHostPtr m_host;          
        /// Stores the value passed into setFSPath()
        std::string m_filesystemPath;
        /// Boolean value indicates if the browser has called setParams() yet or not
        bool m_paramsSet;               
        /// Stores the list of params provided by the browser to setParams()
        FB::VariantMap m_params;        
    private:
        /// Don't use directly; use GetWindow() 
        PluginWindow *m_Window;         
        /// Don't use directly; use getRootJSAPI()
        JSAPIPtr m_api;
        boost::tribool m_windowLessParam;
        bool m_scriptingOnly;
    };
};

#endif

