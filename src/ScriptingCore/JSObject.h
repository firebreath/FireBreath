/**********************************************************\ 
Original Author: Richard Bateman (taxilian)

Created:    Oct 17, 2009
License:    Dual license model; choose one of two:
            New BSD License
            http://www.opensource.org/licenses/bsd-license.php
            - or -
            GNU Lesser General Public License, version 2.1
            http://www.gnu.org/licenses/lgpl-2.1.html

Copyright 2009 Richard Bateman, Firebreath development team
\**********************************************************/

#pragma once
#ifndef H_FB_JSObject
#define H_FB_JSObject

#include "JSAPI.h"
#include "BrowserHost.h"
#include <iterator>

namespace FB
{
    FB_FORWARD_PTR(JSObject);
    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// @class  JSObject
    ///
    /// @brief  Wraps a Javascript Object
    ///         
    /// Whenever you access a javascript object on a page, it will be through this object.  Because
    /// JSObject extends JSAPI, the API will be the same as a JSAPI object other than a few additional
    /// methods provided on this object for convenience.
    /// 
    /// Implementations of JSObject are expected to be threadsafe as of 1.3.0.  What that usually
    /// means is that if you call a method from another thread that it can't be safely run on it will
    /// use BrowserHost::CallOnMainThread to make the call on the main thread and wait for it to
    /// complete.  There may be performance issues with this that should be taken into consideration.
    ///
    /// @see JSAPI
    /// @see NPObjectAPI
    /// @see IDispatchAPI
    ////////////////////////////////////////////////////////////////////////////////////////////////////
    class JSObject : public FB::JSAPI
    {
    public:

        ////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @fn JSObject(const BrowserHostPtr& h)
        ///
        /// @brief  Constructor
        ///
        /// @param  h   The BrowserHost from whence the object came. 
        ////////////////////////////////////////////////////////////////////////////////////////////////////
        JSObject(const BrowserHostPtr& h) : m_host(h)
        {
        }

        ////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @fn virtual ~JSObject()
        ///
        /// @brief  Finaliser. 
        ////////////////////////////////////////////////////////////////////////////////////////////////////
        virtual ~JSObject() {}

        virtual void *getEventId() const { return NULL; }
        virtual void *getEventContext() const { return NULL; }

        virtual bool supportsOptimizedCalls() const { return false; }
        virtual void callMultipleFunctions(const std::string& name, const FB::VariantList& args,
                                           const std::vector<JSObjectPtr>& direct,
                                           const std::vector<JSObjectPtr>& ifaces) {};
        virtual bool isValid() = 0;

        JSObjectPtr shared_from_this() { return boost::static_pointer_cast<JSObject>(JSAPI::shared_from_this()); }
        boost::shared_ptr<const JSObject> shared_from_this() const { return boost::static_pointer_cast<const JSObject>(JSAPI::shared_from_this()); }

        ////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @fn virtual void InvokeAsync(const std::string& methodName, const std::vector<variant>& args)
        ///
        /// @brief  Just like Invoke, but works asynchronously.  Useful for javascript callbacks and events.
        ///         Can be safely called from any thread
        ///
        /// @param  methodName  Name of the method. 
        /// @param  args        The arguments. 
        /// @see Invoke
        ////////////////////////////////////////////////////////////////////////////////////////////////////
        virtual void InvokeAsync(const std::string& methodName, const std::vector<variant>& args)
        {
            if (m_host.expired()) {
                throw std::runtime_error("Cannot invoke asynchronously");
            }
            
            getHost()->ScheduleOnMainThread(shared_from_this(), boost::bind(&JSObject::_invokeAsync, this, args, methodName));
        }

    private:
        virtual void _invokeAsync(const std::vector<variant>& args, const std::string& methodName)
        {
            getHost()->delayedInvoke(0, shared_from_this(), args, methodName);
        }
    public:
        
        ////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @fn virtual void SetPropertyAsync(const std::string& propertyName, const variant& value)
        ///
        /// @brief  Just like SetProperty, but works asynchronously.  Useful if you are running on another
        ///         thread and don't need to wait to be sure it worked.
        ///
        /// @param  propertyName    Name of the property. 
        /// @param  value           The value. 
        ////////////////////////////////////////////////////////////////////////////////////////////////////
        virtual void SetPropertyAsync(const std::string& propertyName, const variant& value)
        {
            if (m_host.expired()) {
                throw std::runtime_error("Cannot invoke asynchronously");
            }
            getHost()->ScheduleOnMainThread(shared_from_this(), boost::bind((FB::SetPropertyType)&JSAPI::SetProperty, this, propertyName, value));
        }
    public:
        ////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @fn template<class Cont> static void GetArrayValues(const FB::JSObjectPtr& src, Cont& dst)
        ///
        /// @brief  Gets Array values out of src and adds them to the STL container dst 
        ///
        /// @param  src         Source for the. 
        /// @param [in,out] dst Destination for the. 
        ///
        /// ### todo    Find a better place for this conversion method. Gotcha to watch out for: has to
        /// be included after variant.h and everywhere where variant::convert_cast<SomeContainer>()
        /// is used. 
        ////////////////////////////////////////////////////////////////////////////////////////////////////
        template<class Cont>
        static void GetArrayValues(const FB::JSObjectPtr& src, Cont& dst);

        ////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @fn template<class Dict> static void GetObjectValues(const FB::JSObjectPtr& src, Dict& dst)
        ///
        /// @brief  Gets object values out of the javscript object src and adds them to the STL Dict
        ///         container dst
        ///
        /// @param  src         Source for the. 
        /// @param [in,out] dst Destination for the. 
        ////////////////////////////////////////////////////////////////////////////////////////////////////
        template<class Dict>
        static void GetObjectValues(const FB::JSObjectPtr& src, Dict& dst);
    
    public:
        /// @brief Get associated FB::JSAPI.
        virtual JSAPIPtr getJSAPI() const = 0;
        /// @brief Get the associated FB::BrowserHost; may throw std::bad_cast
        BrowserHostPtr getHost() { return BrowserHostPtr(m_host); }

    public:
        BrowserHostWeakPtr m_host;
    };

    template<class Cont>
    void JSObject::GetArrayValues(const FB::JSObjectPtr& src, Cont& dst)
    {
        if (!src) {
            return;
        }
        try {
            FB::variant tmp = src->GetProperty("length");
            long length = tmp.convert_cast<long>();
            std::back_insert_iterator<Cont> inserter(dst);

            for(int i=0; i<length; ++i) {
                tmp = src->GetProperty(i);
                *inserter++ = tmp.convert_cast<typename Cont::value_type>();
            }
        } catch(const FB::script_error& e) {
            throw e;
        }
    }

    template<class Dict>
    void JSObject::GetObjectValues(const FB::JSObjectPtr& src, Dict& dst)
    {
        typedef typename Dict::key_type KeyType;
        typedef typename Dict::mapped_type MappedType;
        typedef std::pair<KeyType, MappedType> PairType;
        typedef std::vector<std::string> StringVec;

        if (!src) return;
        try {
            StringVec fields;
            src->getMemberNames(fields);
            std::insert_iterator<Dict> inserter(dst, dst.begin());

            for(StringVec::iterator it = fields.begin(); it != fields.end(); it++) {
                FB::variant tmp = src->GetProperty(*it);
                *inserter++ = PairType(*it, tmp.convert_cast<MappedType>());
            }
        } catch (const FB::script_error& e) {
            throw e;
        }
    }
    
    namespace variant_detail { namespace conversion {
        // Convert in
        template <class T>
        typename boost::enable_if<
            boost::mpl::and_<
            boost::is_base_of<FB::JSAPI, T>,
            boost::mpl::not_<boost::is_base_of<FB::JSObject, T> > >
            ,variant>::type
        make_variant(const boost::shared_ptr<T>& ptr) {
            if (ptr)
                return variant(FB::JSAPIPtr(ptr), true);
            else
                return variant(FB::FBNull());
        }
        template <class T>
        typename boost::enable_if<boost::is_base_of<FB::JSObject, T>,variant>::type
        make_variant(const boost::shared_ptr<T>& ptr) {
            if (ptr)
                return variant(FB::JSObjectPtr(ptr), true);
            else
                return variant(FB::FBNull());
        }

        // Convert out
        template<class T>
        typename boost::enable_if<boost::is_base_of<FB::JSAPI, T>, boost::shared_ptr<T> >::type
            convert_variant(const variant& var, type_spec< boost::shared_ptr<T> >)
        {
            FB::JSAPIPtr ptr;
            // First of all, to succeed it *must* be a JSAPI object!
            if (var.get_type() == typeid(FB::JSObjectPtr)) {
                ptr = var.cast<FB::JSObjectPtr>();
            } else if (var.get_type() == typeid(FB::JSAPIWeakPtr)) {
                ptr = var.cast<FB::JSAPIWeakPtr>().lock();
            } else if (var.empty() || var.is_null()) {
                return boost::shared_ptr<T>();
            } else {
                ptr = var.cast<FB::JSAPIPtr>();
            }
            if (!ptr)
                return boost::shared_ptr<T>();

            FB::JSObjectPtr jso = FB::ptr_cast<FB::JSObject>(ptr);
            if (jso) {
                FB::JSAPIPtr inner = jso->getJSAPI();
                if (inner) {
                    boost::shared_ptr<T> tmp = FB::ptr_cast<T>(inner);
                    if (tmp) {
                        // Whew! We pulled the JSAPI object out of a JSObject and found what we were
                        // looking for; we always return the inner-most object.  Keep that in mind!
                        return tmp;
                    }
                    // If there is an inner object, but it isn't the one we want, fall through
                }
            }
            boost::shared_ptr<T> ret = FB::ptr_cast<T>(ptr);
            if (ret)
                return ret;
            else
                throw FB::bad_variant_cast(var.get_type(), typeid(T));
        }
        template<class T>
        typename boost::enable_if<boost::is_base_of<FB::JSAPI, T>, boost::weak_ptr<T> >::type
            convert_variant(const variant& var, type_spec< boost::weak_ptr<T> >)
        {
            boost::shared_ptr<T> sptr(var.convert_cast<boost::shared_ptr<T> >());
            return boost::weak_ptr<T>(sptr);
        }

        template<class Cont>
        typename FB::meta::enable_for_non_assoc_containers<Cont, const Cont>::type
            convert_variant(const variant& var, type_spec<Cont>)
        {
            typedef FB::JSObjectPtr JsObject;
            
            // if the held data is of type Cont just return it

            if(var.is_of_type<Cont>()) 
                return var.cast<Cont>();

            // if the help data is not a JavaScript object throw

            if(!var.can_be_type<JsObject>())
                throw bad_variant_cast(var.get_type(), typeid(JsObject));
            
            // if it is a JavaScript object try to treat it as an array

            Cont cont;
            FB::JSObject::GetArrayValues(var.convert_cast<JsObject>(), cont);
            return cont;
        }

        template<class Dict>
        typename FB::meta::enable_for_pair_assoc_containers<Dict, const Dict>::type
            convert_variant(const variant& var, type_spec<Dict>)
        {
            typedef FB::JSObjectPtr JsObject;
            
            // if the held data is of type Dict just return it

            if(var.is_of_type<Dict>()) 
                return var.cast<Dict>();

            // if the help data is not a JavaScript object throw

            if(!var.can_be_type<JsObject>())
                throw bad_variant_cast(var.get_type(), typeid(JsObject));
            
            // if it is a JavaScript object try to treat it as an array

            Dict dict;
            FB::JSObject::GetObjectValues(var.convert_cast<JsObject>(), dict);
            return dict;
        }
    } }
    
    // TODO: this doesn't belong here

    /// @brief Get a JSAPI-derived interface from a JSObject.
    /// @since 1.4
    ///
    /// @code 
    /// if (boost::shared_ptr<MyCustomAPI> ptr = FB::get_jsapi<MyCustomAPI>(jsobject)) {
    ///     doStuff(ptr);
    /// } else {
    ///     // failed ...
    /// }
    /// @endcode
    ///
    /// @return Returns a boost::shared_ptr<API> which is empty on failure
    ///         and non-empty on success.
    template<class API>
    boost::shared_ptr<API> get_jsapi(const FB::JSObjectPtr& jso)
    {
        typedef boost::shared_ptr<API> APIPtr;
        
        if (!jso) {
            return APIPtr();
        }
        
        if (FB::JSAPIPtr jsapi = jso->getJSAPI()) {
            return FB::ptr_cast<API>(jsapi);
        }
        
        return APIPtr();
    }
};

#endif

