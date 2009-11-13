
#include "axutil.h"
#include "atlstr.h"

FbPerUserRegistration::FbPerUserRegistration(bool perUser) 
#if _MSC_VER < 1500
  : m_hkey(0)
#endif
{
#if _MSC_VER > 1400
	AtlSetPerUserRegistration(perUser);
#else // _MSC_VER < 1500
	HRESULT hr;
	HKEY key;

	if(!perUser)
		return;

	hr = ::RegOpenKeyW(HKEY_CURRENT_USER, L"Software\\Classes", &key);
	if(FAILED(hr))
		return;

	hr = ::RegOverridePredefKey(HKEY_CLASSES_ROOT, key);
	if(FAILED(hr)) {
		::RegCloseKey(key);
		key = 0;
	}

	m_hkey = key;
#endif
}

FbPerUserRegistration::~FbPerUserRegistration()
{
#if _MSC_VER < 1500
	if(m_hkey)
		::RegCloseKey(m_hkey);
#endif
}