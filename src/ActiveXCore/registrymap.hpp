/** \file registrymap.hpp
  * Defines a 'registry' map for adding variables to rgs files.
  * Copyright 2001-2003 Michael Geddes.  All rights reserved.
  * Found here: http://www.codeproject.com/KB/atl/RegistryMap.aspx
  * Not licensed with the rest of this project, we don't own the copyright
  */

#pragma once
#ifndef COMLIB_REGISTRYMAP_HPP
#define COMLIB_REGISTRYMAP_HPP

#include <atlbase.h>

struct _ATL_REGMAP_ENTRYKeeper : public _ATL_REGMAP_ENTRY
{
	_ATL_REGMAP_ENTRYKeeper(){ szKey=NULL; szData=NULL;}
    _ATL_REGMAP_ENTRYKeeper(LPCOLESTR key, LPCSTR cstrData)
    {
        USES_CONVERSION;
        CA2W data(cstrData);
        szKey=key;
		LPOLESTR newData;
		szData = LPCOLESTR(newData =  new wchar_t[wcslen(data)+1]);
		wcscpy_s(newData,wcslen(data) + 1,data);
    }
	_ATL_REGMAP_ENTRYKeeper(LPCOLESTR key, LPCOLESTR data) 
	{
		szKey=key;
		LPOLESTR newData;
		szData = LPCOLESTR(newData =  new wchar_t[wcslen(data)+1]);
		wcscpy_s(newData,wcslen(data)+1,data);
	}
	_ATL_REGMAP_ENTRYKeeper(LPCOLESTR key, UINT resid)
	{
		static wchar_t Data[256];
		USES_CONVERSION;

		szKey=key;
		if( 0 == LoadStringW(_pModule->m_hInstResource, resid, Data, 255))
		{
			*Data = '\0';
		}

		int	length = wcslen(Data);

		szData = new wchar_t[length];
		wcscpy_s(const_cast<wchar_t *>(szData), length, Data);
	}

	_ATL_REGMAP_ENTRYKeeper(LPCOLESTR key, REFGUID guid) 
	{
		szKey=key;
		LPOLESTR newData;
		szData = LPCOLESTR(newData =  new wchar_t[40]);
		if(szData!=NULL)
		{
			if(0==StringFromGUID2(guid, newData,40))
			{
				*newData=NULL;
			}
		}
	}
	~_ATL_REGMAP_ENTRYKeeper()
	{
		delete [] (LPOLESTR)szData;
	}
};

#define BEGIN_REGMAP(x) static struct _ATL_REGMAP_ENTRY *_GetRegistryMap() { static const _ATL_REGMAP_ENTRYKeeper map[] = {

#define REGMAP_ENTRY(x,y) _ATL_REGMAP_ENTRYKeeper(OLESTR(##x),y),

#define REGMAP_UUID(x,clsid) _ATL_REGMAP_ENTRYKeeper(OLESTR(##x),clsid),

#define END_REGMAP() _ATL_REGMAP_ENTRYKeeper() }; return (_ATL_REGMAP_ENTRY *)map;}

#define DECLARE_REGISTRY_RESOURCEID_EX(x) static HRESULT WINAPI UpdateRegistry(BOOL bRegister) throw()\
	{\
		__if_exists(_Module) \
		{ \
			return _Module.UpdateRegistryFromResource((UINT)x, bRegister, _GetRegistryMap() ); \
		} \
		__if_not_exists(_Module) \
		{ \
			return ATL::_pAtlModule->UpdateRegistryFromResource((UINT)x, bRegister, _GetRegistryMap() ); \
		} \
	}


#endif /* COMLIB_REGISTRYMAP_HPP */
