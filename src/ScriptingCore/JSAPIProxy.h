/**********************************************************\ 
Original Author: Richard Bateman (taxilian)

Created:    Nov 23, 2010
License:    Dual license model; choose one of two:
            New BSD License
            http://www.opensource.org/licenses/bsd-license.php
            - or -
            GNU Lesser General Public License, version 2.1
            http://www.gnu.org/licenses/lgpl-2.1.html
            
Copyright 2009 Richard Bateman, Firebreath development team
\**********************************************************/

#pragma once
#ifndef H_FB_JSAPIProxy
#define H_FB_JSAPIProxy

#include "APITypes.h"
#include "JSAPIImpl.h"

namespace FB
{
    FB_FORWARD_PTR(JSAPIProxy);
    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// @class  JSAPIProxy
    ///
    /// @brief  JavaScript API Wrapper -- this can wrap any type of JSAPI object and be passed back
    ///         to any browser that 
    /// 
    /// There are cases when you may want to pass a JSAPI object into another browser / browserhost
    /// instance (even just such as another page).  This class makes it possible to return a JSAPI object
    /// from a different instance of the plugin into a page, or presumably even a JSObject from one
    /// page into another (be *really* careful with that).  Note that not all JSAPI objects are linked
    /// to a BrowserHost instance, and thus might not need this wrapper class to function correctly
    ///
    /// Both constructors also have an optional SecurityZone parameter that, if provided, can allow
    /// this proxy object to operate in a different security zone than the object it wraps, allowing
    /// the same object to be shared between two instances of a plugin that run in different
    /// security zones
    /// 
    /// @see FB::JSAPI
    /// @see FB::JSAPIAuto
    ////////////////////////////////////////////////////////////////////////////////////////////////////
    class JSAPIProxy : public JSAPIImpl
    {
    protected:
        JSAPIProxy(const FB::JSAPIPtr &inner);
        JSAPIProxy(const FB::JSAPIWeakPtr &inner);

        JSAPIProxy(const SecurityZone& securityLevel, const FB::JSAPIPtr &inner);
        JSAPIProxy(const SecurityZone& securityLevel, const FB::JSAPIWeakPtr &inner);

    public:
        virtual ~JSAPIProxy(void);

        ////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @fn JSAPIProxyPtr FB::JSAPIProxy::create(const FB::JSAPIPtr &inner)
        ///
        /// @brief  Creates a JSAPIProxy that holds a shared_ptr reference to the JSAPI object provided
        ///
        /// @param  inner       the API to reference; a reference to this API will be held and inner will
        ///                     not be released until this proxy object is released
        /// @return std::shared_ptr<JSAPIProxy>
        /// @since  1.4a3
        ////////////////////////////////////////////////////////////////////////////////////////////////////
        static JSAPIProxyPtr create(const FB::JSAPIPtr &inner);
        /// @overload
        static JSAPIProxyPtr create(const SecurityZone& securityLevel, const FB::JSAPIPtr &inner);

        ////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @fn JSAPIProxyPtr  FB::JSAPIProxy::create(const FB::JSAPIWeakPtr &inner)
        ///
        /// @brief  Creates a JSAPIProxy that holds a weak_ptr reference to the JSAPI object provided
        ///
        /// @param  inner       the API to reference; a reference to this API will be weak and when inner
        ///                     is released this object will throw an exception in response to any access
        ///                     except requests for the "expired" property, which is reserved in JSAPIProxy
        ///                     and will return true if inner has been released.
        /// @return std::shared_ptr<JSAPIProxy>
        /// @since  1.4a3
        ////////////////////////////////////////////////////////////////////////////////////////////////////
        static JSAPIProxyPtr create(const FB::JSAPIWeakPtr &inner);
        /// @overload
        static JSAPIProxyPtr create(const SecurityZone& securityLevel, const FB::JSAPIWeakPtr &inner);

        ////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @fn bool  FB::JSAPIProxy::isExpired() const
        ///
        /// @brief  Returns true if the weak_ptr this contains doesn't point to anything
        ///
        /// This can be used to determine if the API object proxied to is still valid or not
        ///
        /// @return bool
        /// @since 1.4a3
        ////////////////////////////////////////////////////////////////////////////////////////////////////
        inline bool isExpired() const {
            return m_apiWeak.expired();
        }

        ////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @fn public void FB::JSAPIProxy::changeObject(const FB::JSAPIPtr &inner)
        ///
        /// @brief  Changes the wrapped JSAPIPtr to the given one
        ///
        /// @param  inner   const FB::JSAPIPtr &    New JSAPIPtr
        /// @since 1.4a3
        ////////////////////////////////////////////////////////////////////////////////////////////////////
        void changeObject(const FB::JSAPIPtr &inner);
        ////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @fn public void FB::JSAPIProxy::changeObject(const FB::JSAPIWeakPtr &inner)
        ///
        /// @brief  Changes the wrapped JSAPIWeakPtr to the given one
        ///
        /// @param  inner   FB::JSAPIWeakPtr &      New JSAPIWeakPtr
        /// @since 1.4a3
        ////////////////////////////////////////////////////////////////////////////////////////////////////
        void changeObject(const FB::JSAPIWeakPtr &inner);
        void reset();

        inline FB::JSAPIPtr getAPI() const {
            FB::JSAPIPtr tmp = m_apiWeak.lock();
            if (tmp)
                return tmp;
            throw FB::object_invalidated();
        }

    protected:
        mutable FB::JSAPIPtr m_api;
        mutable FB::JSAPIWeakPtr m_apiWeak;

    public:

        void invalidate() override;
        virtual void getMemberNames(std::vector<std::string> &nameVector) const override;
        virtual size_t getMemberCount() const override;

        virtual bool HasMethod(const std::wstring& methodName) const override;
        virtual bool HasMethod(std::string methodName) const override;
        virtual bool HasProperty(const std::wstring& propertyName) const override;
        virtual bool HasProperty(std::string propertyName) const override;
        virtual bool HasProperty(int idx) const override;

        virtual variantPromise GetProperty(const std::wstring& propertyName) override;
        virtual variantPromise GetProperty(std::string propertyName) override;

        virtual void SetProperty(const std::wstring& propertyName, const variant& value) override;
        virtual void SetProperty(std::string propertyName, const variant& value) override;

        virtual void RemoveProperty(const std::wstring& propertyName) override;
        virtual void RemoveProperty(std::string propertyName) override;
        
        virtual variantPromise GetProperty(int idx) override;
        virtual void SetProperty(int idx, const variant& value) override;
        virtual void RemoveProperty(int idx) override;

        virtual variantPromise Invoke(const std::wstring& methodName, const std::vector<variant>& args) override;
        virtual variantPromise Invoke(std::string methodName, const std::vector<variant>& args) override;
    };

};
#endif

