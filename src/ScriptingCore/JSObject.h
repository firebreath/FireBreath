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
#include "variant_list.h"
#include "variant_map.h"
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
        virtual void callMultipleFunctions(std::string name, const FB::VariantList& args,
                                           const std::vector<JSObjectPtr>& direct,
                                           const std::vector<JSObjectPtr>& ifaces) {};
        virtual bool isValid() = 0;

    private:
        template<class Cont>
        FB::Promise<Cont> getArrayValuesImpl() {
            try {
                Promise<VariantList> dfdList = getHost()->GetArrayValues(shared_from_this());

                auto onDone = [](VariantList inList) -> Cont {
                    Cont out;
                    for (auto c : inList) {
                        out.emplace_back(c.convert_cast<typename Cont::value_type>());
                    }
                    return out;
                };
                return dfdList.then<Cont>(onDone);
            } catch (std::bad_cast&) {
                throw std::runtime_error("Browser not available, can't convert to array");
            }
        }

        template<class Dict>
        FB::Promise<Dict> getObjectValuesImpl() {
            try {
                Promise<VariantMap> dfdMap = getHost()->GetObjectValues(shared_from_this());

                auto onDone = [](VariantMap inMap) -> Dict {
                    Dict out;
                    for (auto c : inMap) {
                        out[c.first] = c.second.convert_cast<typename Dict::mapped_type>();
                    }
                    return out;
                };
                return dfdMap.then<Dict>(onDone);
            } catch (std::bad_cast&) {
                throw std::runtime_error("Browser not available, can't convert to object");
            }
        }

    protected:
        JSObjectPtr shared_from_this() {
            auto ptr = JSAPI::shared_from_this();
            return std::dynamic_pointer_cast<JSObject>(ptr);
        }
        std::shared_ptr<const JSObject> shared_from_this() const {
            auto ptr = JSAPI::shared_from_this();
            return std::dynamic_pointer_cast<const JSObject>(ptr);
        }

    public:
        ////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @fn virtual void InvokeAsync(std::string methodName, const std::vector<variant>& args)
        ///
        /// @brief  Just like Invoke, but does not care about a return value and has slightly lower overhead.
        ///         Useful for javascript callbacks and events.
        ///         Can be safely called from any thread
        ///
        /// @param  methodName  Name of the method. 
        /// @param  args        The arguments. 
        /// @see Invoke
        ////////////////////////////////////////////////////////////////////////////////////////////////////
        virtual void InvokeAsync(std::string methodName, const std::vector<variant>& args)
        {
            BrowserHostPtr host(m_host.lock());
            if (!host) {
                throw std::runtime_error("Cannot invoke asynchronously");
            }
            host->ScheduleOnMainThread(shared_from_this(), std::bind(&JSObject::_invokeAsync, this, args, methodName));
        }

    private:
        virtual void _invokeAsync(const std::vector<variant>& args, std::string methodName)
        {
            BrowserHostPtr host(m_host.lock());
            if (host) {
                host->delayedInvoke(0, shared_from_this(), args, methodName);
            }
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
        static Promise<Cont> GetArrayValues(const FB::JSObjectPtr& src);

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
        static Promise<Dict> GetObjectValues(const FB::JSObjectPtr& src);
    
    public:
        /// @brief Get the associated FB::BrowserHost; may throw std::bad_cast
        BrowserHostPtr getHost() { return BrowserHostPtr(m_host); }

    public:
        BrowserHostWeakPtr m_host;
    };

    template<class Cont>
    FB::Promise<Cont> JSObject::GetArrayValues(const FB::JSObjectPtr& src)
    {
        if (!src) {
            return Cont();
        }
        return src->getArrayValuesImpl<Cont>();
    }

    template<class Dict>
    FB::Promise<Dict> JSObject::GetObjectValues(const FB::JSObjectPtr& src)
    {
        return src->getObjectValuesImpl<Dict>();
    }
    
    namespace variant_detail { namespace conversion {
        // Convert in
        template <class T>
        typename boost::enable_if<
            boost::mpl::and_<
            boost::is_base_of<FB::JSAPI, T>,
            boost::mpl::not_<boost::is_base_of<FB::JSObject, T> > >
            ,variant>::type
        make_variant(const std::shared_ptr<T>& ptr) {
            if (ptr)
                return variant(FB::JSAPIPtr(ptr), true);
            else
                return variant(FB::FBNull());
        }
        template <class T>
        typename boost::enable_if<boost::is_base_of<FB::JSObject, T>,variant>::type
        make_variant(const std::shared_ptr<T>& ptr) {
            if (ptr)
                return variant(FB::JSObjectPtr(ptr), true);
            else
                return variant(FB::FBNull());
        }

            template<class Cont>
            typename boost::enable_if<
                boost::mpl::and_<
                    FB::meta::is_non_assoc_container<Cont>,
                    boost::mpl::not_<
                        boost::mpl::or_<
                               boost::mpl::or_<
                                   boost::is_same<std::vector<variant>, Cont>,
                                   boost::is_same<std::map<std::string, variant>, Cont>
                                >,
                            boost::mpl::or_<
                                   boost::is_same<std::wstring, Cont>,
                                   boost::is_same<std::string, Cont>
                                >
                            >
                    >
                >
             ,variant>::type
             make_variant(const Cont& var) {
            return make_variant_list(var);
        }

            template<class Dict>
            typename boost::enable_if<
                boost::mpl::and_<
                    FB::meta::is_pair_assoc_container<Dict>,
                    boost::mpl::not_<
                            boost::is_same<std::map<std::string, variant>, Dict>
                    >
                >
             ,variant>::type
             make_variant(const Dict& var) {
                      return make_variant_map(var);
             }

        // Convert out
        template<class T>
        typename boost::enable_if<boost::is_base_of<FB::JSAPI, T>, std::shared_ptr<T> >::type
            convert_variant(const variant& var, type_spec< std::shared_ptr<T> >)
        {
            FB::JSAPIPtr ptr;
            // First of all, to succeed it *must* be a JSAPI object!
            if (var.get_type() == typeid(FB::JSObjectPtr)) {
                ptr = var.cast<FB::JSObjectPtr>();
            } else if (var.get_type() == typeid(FB::JSAPIWeakPtr)) {
                ptr = var.cast<FB::JSAPIWeakPtr>().lock();
            } else if (var.empty() || var.is_null()) {
                return std::shared_ptr<T>();
            } else {
                ptr = var.cast<FB::JSAPIPtr>();
            }
            if (!ptr)
                return std::shared_ptr<T>();

            std::shared_ptr<T> ret = std::dynamic_pointer_cast<T>(ptr);
            if (ret)
                return ret;
            else
                throw FB::bad_variant_cast(var.get_type(), typeid(T));
        }
        template<class T>
        typename boost::enable_if<boost::is_base_of<FB::JSAPI, T>, std::weak_ptr<T> >::type
            convert_variant(const variant& var, type_spec< std::weak_ptr<T> >)
        {
            std::shared_ptr<T> sptr(var.convert_cast<std::shared_ptr<T> >());
            return std::weak_ptr<T>(sptr);
        }

        template<class Cont>
        typename FB::meta::enable_for_non_assoc_containers<Cont, Promise<Cont>>::type
            convert_variant(const variant& var, type_spec<Cont>)
        {
            typedef FB::JSObjectPtr JsObject;
            
            // if the held data is of type Cont just return it

            if(var.is_of_type<Cont>()) 
                return var.cast<Cont>();

            // if the help data is not a JavaScript object throw

            if(!var.is_of_type<JsObject>())
                throw bad_variant_cast(var.get_type(), typeid(JsObject));
            
            // if it is a JavaScript object try to treat it as an array

            return FB::JSObject::GetArrayValues<Cont>(var.cast<JsObject>());
        }

        template<class Dict>
        typename FB::meta::enable_for_pair_assoc_containers<Dict, Promise<Dict>>::type
            convert_variant(const variant& var, type_spec<Dict>)
        {
            typedef FB::JSObjectPtr JsObject;
            
            // if the held data is of type Dict just return it

            if(var.is_of_type<Dict>()) 
                return var.cast<Dict>();

            // if the help data is not a JavaScript object throw

            if(!var.is_of_type<JsObject>())
                throw bad_variant_cast(var.get_type(), typeid(JsObject));
            
            // if it is a JavaScript object try to treat it as an array
            return FB::JSObject::GetObjectValues<Dict>(var.cast<JsObject>());
        }
    } }
};

#endif

