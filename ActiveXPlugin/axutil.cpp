
#include "axutil.h"
#include "atlstr.h"

FbPerUserRegistration::FbPerUserRegistration(bool perUser) 
  : m_hkey(0)
{
#if MSC_VER > 8
	AtlSetPerUserRegistration(perUser);
#else // MSC_VER < 9
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
#if MSC_VER < 9
	if(m_hkey)
		::RegCloseKey(m_hkey);
#endif
}