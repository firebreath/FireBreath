/**********************************************************\ 
Original Author: Richard Bateman (taxilian)

Created:    Oct 17, 2009
License:    Dual license model; choose one of two:
            Eclipse Public License - Version 1.0
            http://www.eclipse.org/legal/epl-v10.html
            - or -
            GNU Lesser General Public License, version 2.1
            http://www.gnu.org/licenses/lgpl-2.1.html

Copyright 2009 Richard Bateman, Firebreath development team
\**********************************************************/

#ifndef H_FB_BrowserObjectAPI
#define H_FB_BrowserObjectAPI

#include "JSAPI.h"
#include "BrowserHostWrapper.h"
#include "AutoPtr.h"

namespace FB
{
    class BrowserObjectAPI : public FB::JSAPI
    {
    public:
        BrowserObjectAPI(BrowserHostWrapper *h);
        virtual ~BrowserObjectAPI();

        virtual void *getEventId() { return NULL; }
        virtual void *getEventContext() { return NULL; }

        virtual void InvokeAsync(std::string methodName, std::vector<variant>& args);

        // TODO: Find a better place for this conversion method.
        //       Gotcha to watch out for: has to be included after variant.h
        //       and everywhere where variant::convert_cast<SomeContainer>()
        //       is used.
        template<class Cont>
        static void GetArrayValues(FB::JSObject src, Cont& dst);
        
        template<class Dict>
        void GetObjectValues(FB::JSObject src, Dict& dst);

    public:
        AutoPtr<BrowserHostWrapper> host;
    };

    template<class Cont>
    void BrowserObjectAPI::GetArrayValues(FB::JSObject src, Cont& dst)
    {
        try
        {
            long length = src->GetProperty("length").convert_cast<long>();
            std::back_insert_iterator<Cont> inserter(dst);

            for(int i=0; i<length; ++i) {
                *inserter++ = src->GetProperty(i).convert_cast<Cont::value_type>();
            }
        }
        catch(FB::script_error& e) 
        {
            throw e;
        }
    }

    template<class Dict>
    void BrowserObjectAPI::GetObjectValues(FB::JSObject src, Dict& dst)
    {
        try 
        {
            std::vector<std::string> fields;
            src->getMemberNames(fields);
            std::insert_iterator<Dict> inserter(dst, dst.begin());
            for (std::vector<std::string>::iterator it = fields.begin(); it != fields.end(); it++) {
                *inserter++ = std::pair<Dict::key_type, Dict::mapped_type>
                    (*it, src->GetProperty(*it).convert_cast<Dict::mapped_type>());
            }
        } catch (FB::script_error& e)
        {
            throw e;
        }
    }
};

#endif