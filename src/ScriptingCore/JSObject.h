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
        /// @fn JSObject(BrowserHostPtr h)
        ///
        /// @brief  Constructor
        ///
        /// @param  h   The BrowserHost from whence the object came. 
        ////////////////////////////////////////////////////////////////////////////////////////////////////
        JSObject(BrowserHostPtr h);

        ////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @fn virtual ~JSObject()
        ///
        /// @brief  Finaliser. 
        ////////////////////////////////////////////////////////////////////////////////////////////////////
        virtual ~JSObject();

        virtual void *getEventId() { return NULL; }
        virtual void *getEventContext() { return NULL; }

        ////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @fn virtual void InvokeAsync(const std::string& methodName, const std::vector<variant>& args)
        ///
        /// @brief  Just like Invoke, but works asynchronously.  Useful for javascript callbacks and events.
        /// 		Can be safely called from any thread
        ///
        /// @param  methodName  Name of the method. 
        /// @param  args        The arguments. 
        /// @see Invoke
        ////////////////////////////////////////////////////////////////////////////////////////////////////
        virtual void InvokeAsync(const std::string& methodName, const std::vector<variant>& args);

        ////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @fn virtual void SetPropertyAsync(const std::string& propertyName, const variant& value)
        ///
        /// @brief  Just like SetProperty, but works asynchronously.  Useful if you are running on another
        /// 		thread and don't need to wait to be sure it worked.
        ///
        /// @param  propertyName    Name of the property. 
        /// @param  value           The value. 
        ////////////////////////////////////////////////////////////////////////////////////////////////////
        virtual void SetPropertyAsync(const std::string& propertyName, const variant& value);

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
        /// 		container dst
        ///
        /// @param  src         Source for the. 
        /// @param [in,out] dst Destination for the. 
        ////////////////////////////////////////////////////////////////////////////////////////////////////
        template<class Dict>
        static void GetObjectValues(const FB::JSObjectPtr& src, Dict& dst);

    public:
        BrowserHostPtr host;
    };

    template<class Cont>
    void JSObject::GetArrayValues(const FB::JSObjectPtr& src, Cont& dst)
    {
        try
        {
            FB::variant tmp = src->GetProperty("length");
            long length = tmp.convert_cast<long>();
            std::back_insert_iterator<Cont> inserter(dst);

            for(int i=0; i<length; ++i) {
                tmp = src->GetProperty(i);
                *inserter++ = tmp.convert_cast<typename Cont::value_type>();
            }
        }
        catch(const FB::script_error& e) 
        {
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

        try 
        {
            StringVec fields;
            src->getMemberNames(fields);
            std::insert_iterator<Dict> inserter(dst, dst.begin());

            for(StringVec::iterator it = fields.begin(); it != fields.end(); it++) 
            {
                FB::variant tmp = src->GetProperty(*it);
                *inserter++ = PairType(*it, tmp.convert_cast<MappedType>());
            }
        } 
        catch (const FB::script_error& e)
        {
            throw e;
        }
    }
    
    // TODO: this doesn't belong here
    template<class Cont>
    typename FB::meta::enable_for_non_assoc_containers<Cont, const Cont>::type
    variant::convert_cast() const
    {
        typedef FB::JSObjectPtr JsObject;
        
        // if the held data is of type Cont just return it

        if(get_type() == typeid(Cont)) 
            return convert_cast_impl<Cont>();

        // if the help data is not a JavaScript object throw

        if(!(get_type() == typeid(JsObject)))
            throw bad_variant_cast(get_type(), typeid(JsObject));
        
        // if it is a JavaScript object try to treat it as an array

        Cont cont;
        FB::JSObject::GetArrayValues(cast<JsObject>(), cont);
        return cont;
    }

    // TODO: this doesn't belong here
    template<class Dict>
    typename FB::meta::enable_for_pair_assoc_containers<Dict, const Dict>::type
    variant::convert_cast() const
    {
        typedef FB::JSObjectPtr JsObject;
        
        // if the held data is of type Dict just return it

        if(get_type() == typeid(Dict)) 
            return convert_cast_impl<Dict>();

        // if the help data is not a JavaScript object throw

        if(!(get_type() == typeid(JsObject)))
            throw bad_variant_cast(get_type(), typeid(JsObject));
        
        // if it is a JavaScript object try to treat it as an array

        Dict dict;
        FB::JSObject::GetObjectValues(cast<JsObject>(), dict);
        return dict;
    }
};

#endif
