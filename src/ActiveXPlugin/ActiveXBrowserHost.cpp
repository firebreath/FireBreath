/**********************************************************\ 
Original Author: Richard Bateman (taxilian)

Created:    Oct 30, 2009
License:    Dual license model; choose one of two:
            Eclipse Public License - Version 1.0
            http://www.eclipse.org/legal/epl-v10.html
            - or -
            GNU Lesser General Public License, version 2.1
            http://www.gnu.org/licenses/lgpl-2.1.html
            
Copyright 2009 Richard Bateman, Firebreath development team
\**********************************************************/

#include "ActiveXBrowserHost.h"
#include "COMJavascriptObject.h"

#include "Win/PluginWindowWin.h"

ActiveXBrowserHost::ActiveXBrowserHost(IHTMLDocument2 *doc)
    : m_hWnd(NULL), m_htmlDoc(doc), m_htmlDocDisp(doc)
{
    if (m_htmlDoc.p != NULL) {
        m_htmlDoc->get_parentWindow(&m_htmlWin);
        m_htmlWinDisp = m_htmlWin;
    }
}

ActiveXBrowserHost::~ActiveXBrowserHost(void)
{
}

void ActiveXBrowserHost::ScheduleAsyncCall(void (*func)(void *), void *userData)
{
    if (m_hWnd != NULL) 
        ::PostMessage(m_hWnd, WM_ASYNCTHREADINVOKE, NULL, 
            (LPARAM)new FB::WINDOWS_ASYNC_EVENT(func, userData));
}

void *ActiveXBrowserHost::getContextID()
{
    return (void*)this;
}

void ActiveXBrowserHost::setWindow(HWND wnd)
{
    m_hWnd = wnd;
}

FB::AutoPtr<FB::BrowserObjectAPI> ActiveXBrowserHost::getDOMDocument()
{
    FB::AutoPtr<FB::BrowserObjectAPI> retObj(new IDispatchAPI(m_htmlDocDisp.p, this));
    return retObj;
}

FB::AutoPtr<FB::BrowserObjectAPI> ActiveXBrowserHost::getDOMWindow()
{
    FB::AutoPtr<FB::BrowserObjectAPI> retObj(new IDispatchAPI(m_htmlWin.p, this));
    return retObj;
}

FB::variant ActiveXBrowserHost::getVariant(const VARIANT *cVar)
{
    CComVariant converted;
    FB::variant retVal;

	switch(cVar->vt)
	{        
	case VT_R4:
	case VT_R8:
	case VT_DECIMAL:
        converted.ChangeType(VT_R8, cVar);
		retVal = (double)converted.dblVal;
		break;

	case VT_I1:
	case VT_I2:
	case VT_I4:
	case VT_UI1:
	case VT_UI2:
	case VT_UI4:
	case VT_I8:
	case VT_UI8:
	case VT_INT:
	case VT_UINT:
        converted.ChangeType(VT_I4, cVar);
        retVal = (long)converted.lVal;
		break;

	case VT_LPSTR:
	case VT_LPWSTR:
	case VT_BSTR:
	case VT_CLSID:
		{
            converted.ChangeType(VT_BSTR, cVar);
			CW2A cStr(converted.bstrVal);

            retVal = std::string(cStr);
		}
		break;

	case VT_DISPATCH:
        retVal = FB::AutoPtr<FB::BrowserObjectAPI>(new IDispatchAPI(cVar->pdispVal, this)); 
		break;

	case VT_ERROR:
	case VT_BOOL:
        converted.ChangeType(VT_BOOL, cVar);
		retVal = (converted.boolVal == VARIANT_TRUE) ? true : false;
		break;

	case VT_EMPTY:
	case VT_NULL:
    default:
        // retVal is already empty, leave it such
		break;
	}

    return retVal;
}

void ActiveXBrowserHost::getComVariant(VARIANT *dest, const FB::variant &var)
{
    CComVariant outVar;
    VariantInit(dest);
    if (var.empty()) {
        // Already empty
        return;
    } else if (var.get_type() == typeid(int)
        || var.get_type() == typeid(long)
        || var.get_type() == typeid(short)
        || var.get_type() == typeid(char)
        || var.get_type() == typeid(unsigned int)
        || var.get_type() == typeid(unsigned short)
        || var.get_type() == typeid(unsigned char)) {
        // Integer type
        outVar = var.convert_cast<long>();

    } else if (var.get_type() == typeid(double)
        || var.get_type() == typeid(float)) {
        outVar = var.convert_cast<double>();

    } else if (var.get_type() == typeid(bool)) {
        outVar = var.convert_cast<bool>();

    } else if (var.get_type() == typeid(std::string)) {
        std::string str = var.convert_cast<std::string>();
        CComBSTR bStr(str.c_str());
        outVar = bStr;
	} else if (var.get_type() == typeid(FB::AutoPtr<FB::BrowserObjectAPI>)) {
        FB::AutoPtr<IDispatchAPI> api = dynamic_cast<IDispatchAPI*>(var.convert_cast<FB::AutoPtr<FB::BrowserObjectAPI>>().ptr());
        if (api.ptr() != NULL) {
            outVar = api->getIDispatch();
        } else {
            outVar = COMJavascriptObject::NewObject(this, var.convert_cast<FB::AutoPtr<FB::BrowserObjectAPI>>().ptr());
        }
	} else if (var.get_type() == typeid(FB::AutoPtr<FB::JSAPI>)) {
        FB::AutoPtr<IDispatchAPI> api = dynamic_cast<IDispatchAPI*>(var.convert_cast<FB::AutoPtr<FB::JSAPI>>().ptr());
        if (api.ptr() != NULL) {
            outVar = api->getIDispatch();
        } else {
            outVar = COMJavascriptObject::NewObject(this, var.convert_cast<FB::AutoPtr<FB::JSAPI>>().ptr());
        }
    }

    outVar.Detach(dest);
}