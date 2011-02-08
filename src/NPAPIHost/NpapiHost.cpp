/**********************************************************\ 
Original Author: Richard Bateman (taxilian)

Created:    Oct 15, 2009
License:    Dual license model; choose one of two:
            New BSD License
            http://www.opensource.org/licenses/bsd-license.php
            - or -
            GNU Lesser General Public License, version 2.1
            http://www.gnu.org/licenses/lgpl-2.1.html

Copyright 2009 Richard Bateman, Firebreath development team
\**********************************************************/


#include "NpapiHost.h"

using namespace FB::Npapi;

FB::TypeIDMap<NPIdentifier> NpapiHost::m_idMapper((NPIdentifier)100);

NpapiHost::NpapiHost(NPInitFuncPtr initPtr, NPShutdownFuncPtr shutdownPtr, NPGetEntryPointsFuncPtr getepPtr)
    : init(initPtr), shutdown(shutdownPtr), getEntryPoints(getepPtr)
{
    memset(&m_funcs, 0, sizeof(NPNetscapeFuncs));
    memset(&m_instance, 0, sizeof(NPP_t));

    m_funcs.size = sizeof(NPNetscapeFuncs);
    m_funcs.version = 1;    // TODO: Set this to the firefox 3 version value
    m_funcs.geturl = &NpapiHost::NH_GetURL;
    m_funcs.posturl = &NpapiHost::NH_PostURL;
    m_funcs.requestread = &NpapiHost::NH_RequestRead;
    m_funcs.newstream = &NpapiHost::NH_NewStream;
    m_funcs.write = &NpapiHost::NH_Write;
    m_funcs.destroystream = &NpapiHost::NH_DestroyStream;
    m_funcs.status = &NpapiHost::NH_Status;
    m_funcs.uagent = &NpapiHost::NH_UserAgent;
    m_funcs.memalloc = &NpapiHost::NH_MemAlloc;
    m_funcs.memfree = &NpapiHost::NH_MemFree;
    m_funcs.memflush = &NpapiHost::NH_MemFlush;
    m_funcs.reloadplugins = &NpapiHost::NH_ReloadPlugins;
    m_funcs.geturlnotify = &NpapiHost::NH_GetURLNotify;
    m_funcs.posturlnotify = &NpapiHost::NH_PostURLNotify;
    m_funcs.getvalue = &NpapiHost::NH_GetValue;
    m_funcs.setvalue = &NpapiHost::NH_SetValue;
    m_funcs.invalidaterect = &NpapiHost::NH_InvalidateRect;
    m_funcs.invalidateregion = &NpapiHost::NH_InvalidateRegion;
    m_funcs.forceredraw = &NpapiHost::NH_ForceRedraw;
    m_funcs.getstringidentifier = &NpapiHost::NH_GetStringIdentifier;
    m_funcs.getstringidentifiers = &NpapiHost::NH_GetStringIdentifiers;
    m_funcs.getintidentifier = &NpapiHost::NH_GetIntIdentifier;
    m_funcs.identifierisstring = &NpapiHost::NH_IdentifierIsString;
    m_funcs.utf8fromidentifier = &NpapiHost::NH_UTF8FromIdentifier;
    m_funcs.intfromidentifier = &NpapiHost::NH_IntFromIdentifier;
    m_funcs.createobject = &NpapiHost::NH_CreateObject;
    m_funcs.retainobject = &NpapiHost::NH_RetainObject;
    m_funcs.releaseobject = &NpapiHost::NH_ReleaseObject;
    m_funcs.invoke = &NpapiHost::NH_Invoke;
    m_funcs.invokeDefault = &NpapiHost::NH_InvokeDefault;
    m_funcs.evaluate = &NpapiHost::NH_Evaluate;
    m_funcs.getproperty = &NpapiHost::NH_GetProperty;
    m_funcs.setproperty = &NpapiHost::NH_SetProperty;
    m_funcs.removeproperty = &NpapiHost::NH_RemoveProperty;
    m_funcs.hasproperty = &NpapiHost::NH_HasProperty;
    m_funcs.hasmethod = &NpapiHost::NH_HasMethod;
    m_funcs.releasevariantvalue = &NpapiHost::NH_ReleaseVariantValue;
    m_funcs.setexception = &NpapiHost::NH_SetException;
    m_funcs.pushpopupsenabledstate = &NpapiHost::NH_PushPopupsEnabledState;
    m_funcs.poppopupsenabledstate = &NpapiHost::NH_PopPopupsEnabledState;
    m_funcs.enumerate = &NpapiHost::NH_Enumerate;
    m_funcs.pluginthreadasynccall = &NpapiHost::NH_PluginThreadAsyncCall;
    m_funcs.construct = &NpapiHost::NH_Construct;
    m_funcs.scheduletimer = &NpapiHost::NH_ScheduleTimer;
    m_funcs.unscheduletimer = &NpapiHost::NH_UnscheduleTimer;

    m_instance.ndata = this;
}

NpapiHost::~NpapiHost()
{
}

NPNetscapeFuncs *NpapiHost::getBrowserFuncs()
{
    return &m_funcs;
}

NPP NpapiHost::getPluginInstance()
{
    return &m_instance;
}

