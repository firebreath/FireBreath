/**********************************************************\ 
Original Author: Georg Fritzsche

Created:    December 13, 2009
License:    Dual license model; choose one of two:
            Eclipse Public License - Version 1.0
            http://www.eclipse.org/legal/epl-v10.html
            - or -
            GNU Lesser General Public License, version 2.1
            http://www.gnu.org/licenses/lgpl-2.1.html

Copyright 2009 Georg Fritzsche, Firebreath development team
\**********************************************************/

#ifndef H_JSARRAY
#define H_JSARRAY

#include "../BrowserObjectAPI.h"
#include "../JSAPIAuto.h"

namespace FB
{
    class JSArray : public JSAPIAuto
    {
    public:
        JSArray();
        explicit JSArray(FB::AutoPtr<FB::BrowserObjectAPI> obj);
        explicit JSArray(const FB::VariantList& values);

        FB::variant GetProperty(int index);
        using FB::JSAPIAuto::GetProperty; // unhide GetProperty(std::string)
        size_t GetLength() const;

        FB::variant& operator[](size_t index);
        const FB::variant& operator[](size_t index) const;

        FB::VariantList& Values();
        const FB::VariantList& Values() const;

    private:
        void RegisterMethods();
        void ExtractList(FB::AutoPtr<FB::BrowserObjectAPI>);

        FB::VariantList m_values;
    };
}

#endif
