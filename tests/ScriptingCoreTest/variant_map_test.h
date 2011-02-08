/**********************************************************\ 
Original Author: Georg Fritzsche

Created:    Dec 13, 2009
License:    Dual license model; choose one of two:
            New BSD License
            http://www.opensource.org/licenses/bsd-license.php
            - or -
            GNU Lesser General Public License, version 2.1
            http://www.gnu.org/licenses/lgpl-2.1.html

Copyright 2009 Georg Fritzsche, Firebreath development team
\**********************************************************/


#include <map>
#include <boost/assign/list_of.hpp>
#include "APITypes.h"
#include "variant_map.h"

TEST(VariantMapTest)
{
    PRINT_TESTNAME;

    using boost::assign::list_of;
    
    {
        typedef std::map<std::string, FB::variant> VariantMap;
        typedef std::pair<VariantMap::key_type, VariantMap::mapped_type> MapPair;
        typedef std::vector<std::string> StringVec;
        
        StringVec names = list_of("a")("b")("c")("d")("e");
        VariantMap values = FB::variant_map_of<std::string>("a","a")("b","b")("c","c")("d","d")("e","e");

        StringVec::const_iterator  itn = names.begin();
        VariantMap::const_iterator itv = values.begin();

        for( ; itn!=names.end() && itv!=values.end(); ++itn, ++itv)
        {
            CHECK(*itn == itv->first);
            CHECK(*itn == itv->second.convert_cast<std::string>());
        }
    }
}

