/**********************************************************\ 
Original Author: Richard Bateman (taxilian)

Created:    Oct 16, 2009
License:    Dual license model; choose one of two:
            New BSD License
            http://www.opensource.org/licenses/bsd-license.php
            - or -
            GNU Lesser General Public License, version 2.1
            http://www.gnu.org/licenses/lgpl-2.1.html
1
Copyright 2009 Richard Bateman, Firebreath development team
\**********************************************************/

#include <cstring>
#include "NpapiTypes.h"
#include "NpapiPluginModule.h"
#include "precompiled_headers.h" // On windows, everything above this line in PCH

#ifdef min
#  undef min
#endif
#ifdef max
#  undef max
#endif

void FB::Npapi::copyNPBrowserFuncs(NPNetscapeFuncs *dstFuncs, NPNetscapeFuncs *srcFuncs, NPP instance)
{
    dstFuncs->size = srcFuncs->size;
    dstFuncs->version = srcFuncs->version;
    dstFuncs->geturl = srcFuncs->geturl;
    dstFuncs->posturl = srcFuncs->posturl;
    dstFuncs->requestread = srcFuncs->requestread;
    dstFuncs->newstream = srcFuncs->newstream;
    dstFuncs->write = srcFuncs->write;
    dstFuncs->destroystream = srcFuncs->destroystream;
    dstFuncs->status = srcFuncs->status;
    dstFuncs->uagent = srcFuncs->uagent;
    dstFuncs->memalloc = srcFuncs->memalloc;
    dstFuncs->memfree = srcFuncs->memfree;
    dstFuncs->memflush = srcFuncs->memflush;
    dstFuncs->reloadplugins = srcFuncs->reloadplugins;
    dstFuncs->geturlnotify = srcFuncs->geturlnotify;
    dstFuncs->posturlnotify = srcFuncs->posturlnotify;
    dstFuncs->getvalue = srcFuncs->getvalue;
    dstFuncs->setvalue = srcFuncs->setvalue;
    dstFuncs->invalidaterect = srcFuncs->invalidaterect;
    dstFuncs->invalidateregion = srcFuncs->invalidateregion;
    dstFuncs->forceredraw = srcFuncs->forceredraw;
    dstFuncs->getstringidentifier = srcFuncs->getstringidentifier;
    dstFuncs->getstringidentifiers = srcFuncs->getstringidentifiers;
    dstFuncs->getintidentifier = srcFuncs->getintidentifier;
    dstFuncs->identifierisstring = srcFuncs->identifierisstring;
    dstFuncs->utf8fromidentifier = srcFuncs->utf8fromidentifier;
    dstFuncs->intfromidentifier = srcFuncs->intfromidentifier;
    dstFuncs->createobject = srcFuncs->createobject;
    dstFuncs->retainobject = srcFuncs->retainobject;
    dstFuncs->releaseobject = srcFuncs->releaseobject;
    dstFuncs->invoke = srcFuncs->invoke;
    dstFuncs->invokeDefault = srcFuncs->invokeDefault;
    dstFuncs->evaluate = srcFuncs->evaluate;
    dstFuncs->getproperty = srcFuncs->getproperty;
    dstFuncs->setproperty = srcFuncs->setproperty;
    dstFuncs->removeproperty = srcFuncs->removeproperty;
    dstFuncs->hasproperty = srcFuncs->hasproperty;
    dstFuncs->hasmethod = srcFuncs->hasmethod;
    dstFuncs->releasevariantvalue = srcFuncs->releasevariantvalue;
    dstFuncs->setexception = srcFuncs->setexception;
    dstFuncs->construct = srcFuncs->construct;
    
    if (srcFuncs->version >= NPVERS_MACOSX_HAS_COCOA_EVENTS) { // 23
        dstFuncs->scheduletimer = srcFuncs->scheduletimer;
        dstFuncs->unscheduletimer = srcFuncs->unscheduletimer;
    }

    if(srcFuncs->version >= NPVERS_HAS_STREAMOUTPUT) { // 8
        // ?
    }
    if(srcFuncs->version >= NPVERS_HAS_NOTIFICATION) { // 9
        // ?
    }
    if(srcFuncs->version >= NPVERS_HAS_LIVECONNECT) { // 9
        // ?
    }
    if(srcFuncs->version >= NPVERS_68K_HAS_LIVECONNECT) { // 11
        // ?
    }
    if(srcFuncs->version >= NPVERS_HAS_WINDOWLESS) { // 11
        // ?
    }
    if(srcFuncs->version >= NPVERS_HAS_XPCONNECT_SCRIPTING) { // 13
        // ?
    }
    if(srcFuncs->version >= NPVERS_HAS_NPRUNTIME_SCRIPTING) { // 14
        // ?
    }
    if(srcFuncs->version >= NPVERS_HAS_FORM_VALUES) { // 15
        // ?
    }
    if(srcFuncs->version >= NPVERS_HAS_POPUPS_ENABLED_STATE) {
        dstFuncs->pushpopupsenabledstate = srcFuncs->pushpopupsenabledstate;
        dstFuncs->poppopupsenabledstate = srcFuncs->poppopupsenabledstate;
    }
    if(srcFuncs->version >= NPVERS_HAS_RESPONSE_HEADERS) { // 17
        // ?
    }
    if(srcFuncs->version >= NPVERS_HAS_NPOBJECT_ENUM) { // 18
        dstFuncs->enumerate = srcFuncs->enumerate;
    }
#ifdef XP_MACOSX
    dstFuncs->pluginthreadasynccall = &FB::Npapi::NpapiPluginModule::scheduleAsyncCallback;
#else
    if(srcFuncs->version >= NPVERS_HAS_PLUGIN_THREAD_ASYNC_CALL) { // 19
        dstFuncs->pluginthreadasynccall = srcFuncs->pluginthreadasynccall;
    }
#endif
    if(srcFuncs->version >= NPVERS_HAS_ALL_NETWORK_STREAMS) { // 20
        // ?
    }
    if(srcFuncs->version >= NPVERS_HAS_URL_AND_AUTH_INFO) { // 21
        dstFuncs->getvalueforurl = srcFuncs->getvalueforurl;
        dstFuncs->setvalueforurl = srcFuncs->setvalueforurl;
        dstFuncs->getauthenticationinfo = srcFuncs->getauthenticationinfo;
    }
}

