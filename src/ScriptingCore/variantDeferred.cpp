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

FB::variantDeferred::variantDeferred(FB::variant v, PrivateOnly) : m_value(v), m_state(State::RESOLVED) { }

FB::variantDeferred::variantDeferred(PrivateOnly) : m_state(State::PENDING) { }

FB::variantDeferred::~variantDeferred()
{
    if (m_state == State::PENDING) {
        reject(std::runtime_error("Deferred object destroyed"));
    }
}

// TODO: Implement chaining
void FB::variantDeferred::then(DeferredCallback successFn, DeferredCallback failFn)
{
    this->done(successFn);
    this->fail(failFn);
}

void FB::variantDeferred::done(DeferredCallback successFn)
{
    if (m_state == State::PENDING) {
        m_onResolve.emplace_back(successFn);
    }
    else if (m_state == State::RESOLVED) {
        successFn(m_value);
    }
}

void FB::variantDeferred::fail(DeferredCallback failFn)
{
    if (m_state == State::PENDING) {
        m_onReject.emplace_back(failFn);
    }
    else if (m_state == State::REJECTED) {
        failFn(m_err);
    }
}

void FB::variantDeferred::always(DeferredCallback alwaysFn)
{
    done(alwaysFn);
    fail(alwaysFn);
}

void FB::variantDeferred::resolve(variant v)
{
    m_value = v;
    m_state = State::RESOLVED;
    for (auto fn : m_onResolve) {
        fn(v);
    }
    m_onResolve.clear();
}

void FB::variantDeferred::reject(std::exception e)
{
    m_err = e;
    m_state = State::REJECTED;
    for (auto fn : m_onReject) {
        fn(e);
    }
    m_onReject.clear();
}
