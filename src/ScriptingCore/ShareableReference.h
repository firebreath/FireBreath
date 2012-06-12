#ifndef ShareableReference_h__
#define ShareableReference_h__

/**********************************************************\
Original Author: Richard Bateman (taxilian)

Created:    Feb 19, 2011
License:    Dual license model; choose one of two:
            New BSD License
            http://www.opensource.org/licenses/bsd-license.php
            - or -
            GNU Lesser General Public License, version 2.1
            http://www.gnu.org/licenses/lgpl-2.1.html

Copyright 2009 Richard Bateman, Firebreath development team
\**********************************************************/

#include <boost/enable_shared_from_this.hpp>

namespace FB {

    template <class C>
    class ShareableReference : boost::noncopyable, public boost::enable_shared_from_this<ShareableReference<C> >
    {
    public:
        ShareableReference(C* ptr) : m_ptr(ptr) {}
        C* getPtr() { return m_ptr; }

    private:
        C* m_ptr;
    };

};
#endif // ShareableReference_h__
