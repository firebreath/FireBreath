/**********************************************************\ 
Original Author: Georg Fritzsche

Created:    Jan 12, 2010
License:    Dual license model; choose one of two:
            New BSD License
            http://www.opensource.org/licenses/bsd-license.php
            - or -
            GNU Lesser General Public License, version 2.1
            http://www.gnu.org/licenses/lgpl-2.1.html

Copyright 2010 Georg Fritzsche, Firebreath development team
\**********************************************************/

#include <iostream>
#include <map>
#include <set>
#include <vector>
#include <string>
#include <cstdio>

#include "Util/meta_util.h"

class MetaTestEmptyClass {};

TEST(MetaTest)
{
    PRINT_TESTNAME;

    namespace m = FB::meta;
    namespace d = FB::meta::detail;    

    typedef MetaTestEmptyClass Empty;
    
    // has_type_*<> checks
    
    {
        CHECK((d::has_type_iterator< std::vector<int> >::value));
        CHECK((d::has_type_iterator< std::string >::value));
        CHECK((!d::has_type_iterator< Empty >::value));
    }

    {
        typedef std::vector<int> Test;

        CHECK((d::has_type_iterator       <Test>::value ));
        CHECK((d::has_type_const_iterator <Test>::value));
        CHECK((d::has_type_value_type     <Test>::value));
    //  CHECK((d::has_type_reference      <Test>::value));
    //  CHECK((d::has_type_const_reference<Test>::value));
        CHECK((d::has_type_pointer        <Test>::value));
        CHECK((d::has_type_difference_type<Test>::value));
        CHECK((d::has_type_size_type      <Test>::value));
    }

    // is_container_impl<> step by step

    {
        typedef std::vector<int> T;

        CHECK(d::is_container_impl<T>::has_memfun_begin);
        CHECK(d::is_container_impl<T>::has_memfun_end);
        CHECK(d::is_container_impl<T>::has_memfun_size);
        CHECK(d::is_container_impl<T>::has_memfun_max_size);
        CHECK(d::is_container_impl<T>::has_memfun_empty);
        CHECK(d::is_container_impl<T>::has_memfun_swap);
    }

    {
        typedef Empty T;

        CHECK((!d::is_container_impl<T>::has_memfun_begin));
        CHECK((!d::is_container_impl<T>::has_memfun_end));
        CHECK((!d::is_container_impl<T>::has_memfun_size));
        CHECK((!d::is_container_impl<T>::has_memfun_max_size));
        CHECK((!d::is_container_impl<T>::has_memfun_empty));
        CHECK((!d::is_container_impl<T>::has_memfun_swap));
    }
    
    // is_assoc_impl<> step by step

    {
        typedef std::map<int,int> T;

        CHECK((d::is_assoc_impl<T>::has_memfun_erase_1));
        CHECK((d::is_assoc_impl<T>::has_memfun_erase_2));
        CHECK((d::is_assoc_impl<T>::has_memfun_clear));
        CHECK((d::is_assoc_impl<T>::has_memfun_find));
        CHECK((d::is_assoc_impl<T>::has_memfun_count));
        CHECK((d::is_assoc_impl<T>::has_memfun_equal_range));
    }

    {
        typedef Empty T;

        CHECK((!d::is_assoc_impl<T>::has_memfun_erase_1));
        CHECK((!d::is_assoc_impl<T>::has_memfun_erase_2));
        CHECK((!d::is_assoc_impl<T>::has_memfun_clear));
        CHECK((!d::is_assoc_impl<T>::has_memfun_find));
        CHECK((!d::is_assoc_impl<T>::has_memfun_count));
        CHECK((!d::is_assoc_impl<T>::has_memfun_equal_range));
    }
    
    // is_pair_assoc_impl<>

    {
        typedef std::map<int,int> T;
        CHECK((d::has_type_mapped_type<T>::value));
        CHECK((d::check_pair_assoc_value_type<true, T>::value));
        CHECK((d::is_pair_assoc_impl<T>::has_mapped_type));
        CHECK((d::is_pair_assoc_impl<T>::value_type_is_pair));
    }

    {
        typedef Empty T;
        CHECK((!d::has_type_mapped_type<T>::value));
        CHECK((!d::check_pair_assoc_value_type<false, T>::value));
        CHECK((!d::is_pair_assoc_impl<T>::has_mapped_type));
        CHECK((!d::is_pair_assoc_impl<T>::value_type_is_pair));
    }

    // implementation checks

    {
        typedef std::map<int,int> MapIntInt;
        typedef std::vector<int> VecInt;
        typedef std::string String;

        CHECK((m::is_container<MapIntInt>::value));
        CHECK((m::is_assoc_container<MapIntInt>::value));
        CHECK((m::is_pseudo_container<std::string>::value));

        CHECK((!m::is_container<int>::value));
        CHECK((!m::is_assoc_container<String>::value));
        CHECK((!m::is_pseudo_container<VecInt>::value));
        CHECK((!m::is_pseudo_container<Empty>::value));
        CHECK((!m::is_container<Empty>::value));
        CHECK((!m::is_assoc_container<Empty>::value));
    }
}
