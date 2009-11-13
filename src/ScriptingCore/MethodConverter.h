/**********************************************************\
Original Author: Georg Fritzsche
Generated on: 2009-11-09
License:      Eclipse Public License - Version 1.0
              http://www.eclipse.org/legal/epl-v10.html

Copyright 2009 Georg Fritzsche, Firebreath development team
\**********************************************************/


#ifndef METHOD_CONVERTER_H
#define METHOD_CONVERTER_H

#include <boost/function.hpp>
#include <boost/bind.hpp>
#include "ConverterUtils.h"

namespace FB
{
	namespace detail { namespace methods
	{
		template<class C, typename F>
		struct method_wrapper0 {
			typedef FB::variant result_type;
			F f;
			method_wrapper0(F f) : f(f) {}
			FB::variant operator()(C* instance, std::vector<FB::variant>& in)
			{
				if(in.size() != 0) throw FB::invalid_arguments();
				return (instance->*f)();
			}
		};
		template<class C, typename T0, typename F>
		struct method_wrapper1 {
			typedef FB::variant result_type;
			F f;
			method_wrapper1(F f) : f(f) {}
			FB::variant operator()(C* instance, std::vector<FB::variant>& in)
			{
				if(in.size() != 1) throw FB::invalid_arguments();
				return (instance->*f)(in[0].convert_cast<typename FB::detail::plain_type<T0>::type>());
			}
		};
		template<class C, typename T0, typename T1, typename F>
		struct method_wrapper2 {
			typedef FB::variant result_type;
			F f;
			method_wrapper2(F f) : f(f) {}
			FB::variant operator()(C* instance, std::vector<FB::variant>& in)
			{
				if(in.size() != 2) throw FB::invalid_arguments();
				return (instance->*f)(in[0].convert_cast<typename FB::detail::plain_type<T0>::type>(), in[1].convert_cast<typename FB::detail::plain_type<T1>::type>());
			}
		};
		template<class C, typename T0, typename T1, typename T2, typename F>
		struct method_wrapper3 {
			typedef FB::variant result_type;
			F f;
			method_wrapper3(F f) : f(f) {}
			FB::variant operator()(C* instance, std::vector<FB::variant>& in)
			{
				if(in.size() != 3) throw FB::invalid_arguments();
				return (instance->*f)(in[0].convert_cast<typename FB::detail::plain_type<T0>::type>(), in[1].convert_cast<typename FB::detail::plain_type<T1>::type>(), in[2].convert_cast<typename FB::detail::plain_type<T2>::type>());
			}
		};
		template<class C, typename T0, typename T1, typename T2, typename T3, typename F>
		struct method_wrapper4 {
			typedef FB::variant result_type;
			F f;
			method_wrapper4(F f) : f(f) {}
			FB::variant operator()(C* instance, std::vector<FB::variant>& in)
			{
				if(in.size() != 4) throw FB::invalid_arguments();
				return (instance->*f)(in[0].convert_cast<typename FB::detail::plain_type<T0>::type>(), in[1].convert_cast<typename FB::detail::plain_type<T1>::type>(), in[2].convert_cast<typename FB::detail::plain_type<T2>::type>(), in[3].convert_cast<typename FB::detail::plain_type<T3>::type>());
			}
		};
		template<class C, typename T0, typename T1, typename T2, typename T3, typename T4, typename F>
		struct method_wrapper5 {
			typedef FB::variant result_type;
			F f;
			method_wrapper5(F f) : f(f) {}
			FB::variant operator()(C* instance, std::vector<FB::variant>& in)
			{
				if(in.size() != 5) throw FB::invalid_arguments();
				return (instance->*f)(in[0].convert_cast<typename FB::detail::plain_type<T0>::type>(), in[1].convert_cast<typename FB::detail::plain_type<T1>::type>(), in[2].convert_cast<typename FB::detail::plain_type<T2>::type>(), in[3].convert_cast<typename FB::detail::plain_type<T3>::type>(), in[4].convert_cast<typename FB::detail::plain_type<T4>::type>());
			}
		};
		template<class C, typename T0, typename T1, typename T2, typename T3, typename T4, typename T5, typename F>
		struct method_wrapper6 {
			typedef FB::variant result_type;
			F f;
			method_wrapper6(F f) : f(f) {}
			FB::variant operator()(C* instance, std::vector<FB::variant>& in)
			{
				if(in.size() != 6) throw FB::invalid_arguments();
				return (instance->*f)(in[0].convert_cast<typename FB::detail::plain_type<T0>::type>(), in[1].convert_cast<typename FB::detail::plain_type<T1>::type>(), in[2].convert_cast<typename FB::detail::plain_type<T2>::type>(), in[3].convert_cast<typename FB::detail::plain_type<T3>::type>(), in[4].convert_cast<typename FB::detail::plain_type<T4>::type>(), in[5].convert_cast<typename FB::detail::plain_type<T5>::type>());
			}
		};
		template<class C, typename T0, typename T1, typename T2, typename T3, typename T4, typename T5, typename T6, typename F>
		struct method_wrapper7 {
			typedef FB::variant result_type;
			F f;
			method_wrapper7(F f) : f(f) {}
			FB::variant operator()(C* instance, std::vector<FB::variant>& in)
			{
				if(in.size() != 7) throw FB::invalid_arguments();
				return (instance->*f)(in[0].convert_cast<typename FB::detail::plain_type<T0>::type>(), in[1].convert_cast<typename FB::detail::plain_type<T1>::type>(), in[2].convert_cast<typename FB::detail::plain_type<T2>::type>(), in[3].convert_cast<typename FB::detail::plain_type<T3>::type>(), in[4].convert_cast<typename FB::detail::plain_type<T4>::type>(), in[5].convert_cast<typename FB::detail::plain_type<T5>::type>(), in[6].convert_cast<typename FB::detail::plain_type<T6>::type>());
			}
		};
		template<class C, typename T0, typename T1, typename T2, typename T3, typename T4, typename T5, typename T6, typename T7, typename F>
		struct method_wrapper8 {
			typedef FB::variant result_type;
			F f;
			method_wrapper8(F f) : f(f) {}
			FB::variant operator()(C* instance, std::vector<FB::variant>& in)
			{
				if(in.size() != 8) throw FB::invalid_arguments();
				return (instance->*f)(in[0].convert_cast<typename FB::detail::plain_type<T0>::type>(), in[1].convert_cast<typename FB::detail::plain_type<T1>::type>(), in[2].convert_cast<typename FB::detail::plain_type<T2>::type>(), in[3].convert_cast<typename FB::detail::plain_type<T3>::type>(), in[4].convert_cast<typename FB::detail::plain_type<T4>::type>(), in[5].convert_cast<typename FB::detail::plain_type<T5>::type>(), in[6].convert_cast<typename FB::detail::plain_type<T6>::type>(), in[7].convert_cast<typename FB::detail::plain_type<T7>::type>());
			}
		};
		template<class C, typename T0, typename T1, typename T2, typename T3, typename T4, typename T5, typename T6, typename T7, typename T8, typename F>
		struct method_wrapper9 {
			typedef FB::variant result_type;
			F f;
			method_wrapper9(F f) : f(f) {}
			FB::variant operator()(C* instance, std::vector<FB::variant>& in)
			{
				if(in.size() != 9) throw FB::invalid_arguments();
				return (instance->*f)(in[0].convert_cast<typename FB::detail::plain_type<T0>::type>(), in[1].convert_cast<typename FB::detail::plain_type<T1>::type>(), in[2].convert_cast<typename FB::detail::plain_type<T2>::type>(), in[3].convert_cast<typename FB::detail::plain_type<T3>::type>(), in[4].convert_cast<typename FB::detail::plain_type<T4>::type>(), in[5].convert_cast<typename FB::detail::plain_type<T5>::type>(), in[6].convert_cast<typename FB::detail::plain_type<T6>::type>(), in[7].convert_cast<typename FB::detail::plain_type<T7>::type>(), in[8].convert_cast<typename FB::detail::plain_type<T8>::type>());
			}
		};
		template<class C, typename T0, typename T1, typename T2, typename T3, typename T4, typename T5, typename T6, typename T7, typename T8, typename T9, typename F>
		struct method_wrapper10 {
			typedef FB::variant result_type;
			F f;
			method_wrapper10(F f) : f(f) {}
			FB::variant operator()(C* instance, std::vector<FB::variant>& in)
			{
				if(in.size() != 10) throw FB::invalid_arguments();
				return (instance->*f)(in[0].convert_cast<typename FB::detail::plain_type<T0>::type>(), in[1].convert_cast<typename FB::detail::plain_type<T1>::type>(), in[2].convert_cast<typename FB::detail::plain_type<T2>::type>(), in[3].convert_cast<typename FB::detail::plain_type<T3>::type>(), in[4].convert_cast<typename FB::detail::plain_type<T4>::type>(), in[5].convert_cast<typename FB::detail::plain_type<T5>::type>(), in[6].convert_cast<typename FB::detail::plain_type<T6>::type>(), in[7].convert_cast<typename FB::detail::plain_type<T7>::type>(), in[8].convert_cast<typename FB::detail::plain_type<T8>::type>(), in[9].convert_cast<typename FB::detail::plain_type<T9>::type>());
			}
		};
	} } // namespace detail::methods
	
	template<class C, typename R>
	inline FB::CallMethodFunctor
	make_method(C* instance, R (C::*function)())
	{
		return boost::bind(FB::detail::methods::method_wrapper0<C, R (C::*)()>(function), instance, _1);
	}
	
	template<class C, typename R, typename T0>
	inline FB::CallMethodFunctor
	make_method(C* instance, R (C::*function)(T0))
	{
		return boost::bind(FB::detail::methods::method_wrapper1<C, T0, R (C::*)(T0)>(function), instance, _1);
	}
	
	template<class C, typename R, typename T0, typename T1>
	inline FB::CallMethodFunctor
	make_method(C* instance, R (C::*function)(T0, T1))
	{
		return boost::bind(FB::detail::methods::method_wrapper2<C, T0, T1, R (C::*)(T0, T1)>(function), instance, _1);
	}
	
	template<class C, typename R, typename T0, typename T1, typename T2>
	inline FB::CallMethodFunctor
	make_method(C* instance, R (C::*function)(T0, T1, T2))
	{
		return boost::bind(FB::detail::methods::method_wrapper3<C, T0, T1, T2, R (C::*)(T0, T1, T2)>(function), instance, _1);
	}
	
	template<class C, typename R, typename T0, typename T1, typename T2, typename T3>
	inline FB::CallMethodFunctor
	make_method(C* instance, R (C::*function)(T0, T1, T2, T3))
	{
		return boost::bind(FB::detail::methods::method_wrapper4<C, T0, T1, T2, T3, R (C::*)(T0, T1, T2, T3)>(function), instance, _1);
	}
	
	template<class C, typename R, typename T0, typename T1, typename T2, typename T3, typename T4>
	inline FB::CallMethodFunctor
	make_method(C* instance, R (C::*function)(T0, T1, T2, T3, T4))
	{
		return boost::bind(FB::detail::methods::method_wrapper5<C, T0, T1, T2, T3, T4, R (C::*)(T0, T1, T2, T3, T4)>(function), instance, _1);
	}
	
	template<class C, typename R, typename T0, typename T1, typename T2, typename T3, typename T4, typename T5>
	inline FB::CallMethodFunctor
	make_method(C* instance, R (C::*function)(T0, T1, T2, T3, T4, T5))
	{
		return boost::bind(FB::detail::methods::method_wrapper6<C, T0, T1, T2, T3, T4, T5, R (C::*)(T0, T1, T2, T3, T4, T5)>(function), instance, _1);
	}
	
	template<class C, typename R, typename T0, typename T1, typename T2, typename T3, typename T4, typename T5, typename T6>
	inline FB::CallMethodFunctor
	make_method(C* instance, R (C::*function)(T0, T1, T2, T3, T4, T5, T6))
	{
		return boost::bind(FB::detail::methods::method_wrapper7<C, T0, T1, T2, T3, T4, T5, T6, R (C::*)(T0, T1, T2, T3, T4, T5, T6)>(function), instance, _1);
	}
	
	template<class C, typename R, typename T0, typename T1, typename T2, typename T3, typename T4, typename T5, typename T6, typename T7>
	inline FB::CallMethodFunctor
	make_method(C* instance, R (C::*function)(T0, T1, T2, T3, T4, T5, T6, T7))
	{
		return boost::bind(FB::detail::methods::method_wrapper8<C, T0, T1, T2, T3, T4, T5, T6, T7, R (C::*)(T0, T1, T2, T3, T4, T5, T6, T7)>(function), instance, _1);
	}
	
	template<class C, typename R, typename T0, typename T1, typename T2, typename T3, typename T4, typename T5, typename T6, typename T7, typename T8>
	inline FB::CallMethodFunctor
	make_method(C* instance, R (C::*function)(T0, T1, T2, T3, T4, T5, T6, T7, T8))
	{
		return boost::bind(FB::detail::methods::method_wrapper9<C, T0, T1, T2, T3, T4, T5, T6, T7, T8, R (C::*)(T0, T1, T2, T3, T4, T5, T6, T7, T8)>(function), instance, _1);
	}
	
	template<class C, typename R, typename T0, typename T1, typename T2, typename T3, typename T4, typename T5, typename T6, typename T7, typename T8, typename T9>
	inline FB::CallMethodFunctor
	make_method(C* instance, R (C::*function)(T0, T1, T2, T3, T4, T5, T6, T7, T8, T9))
	{
		return boost::bind(FB::detail::methods::method_wrapper10<C, T0, T1, T2, T3, T4, T5, T6, T7, T8, T9, R (C::*)(T0, T1, T2, T3, T4, T5, T6, T7, T8, T9)>(function), instance, _1);
	}
	
} // namespace FB

#endif //METHOD_CONVERTER_H