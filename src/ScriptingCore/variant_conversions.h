/**********************************************************\ 
 Original Author: Richard Bateman
 
 Created:    Dec 30, 2010
 License:    Dual license model; choose one of two:
 New BSD License
 http://www.opensource.org/licenses/bsd-license.php
 - or -
 GNU Lesser General Public License, version 2.1
 http://www.gnu.org/licenses/lgpl-2.1.html
 
 Copyright 2009 Richard Bateman, Firebreath development team
\**********************************************************/

#include <boost/shared_ptr.hpp>

namespace FB {
    class variant;
    class JSAPI;
    class JSObject;
    namespace variant_detail {
        namespace conversion {
            template <class T>
            struct type_spec
            {};
            
            template <class T>
            typename boost::enable_if<
                boost::mpl::and_<
                boost::is_base_of<FB::JSAPI, T>,
                boost::mpl::not_<boost::is_base_of<FB::JSObject, T> > >
                ,variant>::type
            make_variant(const boost::shared_ptr<T>& ptr);
            
            template <class T>
            typename boost::enable_if<boost::is_base_of<FB::JSObject, T>,variant>::type
            make_variant(const boost::shared_ptr<T>& ptr);
            
            template<class T>
            typename boost::enable_if<boost::is_base_of<FB::JSAPI, T>, boost::shared_ptr<T> >::type
            convert_variant(const variant& var, variant_detail::conversion::type_spec< boost::shared_ptr<T> >);
            
            template<class Cont>
            typename FB::meta::enable_for_non_assoc_containers<Cont, const Cont>::type
            convert_variant(const variant& var, type_spec<Cont>);
            
            template<class Dict>
            typename FB::meta::enable_for_pair_assoc_containers<Dict, const Dict>::type
            convert_variant(const variant& var, type_spec<Dict>);
        }
    }
}

