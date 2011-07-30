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

#include "precompiled_headers.h" // On windows, everything above this line in PCH
#include "axutil.h"
#include "atlstr.h"

using namespace FB;
using namespace FB::ActiveX;

FbPerUserRegistration::FbPerUserRegistration(bool perUser) 
#if _ATL_VER < 0x0900
  : m_mapping(false)
#endif
{
#if _ATL_VER < 0x0900
    // this seems to be always active and therefore may break
    // any COM functionality thats not registered per user

    if (!perUser) {
        return;
    }

    HKEY key;

    LONG err = ::RegOpenKeyEx(HKEY_CURRENT_USER, TEXT("Software\\Classes"), 0, MAXIMUM_ALLOWED, &key);
    if(err == ERROR_SUCCESS) {
        err = ::RegOverridePredefKey(HKEY_CLASSES_ROOT, key);
        ::RegCloseKey(key);
    }

    if (err == ERROR_SUCCESS) {
        EnablePerUserTLibRegistration();
        m_mapping = true;
    }
#else // _ATL_VER >= 0900
    AtlSetPerUserRegistration(perUser);
#endif
}

#if _ATL_VER < 0x0900
void FbPerUserRegistration::EnablePerUserTLibRegistration()
{
    HMODULE hOleaut32 = ::GetModuleHandle(TEXT("Oleaut32.dll"));
    assert(hOleaut32);

    typedef void (WINAPI * EnablePerUserTLibRegistrationProcPtr) (void);
    EnablePerUserTLibRegistrationProcPtr enablePerUserTLibRegistrationProcPtr =
        reinterpret_cast<EnablePerUserTLibRegistrationProcPtr>(
        GetProcAddress(hOleaut32, "OaEnablePerUserTLibRegistration"));
    if (enablePerUserTLibRegistrationProcPtr) {
        enablePerUserTLibRegistrationProcPtr();
    }
}

#endif

FB::ActiveX::FbPerUserRegistration::~FbPerUserRegistration()
{
#if _ATL_VER < 0x0900
    if (m_mapping)
        ::RegOverridePredefKey(HKEY_CLASSES_ROOT, NULL);
#endif
}

using FB::ActiveX::AxIdMap;

#define DISPID_NORMAL_FIRST                     1000

class AxIdMapInit
{
public:
    AxIdMapInit()
    {
        // As of IE9 some events seem to have predefined DISPIDs; let's try to define those here:

        AxIdMap.setIdForValue(5, "onmousedown");
        AxIdMap.setIdForValue(6, "onmousemove");
        AxIdMap.setIdForValue(7, "onmouseup");
        AxIdMap.setIdForValue(8, "onmouseout");
        AxIdMap.setIdForValue(9, "onmouseover");
        AxIdMap.setIdForValue(10, "onclick");
        AxIdMap.setIdForValue(11, "ondblclick");
        AxIdMap.setIdForValue(12, "onkeydown");
        AxIdMap.setIdForValue(13, "onkeypress");
        AxIdMap.setIdForValue(14, "onkeyup");
        AxIdMap.setIdForValue(15, "onblur");
        AxIdMap.setIdForValue(16, "onreadystatechange");

        AxIdMap.setIdForValue(DISPID_NORMAL_FIRST, "onabort");
        AxIdMap.setIdForValue(DISPID_NORMAL_FIRST + 1, "onchange");
        AxIdMap.setIdForValue(DISPID_NORMAL_FIRST + 2, "onerror");
        AxIdMap.setIdForValue(DISPID_NORMAL_FIRST + 3, "onload");
        AxIdMap.setIdForValue(DISPID_NORMAL_FIRST + 6, "onselect");
        AxIdMap.setIdForValue(DISPID_NORMAL_FIRST + 7, "onsubmit");
        AxIdMap.setIdForValue(DISPID_NORMAL_FIRST + 8, "onunload");
        AxIdMap.setIdForValue(DISPID_NORMAL_FIRST + 9, "onbounce");
        AxIdMap.setIdForValue(DISPID_NORMAL_FIRST + 10, "onfinish");
        AxIdMap.setIdForValue(DISPID_NORMAL_FIRST + 11, "onstart");
        AxIdMap.setIdForValue(DISPID_NORMAL_FIRST + 13, "onlayout");
        AxIdMap.setIdForValue(DISPID_NORMAL_FIRST + 14, "onscroll");
        AxIdMap.setIdForValue(DISPID_NORMAL_FIRST + 15, "onreset");
        AxIdMap.setIdForValue(DISPID_NORMAL_FIRST + 16, "onresize");
        AxIdMap.setIdForValue(DISPID_NORMAL_FIRST + 17, "onbeforeunload");
        AxIdMap.setIdForValue(DISPID_NORMAL_FIRST + 18, "onchangefocus");
        AxIdMap.setIdForValue(DISPID_NORMAL_FIRST + 19, "onchangeblur");
        AxIdMap.setIdForValue(DISPID_NORMAL_FIRST + 20, "onpersist");
        AxIdMap.setIdForValue(DISPID_NORMAL_FIRST + 21, "onpersistsave");
        AxIdMap.setIdForValue(DISPID_NORMAL_FIRST + 22, "onpersistload");
        AxIdMap.setIdForValue(DISPID_NORMAL_FIRST + 23, "oncontextmenu");
        AxIdMap.setIdForValue(DISPID_NORMAL_FIRST + 24, "onbeforeprint");
        AxIdMap.setIdForValue(DISPID_NORMAL_FIRST + 25, "onafterprint");
        AxIdMap.setIdForValue(DISPID_NORMAL_FIRST + 26, "onstop");
        AxIdMap.setIdForValue(DISPID_NORMAL_FIRST + 27, "onbeforeeditfocus");
        AxIdMap.setIdForValue(DISPID_NORMAL_FIRST + 28, "onmousehover");
        AxIdMap.setIdForValue(DISPID_NORMAL_FIRST + 29, "oncontentready");
        AxIdMap.setIdForValue(DISPID_NORMAL_FIRST + 30, "onlayoutcomplete");
        AxIdMap.setIdForValue(DISPID_NORMAL_FIRST + 31, "onpage");
        AxIdMap.setIdForValue(DISPID_NORMAL_FIRST + 32, "onlinkedoverflow");
        AxIdMap.setIdForValue(DISPID_NORMAL_FIRST + 33, "onmousewheel");
        AxIdMap.setIdForValue(DISPID_NORMAL_FIRST + 34, "onbeforedeactivate");
        AxIdMap.setIdForValue(DISPID_NORMAL_FIRST + 35, "onmove");
        AxIdMap.setIdForValue(DISPID_NORMAL_FIRST + 36, "oncontrolselect");
        AxIdMap.setIdForValue(DISPID_NORMAL_FIRST + 37, "onselectionchange");
        AxIdMap.setIdForValue(DISPID_NORMAL_FIRST + 38, "onmovestart");
        AxIdMap.setIdForValue(DISPID_NORMAL_FIRST + 39, "onmoveend");
        AxIdMap.setIdForValue(DISPID_NORMAL_FIRST + 40, "onresizestart");
        AxIdMap.setIdForValue(DISPID_NORMAL_FIRST + 41, "onresizeend");
        AxIdMap.setIdForValue(DISPID_NORMAL_FIRST + 42, "onmouseenter");
        AxIdMap.setIdForValue(DISPID_NORMAL_FIRST + 43, "onmouseleave");
        AxIdMap.setIdForValue(DISPID_NORMAL_FIRST + 44, "onactivate");
        AxIdMap.setIdForValue(DISPID_NORMAL_FIRST + 45, "ondeactivate");
        AxIdMap.setIdForValue(DISPID_NORMAL_FIRST + 46, "onmultilayoutcleanup");
        AxIdMap.setIdForValue(DISPID_NORMAL_FIRST + 47, "onbeforeactivate");
        AxIdMap.setIdForValue(DISPID_NORMAL_FIRST + 48, "onfocusin");
        AxIdMap.setIdForValue(DISPID_NORMAL_FIRST + 49, "onfocusout");
        AxIdMap.setIdForValue(DISPID_NORMAL_FIRST + 50, "onvaluechange");
        AxIdMap.setIdForValue(DISPID_NORMAL_FIRST + 51, "onselectadd");
        AxIdMap.setIdForValue(DISPID_NORMAL_FIRST + 52, "onselectremove");
        AxIdMap.setIdForValue(DISPID_NORMAL_FIRST + 53, "onselectwithin");
        AxIdMap.setIdForValue(DISPID_NORMAL_FIRST + 54, "onsystemscrollingstart");
        AxIdMap.setIdForValue(DISPID_NORMAL_FIRST + 55, "onsystemscrollingend");
        AxIdMap.setIdForValue(DISPID_NORMAL_FIRST + 56, "onobjectcontentscrolled");
        AxIdMap.setIdForValue(DISPID_NORMAL_FIRST + 57, "onstorage");
        AxIdMap.setIdForValue(DISPID_NORMAL_FIRST + 58, "onstoragecommit");
        AxIdMap.setIdForValue(DISPID_NORMAL_FIRST + 59, "onshow");
        AxIdMap.setIdForValue(DISPID_NORMAL_FIRST + 60, "onhide");
        AxIdMap.setIdForValue(DISPID_NORMAL_FIRST + 61, "onalert");
        AxIdMap.setIdForValue(DISPID_NORMAL_FIRST + 62, "onpopupmenustart");
        AxIdMap.setIdForValue(DISPID_NORMAL_FIRST + 63, "onpopupmenuend");
        AxIdMap.setIdForValue(DISPID_NORMAL_FIRST + 64, "ononline");
        AxIdMap.setIdForValue(DISPID_NORMAL_FIRST + 65, "onoffline");
        AxIdMap.setIdForValue(DISPID_NORMAL_FIRST + 66, "onhashchange");
        AxIdMap.setIdForValue(DISPID_NORMAL_FIRST + 67, "onmessage");
        AxIdMap.setIdForValue(DISPID_NORMAL_FIRST + 68, "ondommutation");

        AxIdMap.setIdForValue(0, "value");
        AxIdMap.setIdForValue(-1, "toString");
        AxIdMap.setIdForValue(-3, "propertyput");
        AxIdMap.setIdForValue(-4, "newenum");
        AxIdMap.setIdForValue(-5, "evaluate");
        AxIdMap.setIdForValue(-6, "constructor");
        AxIdMap.setIdForValue(-7, "destructor");
        AxIdMap.setIdForValue(-8, "collect");
        AxIdMap.setIdForValue(-500, "autosize");
        AxIdMap.setIdForValue(-501, "backcolor");
        AxIdMap.setIdForValue(-502, "backstyle");
        AxIdMap.setIdForValue(-503, "bordercolor");
        AxIdMap.setIdForValue(-504, "borderstyle");
        AxIdMap.setIdForValue(-505, "borderwidth");
        AxIdMap.setIdForValue(-507, "drawmode");
        AxIdMap.setIdForValue(-508, "drawstyle");
        AxIdMap.setIdForValue(-509, "drawwidth");
        AxIdMap.setIdForValue(-510, "fillcolor");
        AxIdMap.setIdForValue(-511, "fillstyle");
        AxIdMap.setIdForValue(-512, "font");
        AxIdMap.setIdForValue(-513, "forecolor");
        AxIdMap.setIdForValue(-514, "enabled");
        AxIdMap.setIdForValue(-515, "hwnd");
        AxIdMap.setIdForValue(-516, "tabstop");
        AxIdMap.setIdForValue(-517, "text");
        AxIdMap.setIdForValue(-518, "caption");
        AxIdMap.setIdForValue(-519, "bordervisible");
        AxIdMap.setIdForValue(-520, "appearance");
        AxIdMap.setIdForValue(-521, "mousepointer");
        AxIdMap.setIdForValue(-522, "mouseicon");
        AxIdMap.setIdForValue(-523, "picture");
        AxIdMap.setIdForValue(-524, "valid");
        AxIdMap.setIdForValue(-525, "readystate");
        AxIdMap.setIdForValue(-526, "listindex");
        AxIdMap.setIdForValue(-527, "selected");
        AxIdMap.setIdForValue(-528, "list");
        AxIdMap.setIdForValue(-529, "column");
        AxIdMap.setIdForValue(-531, "listcount");
        AxIdMap.setIdForValue(-532, "multiselect");
        AxIdMap.setIdForValue(-533, "maxlength");
        AxIdMap.setIdForValue(-534, "passwordchar");
        AxIdMap.setIdForValue(-535, "scrollbars");
        AxIdMap.setIdForValue(-536, "wordwrap");
        AxIdMap.setIdForValue(-537, "multiline");
        AxIdMap.setIdForValue(-538, "numberofrows");
        AxIdMap.setIdForValue(-539, "numberofcolumns");
        AxIdMap.setIdForValue(-540, "displaystyle");
        AxIdMap.setIdForValue(-541, "groupname");
        AxIdMap.setIdForValue(-542, "imemode");
        AxIdMap.setIdForValue(-543, "accelerator");
        AxIdMap.setIdForValue(-544, "enterkeybehavior");
        AxIdMap.setIdForValue(-545, "tabkeybehavior");
        AxIdMap.setIdForValue(-546, "seltext");
        AxIdMap.setIdForValue(-547, "selstart");
        AxIdMap.setIdForValue(-548, "sellength");

        AxIdMap.setIdForValue(-550, "refresh");
        AxIdMap.setIdForValue(-551, "doclick");
        AxIdMap.setIdForValue(-552, "aboutbox");
        AxIdMap.setIdForValue(-553, "additem");
        AxIdMap.setIdForValue(-554, "clear");
        AxIdMap.setIdForValue(-555, "removeitem");
        AxIdMap.setIdForValue(-600, "click");
        AxIdMap.setIdForValue(-601, "dblclick");
        AxIdMap.setIdForValue(-602, "keydown");
        AxIdMap.setIdForValue(-603, "keypress");
        AxIdMap.setIdForValue(-604, "keyup");
        AxIdMap.setIdForValue(-605, "mousedown");
        AxIdMap.setIdForValue(-606, "mousemove");
        AxIdMap.setIdForValue(-607, "mouseup");
        AxIdMap.setIdForValue(-608, "errorevent");
        AxIdMap.setIdForValue(-609, "readystatechange");
        AxIdMap.setIdForValue(-610, "click_value");
        AxIdMap.setIdForValue(-611, "righttoleft");
        AxIdMap.setIdForValue(-612, "toptobottom");
        AxIdMap.setIdForValue(-613, "this");
        AxIdMap.setIdForValue(-701, "ambient_backcolor");
        AxIdMap.setIdForValue(-702, "ambient_displayname");
        AxIdMap.setIdForValue(-703, "ambient_font");
        AxIdMap.setIdForValue(-704, "ambient_forecolor");
        AxIdMap.setIdForValue(-705, "ambient_localeid");
        AxIdMap.setIdForValue(-706, "ambient_messagereflect");
        AxIdMap.setIdForValue(-707, "ambient_scaleunits");
        AxIdMap.setIdForValue(-708, "ambient_textalign");
        AxIdMap.setIdForValue(-709, "ambient_usermode");
        AxIdMap.setIdForValue(-710, "ambient_uidead");
        AxIdMap.setIdForValue(-711, "ambient_showgrabhandles");
        AxIdMap.setIdForValue(-712, "ambient_showhatching");
        AxIdMap.setIdForValue(-713, "ambient_displayasdefault");
        AxIdMap.setIdForValue(-714, "ambient_supportsmnemonics");
        AxIdMap.setIdForValue(-715, "ambient_autoclip");
        AxIdMap.setIdForValue(-716, "ambient_appearance");
        AxIdMap.setIdForValue(-725, "ambient_codepage");
        AxIdMap.setIdForValue(-726, "ambient_palette");
        AxIdMap.setIdForValue(-727, "ambient_charsetidforvalue");
        AxIdMap.setIdForValue(-728, "ambient_transferpriority");
        AxIdMap.setIdForValue(-732, "ambient_righttoleft");
        AxIdMap.setIdForValue(-733, "ambient_toptobottom");
        AxIdMap.setIdForValue(-800, "name");
        AxIdMap.setIdForValue(-801, "delete");
        AxIdMap.setIdForValue(-802, "object");
        AxIdMap.setIdForValue(-803, "parent");
        AxIdMap.setIdForValue(-5500, "windowobject");
        AxIdMap.setIdForValue(-5506, "locationobject");
        AxIdMap.setIdForValue(-5507, "historyobject");
        AxIdMap.setIdForValue(-5508, "navigatorobject");
        AxIdMap.setIdForValue(-5511, "securityctx");
        AxIdMap.setIdForValue(-5512, "ambient_dlcontrol");
        AxIdMap.setIdForValue(-5513, "ambient_useragent");
        AxIdMap.setIdForValue(-5514, "securitydomain");
    }
};
#undef DISPID_NORMAL_FIRST

FB::TypeIDMap<DISPID> FB::ActiveX::AxIdMap(100);
AxIdMapInit __AxIdInitClassDummy;

