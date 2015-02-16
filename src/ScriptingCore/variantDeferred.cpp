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

#include "variantDeferred.h"
#include <stdexcept>

namespace FB {
    struct VariantPromiseData
    {
        using State = variantDeferred::State;
        VariantPromiseData() : m_state(State::PENDING) {}
        VariantPromiseData(variant v) : m_value(v), m_state(State::RESOLVED) {}

        ~VariantPromiseData() {
            if (m_state == State::PENDING) {
                reject(std::runtime_error("Deferred object destroyed"));
            }
        }

        void reject(std::exception e) {
            m_err = e;
            m_state = State::REJECTED;
            for (auto fn : m_onReject) {
                fn(e);
            }
            m_onReject.clear();
        }
        void resolve(variant v) {
            m_value = v;
            m_state = State::RESOLVED;
            for (auto fn : m_onResolve) {
                fn(v);
            }
            m_onResolve.clear();
        }
        variant m_value;
        State m_state;
        std::exception m_err;

        std::vector<variantDeferred::DeferredCallback> m_onResolve;
        std::vector<variantDeferred::DeferredCallback> m_onReject;
    };
}

FB::variantDeferredPtr FB::variantDeferred::makeDeferred(variant v) {
    // If the variant just wraps a variantDeferredPtr then return that
    if (v.is_of_type<FB::variantDeferredPtr>()) { return v.cast<FB::variantDeferredPtr>(); }
    return std::make_shared<variantDeferred>(v, PrivateOnly());
}

FB::variantDeferred::variantDeferred(FB::variant v, PrivateOnly) : pData(new FB::VariantPromiseData(v)) { }

FB::variantDeferred::variantDeferred(PrivateOnly) : pData(new FB::VariantPromiseData()) { }

FB::variantDeferred::~variantDeferred()
{
    invalidate();
}

// TODO: Implement chaining
void FB::variantDeferred::then(DeferredCallback successFn, DeferredCallback failFn)
{
    this->done(successFn);
    this->fail(failFn);
}

void FB::variantDeferred::done(DeferredCallback successFn)
{
    if (pData->m_state == State::PENDING) {
        pData->m_onResolve.emplace_back(successFn);
    }
    else if (pData->m_state == State::RESOLVED) {
        successFn(pData->m_value);
    }
}

void FB::variantDeferred::fail(DeferredCallback failFn)
{
    if (pData->m_state == State::PENDING) {
        pData->m_onReject.emplace_back(failFn);
    }
    else if (pData->m_state == State::REJECTED) {
        failFn(pData->m_err);
    }
}

void FB::variantDeferred::always(DeferredCallback alwaysFn)
{
    done(alwaysFn);
    fail(alwaysFn);
}

void FB::variantDeferred::resolve(variant v)
{
    if (pData) {
        pData->resolve(v);
    }
}

void FB::variantDeferred::reject(std::exception e)
{
    if (pData) {
        pData->reject(e);
    }
}
