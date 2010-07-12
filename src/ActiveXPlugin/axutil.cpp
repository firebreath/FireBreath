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
#if _MSC_VER < 1500
  : m_hkey(0)
#endif
{
#if _MSC_VER > 1400
    AtlSetPerUserRegistration(perUser);
#else // _MSC_VER < 1500
    // this seems to be always active and therefore may break
    // any COM functionality thats not registered per user

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