/**********************************************************\ 
Original Author: Georg Fritzsche

Created:    Sept 17, 2009
License:    Dual license model; choose one of two:
            New BSD License
            http://www.opensource.org/licenses/bsd-license.php
            - or -
            GNU Lesser General Public License, version 2.1
            http://www.gnu.org/licenses/lgpl-2.1.html

Copyright 2009 Richard Bateman, Firebreath development team
\**********************************************************/

#include "axutil.h"
#include "atlstr.h"

FbPerUserRegistration::FbPerUserRegistration(bool perUser) 
#if _ATL_VER < 0x0900
  : m_hkey(0)
#endif
{
#if _ATL_VER < 0x0900
    // this seems to be always active and therefore may break
    // any COM functionality thats not registered per user

    HRESULT hr;
    HKEY key;

    if(!perUser && m_hkey) {
        ::RegCloseKey(m_hkey);
    } else if (!perUser) {
        return;
    }

    hr = ::RegOpenKeyW(HKEY_CURRENT_USER, L"Software\\Classes", &key);
    if(FAILED(hr))
        return;

    hr = ::RegOverridePredefKey(HKEY_CLASSES_ROOT, key);
    if(FAILED(hr)) {
        ::RegCloseKey(key);
        key = 0;
    }

    m_hkey = key;
#else // _ATL_VER >= 0900
    AtlSetPerUserRegistration(perUser);
#endif
}

FbPerUserRegistration::~FbPerUserRegistration()
{
#if _ATL_VER < 0x0900
    if(m_hkey)
        ::RegCloseKey(m_hkey);
#endif
}