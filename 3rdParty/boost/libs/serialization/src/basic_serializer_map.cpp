/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////8
// serializer_map.cpp:

// (C) Copyright 2002 Robert Ramey - http://www.rrsd.com . 
// Use, modification and distribution is subject to the Boost Software
// License, Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

//  See http://www.boost.org for updates, documentation, and revision history.

#if (defined _MSC_VER) && (_MSC_VER == 1200)
#  pragma warning (disable : 4786) // too long name, harmless warning
#endif

#define BOOST_ARCHIVE_SOURCE
#include <boost/archive/detail/basic_serializer.hpp>
#include <boost/archive/detail/basic_serializer_map.hpp>

namespace boost {
    namespace serialization {
        class extended_type_info;
    }
namespace archive {
namespace detail {

// note: We can't implement this as an associative
// container as such a container might be dependent
// upon the extended_type_info::m_key which might not
// be assigned until later.  So we use a "slower" method.
// This not a big deal however as the slower "find" operations
// operations are only called occasionally:
// a) At module unloading
// b) Once per input archive - the value is cached in the archive
//    implemenation.

BOOST_ARCHIVE_DECL(void) 
basic_serializer_map::insert(const basic_serializer * bs){
    m_map.push_back(bs);
}

BOOST_ARCHIVE_DECL(void) 
basic_serializer_map::erase(const basic_serializer * bs){
    map_type::iterator it = m_map.begin();
    map_type::iterator it_end = m_map.end();
    it = std::find(it, it_end, bs);
    if(it != it_end){
        m_map.erase(it);
    }
    else
        assert(false); // this should never occur
}

class equals {
    const boost::serialization::extended_type_info * m_eti;
public:
    bool operator()(const basic_serializer * bs) const {
        return *m_eti == bs->get_eti();
    }
    equals(const boost::serialization::extended_type_info * eti) :
        m_eti(eti)
    {}
};

// find the "oldest" matching pointer serializer
BOOST_ARCHIVE_DECL(const basic_serializer *)
basic_serializer_map::find(
    const boost::serialization::extended_type_info & eti
) const {
    map_type::const_iterator it = m_map.begin();
    map_type::const_iterator it_end = m_map.end();
    it = std::find_if(it, it_end, equals(& eti));
    assert(it != m_map.end());
    return *it;
}

} // namespace detail
} // namespace archive
} // namespace boost

