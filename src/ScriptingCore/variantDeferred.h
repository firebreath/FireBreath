/**********************************************************\ 
Original Author: Richard Bateman (taxilian)

Created:    Feb 10, 2015
License:    Dual license model; choose one of two:
            New BSD License
            http://www.opensource.org/licenses/bsd-license.php
            - or -
            GNU Lesser General Public License, version 2.1
            http://www.gnu.org/licenses/lgpl-2.1.html

Copyright 2015 Richard Bateman and the FireBreath Dev Team
\**********************************************************/

#pragma once
#ifndef H_VARIANTDEFERRED
#define H_VARIANTDEFERRED

#include <functional>
#include <APITypes.h>
#include <boost/noncopyable.hpp>

namespace FB {
    struct VariantPromiseData;
    FB_FORWARD_PTR(variantDeferred);
    class variantDeferred final : boost::noncopyable {
    public:
        using DeferredCallback = std::function < void(variant) > ;
        enum class State {PENDING, RESOLVED, REJECTED};

        static variantDeferredPtr makeDeferred() { return std::make_shared<variantDeferred>(PrivateOnly()); }
        static variantDeferredPtr makeDeferred(variant v);

    private:
        struct PrivateOnly {};
    public:
        variantDeferred(variant v, PrivateOnly);
        variantDeferred(PrivateOnly);
        
        ~variantDeferred();

        void invalidate() {
            pData.release();
        }

        void then(DeferredCallback success, DeferredCallback fail);
        void done(DeferredCallback success);
        void fail(DeferredCallback fail);
        void always(DeferredCallback always);

        void resolve(variant v);
        void reject(std::exception e);

    private:
        std::unique_ptr<VariantPromiseData> pData;
    };
}

#endif // H_VARIANTDEFERRED