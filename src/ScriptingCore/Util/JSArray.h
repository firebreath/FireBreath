/**********************************************************\ 
Original Author: Georg Fritzsche

Created:    December 13, 2009
License:    Dual license model; choose one of two:
            Eclipse Public License - Version 1.0
            http://www.eclipse.org/legal/epl-v10.html
            - or -
            GNU Lesser General Public License, version 2.1
            http://www.gnu.org/licenses/lgpl-2.1.html

Copyright 2009 Georg Fritzsche, Firebreath development team
\**********************************************************/

#ifndef H_JSARRAY
#define H_JSARRAY

#include "../BrowserObjectAPI.h"
#include "../JSAPIAuto.h"

namespace FB
{
	class JSArray : public JSAPIAuto
	{
	public:
		JSArray();
		explicit JSArray(FB::AutoPtr<FB::BrowserObjectAPI> obj);
		explicit JSArray(const FB::VariantList& values);

		FB::variant GetProperty(int index);
		using FB::JSAPIAuto::GetProperty; // unhide GetProperty(std::string)
		size_t GetLength() const;

		FB::variant& operator[](size_t index);
		const FB::variant& operator[](size_t index) const;

		FB::VariantList& Values();
		const FB::VariantList& Values() const;

		FB::VariantList CopyValues() const;

		// convencience conversion to a vector

		template<class Cont>
		void ToContainer(Cont& vec) const;

		template<class Cont>
		Cont ToContainer() const;

		template<typename T>
		std::vector<T> ToVectorOf() const;

	private:
		void RegisterMethods();

		FB::VariantList m_values;
	};
}

namespace FB
{
	template<class Cont>
	void JSArray::ToContainer(Cont &cont) const
	{
		typedef Cont::value_type T;
		std::back_insert_iterator<Cont> inserter(cont);

		try
		{
			FB::VariantList::const_iterator it(m_values.begin());
			for( ; it != m_values.end(); ++it)
				inserter = it->convert_cast<T>();
		} 
		catch(FB::bad_variant_cast& e) 
		{
			throw e;
		}
	}

	template<typename T>
	std::vector<T> JSArray::ToVectorOf() const
	{
		std::vector<T> c;
		ToContainer(c);
		return c;
	}

	template<class Cont>
	Cont JSArray::ToContainer() const
	{
		Cont c;
		ToContainer(c);
		return c;
	}
}

#endif
