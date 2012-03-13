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

#define DISPID_EVENTS 0x80010000 + 6000

class AxIdMapInit
{
public:
    AxIdMapInit()
    {
        // As of IE9 some events seem to have predefined DISPIDs; let's try to define those here:
        AxIdMap.setIdForValue((DISPID_EVENTS + 28), "onabort");
        AxIdMap.setIdForValue((DISPID_EVENTS + 87), "onactivate");
        AxIdMap.setIdForValue((DISPID_EVENTS + 67), "onafterprint");
        AxIdMap.setIdForValue((DISPID_EVENTS + 22), "onafterupdate");
        AxIdMap.setIdForValue((DISPID_EVENTS + 104), "onalert");
        AxIdMap.setIdForValue((DISPID_EVENTS + 70), "onattachevent");
        AxIdMap.setIdForValue((DISPID_EVENTS + 90), "onbeforeactivate");
        AxIdMap.setIdForValue((DISPID_EVENTS + 59), "onbeforecopy");
        AxIdMap.setIdForValue((DISPID_EVENTS + 58), "onbeforecut");
        AxIdMap.setIdForValue((DISPID_EVENTS + 77), "onbeforedeactivate");
        AxIdMap.setIdForValue((DISPID_EVENTS + 23), "onbeforedragover");
        AxIdMap.setIdForValue((DISPID_EVENTS + 24), "onbeforedroporpaste");
        AxIdMap.setIdForValue((DISPID_EVENTS + 69), "onbeforeeditfocus");
        AxIdMap.setIdForValue((DISPID_EVENTS + 60), "onbeforepaste");
        AxIdMap.setIdForValue((DISPID_EVENTS + 66), "onbeforeprint");
        AxIdMap.setIdForValue((DISPID_EVENTS + 39), "onbeforeunload");
        AxIdMap.setIdForValue((DISPID_EVENTS + 21), "onbeforeupdate");
        AxIdMap.setIdForValue((DISPID_EVENTS + 15), "onblur");
        AxIdMap.setIdForValue((DISPID_EVENTS + 20), "onbounce");
        AxIdMap.setIdForValue((DISPID_EVENTS + 64), "oncellchange");
        AxIdMap.setIdForValue((DISPID_EVENTS + 30), "onchange");
        AxIdMap.setIdForValue((DISPID_EVENTS + 45), "onchangeblur");
        AxIdMap.setIdForValue((DISPID_EVENTS + 44), "onchangefocus");
        AxIdMap.setIdForValue((DISPID_EVENTS +  8), "onclick");
        AxIdMap.setIdForValue((DISPID_EVENTS + 72), "oncontentready");
        AxIdMap.setIdForValue((DISPID_EVENTS + 65), "oncontextmenu");
        AxIdMap.setIdForValue((DISPID_EVENTS + 79), "oncontrolselect");
        AxIdMap.setIdForValue((DISPID_EVENTS + 56), "oncopy");
        AxIdMap.setIdForValue((DISPID_EVENTS + 55), "oncut");
        AxIdMap.setIdForValue((DISPID_EVENTS + 41), "ondataavailable");
        AxIdMap.setIdForValue((DISPID_EVENTS + 40), "ondatasetchanged");
        AxIdMap.setIdForValue((DISPID_EVENTS + 42), "ondatasetcomplete");
        AxIdMap.setIdForValue((DISPID_EVENTS +  9), "ondblclick");
        AxIdMap.setIdForValue((DISPID_EVENTS + 88), "ondeactivate");
        AxIdMap.setIdForValue((DISPID_EVENTS + 111), "ondommutation");
        AxIdMap.setIdForValue((DISPID_EVENTS + 49), "ondrag");
        AxIdMap.setIdForValue((DISPID_EVENTS + 50), "ondragend");
        AxIdMap.setIdForValue((DISPID_EVENTS + 51), "ondragenter");
        AxIdMap.setIdForValue((DISPID_EVENTS + 53), "ondragleave");
        AxIdMap.setIdForValue((DISPID_EVENTS + 52), "ondragover");
        AxIdMap.setIdForValue((DISPID_EVENTS + 35), "ondragstart");
        AxIdMap.setIdForValue((DISPID_EVENTS + 54), "ondrop");
        AxIdMap.setIdForValue((DISPID_EVENTS + 29), "onerror");
        AxIdMap.setIdForValue((DISPID_EVENTS + 38), "onerrorupdate");
        AxIdMap.setIdForValue((DISPID_EVENTS + 43), "onfilter");
        AxIdMap.setIdForValue((DISPID_EVENTS + 26), "onfinish");
        AxIdMap.setIdForValue((DISPID_EVENTS + 14), "onfocus");
        AxIdMap.setIdForValue((DISPID_EVENTS + 91), "onfocusin");
        AxIdMap.setIdForValue((DISPID_EVENTS + 92), "onfocusout");
        AxIdMap.setIdForValue((DISPID_EVENTS + 109), "onhashchange");
        AxIdMap.setIdForValue((DISPID_EVENTS + 13), "onhelp");
        AxIdMap.setIdForValue((DISPID_EVENTS + 103), "onhide");
        AxIdMap.setIdForValue((DISPID_EVENTS +  5), "onkeydown");
        AxIdMap.setIdForValue((DISPID_EVENTS +  7), "onkeypress");
        AxIdMap.setIdForValue((DISPID_EVENTS +  6), "onkeyup");
        AxIdMap.setIdForValue((DISPID_EVENTS + 34), "onlayout");
        AxIdMap.setIdForValue((DISPID_EVENTS + 73), "onlayoutcomplete");
        AxIdMap.setIdForValue((DISPID_EVENTS + 75), "onlinkedoverflow");
        AxIdMap.setIdForValue((DISPID_EVENTS + 32), "onload");
        AxIdMap.setIdForValue((DISPID_EVENTS + 46), "onlosecapture");
        AxIdMap.setIdForValue((DISPID_EVENTS + 110), "onmessage");
        AxIdMap.setIdForValue((DISPID_EVENTS +  2), "onmousedown");
        AxIdMap.setIdForValue((DISPID_EVENTS + 85), "onmouseenter");
        AxIdMap.setIdForValue((DISPID_EVENTS + 71), "onmousehover");
        AxIdMap.setIdForValue((DISPID_EVENTS + 86), "onmouseleave");
        AxIdMap.setIdForValue((DISPID_EVENTS +  4), "onmousemove");
        AxIdMap.setIdForValue((DISPID_EVENTS +  1), "onmouseout");
        AxIdMap.setIdForValue((DISPID_EVENTS +  0), "onmouseover");
        AxIdMap.setIdForValue((DISPID_EVENTS +  3), "onmouseup");
        AxIdMap.setIdForValue((DISPID_EVENTS + 76), "onmousewheel");
        AxIdMap.setIdForValue((DISPID_EVENTS + 78), "onmove");
        AxIdMap.setIdForValue((DISPID_EVENTS + 82), "onmoveend");
        AxIdMap.setIdForValue((DISPID_EVENTS + 81), "onmovestart");
        AxIdMap.setIdForValue((DISPID_EVENTS + 89), "onmultilayoutcleanup");
        AxIdMap.setIdForValue((DISPID_EVENTS + 99), "onobjectcontentscrolled");
        AxIdMap.setIdForValue((DISPID_EVENTS + 108), "onoffline");
        AxIdMap.setIdForValue((DISPID_EVENTS + 107), "ononline");
        AxIdMap.setIdForValue((DISPID_EVENTS + 74), "onpage");
        AxIdMap.setIdForValue((DISPID_EVENTS + 57), "onpaste");
        AxIdMap.setIdForValue((DISPID_EVENTS + 61), "onpersistload");
        AxIdMap.setIdForValue((DISPID_EVENTS + 48), "onpersistsave");
        AxIdMap.setIdForValue((DISPID_EVENTS + 106), "onpopupmenuend");
        AxIdMap.setIdForValue((DISPID_EVENTS + 105), "onpopupmenustart");
        AxIdMap.setIdForValue((DISPID_EVENTS + 47), "onpropertychange");
        AxIdMap.setIdForValue((DISPID_EVENTS + 25), "onreadystatechange");
        AxIdMap.setIdForValue((DISPID_EVENTS + 12), "onreset");
        AxIdMap.setIdForValue((DISPID_EVENTS + 36), "onresize");
        AxIdMap.setIdForValue((DISPID_EVENTS + 84), "onresizeend");
        AxIdMap.setIdForValue((DISPID_EVENTS + 83), "onresizestart");
        AxIdMap.setIdForValue((DISPID_EVENTS + 19), "onrowenter");
        AxIdMap.setIdForValue((DISPID_EVENTS + 18), "onrowexit");
        AxIdMap.setIdForValue((DISPID_EVENTS + 62), "onrowsdelete");
        AxIdMap.setIdForValue((DISPID_EVENTS + 63), "onrowsinserted");
        AxIdMap.setIdForValue((DISPID_EVENTS + 31), "onscroll");
        AxIdMap.setIdForValue((DISPID_EVENTS + 10), "onselect");
        AxIdMap.setIdForValue((DISPID_EVENTS + 94), "onselectadd");
        AxIdMap.setIdForValue((DISPID_EVENTS + 80), "onselectionchange");
        AxIdMap.setIdForValue((DISPID_EVENTS + 95), "onselectremove");
        AxIdMap.setIdForValue((DISPID_EVENTS + 37), "onselectstart");
        AxIdMap.setIdForValue((DISPID_EVENTS + 96), "onselectwithin");
        AxIdMap.setIdForValue((DISPID_EVENTS + 102), "onshow");
        AxIdMap.setIdForValue((DISPID_EVENTS + 27), "onstart");
        AxIdMap.setIdForValue((DISPID_EVENTS + 68), "onstop");
        AxIdMap.setIdForValue((DISPID_EVENTS + 100), "onstorage");
        AxIdMap.setIdForValue((DISPID_EVENTS + 101), "onstoragecommit");
        AxIdMap.setIdForValue((DISPID_EVENTS + 11), "onsubmit");
        AxIdMap.setIdForValue((DISPID_EVENTS + 98), "onsystemscrollingend");
        AxIdMap.setIdForValue((DISPID_EVENTS + 97), "onsystemscrollingstart");
        AxIdMap.setIdForValue((DISPID_EVENTS + 33), "onunload");
        AxIdMap.setIdForValue((DISPID_EVENTS + 93), "onvaluechange");


        AxIdMap.setIdForValue((DISPID_EVENTS + 112), "ontimeout");
        AxIdMap.setIdForValue((DISPID_EVENTS + 113), "onwheel");
        AxIdMap.setIdForValue((DISPID_EVENTS + 114), "onsvgload");
        AxIdMap.setIdForValue((DISPID_EVENTS + 115), "onsvgunload");
        AxIdMap.setIdForValue((DISPID_EVENTS + 116), "onsvgabort");
        AxIdMap.setIdForValue((DISPID_EVENTS + 117), "onsvgerror");
        AxIdMap.setIdForValue((DISPID_EVENTS + 118), "onsvgresize");
        AxIdMap.setIdForValue((DISPID_EVENTS + 119), "onsvgscroll");
        AxIdMap.setIdForValue((DISPID_EVENTS + 120), "onsvgzoom");
        AxIdMap.setIdForValue((DISPID_EVENTS + 121), "ononmsthumbnailclick");
        AxIdMap.setIdForValue((DISPID_EVENTS + 122), "oncompositionstart");
        AxIdMap.setIdForValue((DISPID_EVENTS + 123), "oncompositionupdate");
        AxIdMap.setIdForValue((DISPID_EVENTS + 124), "oncompositionend");
        AxIdMap.setIdForValue((DISPID_EVENTS + 125), "ondomattrmodified");
        AxIdMap.setIdForValue((DISPID_EVENTS + 126), "ondomcontentloaded");
        AxIdMap.setIdForValue((DISPID_EVENTS + 127), "oninput");
        AxIdMap.setIdForValue((DISPID_EVENTS + 128), "ondomchardatamodified");
        AxIdMap.setIdForValue((DISPID_EVENTS + 129), "ontextinput");
        AxIdMap.setIdForValue((DISPID_EVENTS + 130), "ononmssitemodejumplistitemremoved");
        AxIdMap.setIdForValue((DISPID_EVENTS + 131), "ondomnodeinserted");
        AxIdMap.setIdForValue((DISPID_EVENTS + 132), "ondomnoderemoved");
        AxIdMap.setIdForValue((DISPID_EVENTS + 133), "ondomsubtreemodified");
        AxIdMap.setIdForValue((DISPID_EVENTS + 134), "oncanplay");
        AxIdMap.setIdForValue((DISPID_EVENTS + 135), "oncanplaythrough");
        AxIdMap.setIdForValue((DISPID_EVENTS + 136), "ondurationchange");
        AxIdMap.setIdForValue((DISPID_EVENTS + 137), "onemptied");
        AxIdMap.setIdForValue((DISPID_EVENTS + 138), "onended");
        AxIdMap.setIdForValue((DISPID_EVENTS + 139), "onloadeddata");
        AxIdMap.setIdForValue((DISPID_EVENTS + 140), "onloadedmetadata");
        AxIdMap.setIdForValue((DISPID_EVENTS + 141), "onloadstart");
        AxIdMap.setIdForValue((DISPID_EVENTS + 142), "onpause");
        AxIdMap.setIdForValue((DISPID_EVENTS + 143), "onplay");
        AxIdMap.setIdForValue((DISPID_EVENTS + 144), "onplaying");
        AxIdMap.setIdForValue((DISPID_EVENTS + 145), "onprogress");
        AxIdMap.setIdForValue((DISPID_EVENTS + 146), "onratechange");
        AxIdMap.setIdForValue((DISPID_EVENTS + 147), "onseeked");
        AxIdMap.setIdForValue((DISPID_EVENTS + 148), "onseeking");
        AxIdMap.setIdForValue((DISPID_EVENTS + 149), "onstalled");
        AxIdMap.setIdForValue((DISPID_EVENTS + 150), "onsuspend");
        AxIdMap.setIdForValue((DISPID_EVENTS + 151), "ontimeupdate");
        AxIdMap.setIdForValue((DISPID_EVENTS + 152), "onvolumechange");
        AxIdMap.setIdForValue((DISPID_EVENTS + 153), "onwaiting");

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

#undef DISPID_EVENTS

FB::TypeIDMap<DISPID> FB::ActiveX::AxIdMap(100);
AxIdMapInit __AxIdInitClassDummy;

