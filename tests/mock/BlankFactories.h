#pragma once
#ifndef BlankFactories_h__
#define BlankFactories_h__

#ifdef FB_WIN
#include "COMJavascriptObject.h"
#define FB_LIBID LIBID_FireBreathWinLib
EXTERN_C const IID LIBID_FireBreathWinLib;
EXTERN_C const CLSID CLSID_FBComJavascriptObject;
EXTERN_C const IID DIID_IFBComEventSource;
MIDL_INTERFACE("a7f89003-620a-56e0-aeea-ec0e8da18fb3")
IFBComJavascriptObject : public IDispatchEx
{
public:
};

typedef FB::ActiveX::COMJavascriptObject<&CLSID_FBComJavascriptObject, IFBComJavascriptObject, &DIID_IFBComEventSource, &FB_LIBID> COMJSObject;
#endif
#endif // BlankFactories_h__

