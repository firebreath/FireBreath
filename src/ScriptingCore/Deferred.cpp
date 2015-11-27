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

#include "APITypes.h"
#include "Deferred.h"

using namespace FB;

Promise<void> FB::Deferred<void>::promise() const {
    return Promise<void>(m_data);
}

void FB::Deferred<void>::invalidate() const {
    if (m_data && m_data->state == PromiseState::PENDING) {
        reject(std::make_exception_ptr(std::runtime_error("Deferred object destroyed: 3")));
    }
}

void FB::Deferred<void>::resolve(Promise<void> v) const {
    auto self(*this);
    auto onDone = [self]() {
        self.resolve();
    };
    auto onFail = [self](std::exception_ptr e) { self.reject(e); };
    promise().done(onDone, onFail);
}

namespace FB {
    template class Promise<FB::variant>;
    template class Promise<FB::VariantList>;
    template class Deferred<FB::variant>;
    template class Deferred<FB::VariantList>;
}
