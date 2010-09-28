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

#ifndef H_FB_JSObject
#define H_FB_JSObject

#include "JSAPI.h"
#include "BrowserHostWrapper.h"
#include <iterator>

namespace FB
{
    class JSObject : public FB::JSAPI
    {
    public:
        JSObject(BrowserHostPtr h);
        virtual ~JSObject();

        virtual void *getEventId() { return NULL; }
        virtual void *getEventContext() { return NULL; }

        virtual void InvokeAsync(const std::string& methodName, const std::vector<variant>& args);

    protected:
        virtual variant InvokeMainThread(const std::string& methodName, const std::vector<variant>& args);

    public:
        // TODO: Find a better place for this conversion method.
        //       Gotcha to watch out for: has to be included after variant.h
        //       and everywhere where variant::convert_cast<SomeContainer>()
        //       is used.
        template<class Cont>
        static void GetArrayValues(const FB::JSObjectPtr& src, Cont& dst);
        
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
