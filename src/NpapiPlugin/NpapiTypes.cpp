/**********************************************************\ 
Original Author: Richard Bateman (taxilian)

Created:    Oct 16, 2009
License:    Dual license model; choose one of two:
            Eclipse Public License - Version 1.0
            http://www.eclipse.org/legal/epl-v10.html
            - or -
            GNU Lesser General Public License, version 2.1
            http://www.gnu.org/licenses/lgpl-2.1.html

Copyright 2009 Richard Bateman, Firebreath development team
\**********************************************************/

#include "NpapiTypes.h"

void FB::Npapi::copyNPBrowserFuncs(NPNetscapeFuncs *dstFuncs, NPNetscapeFuncs *srcFuncs)
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
    dstFuncs->pushpopupsenabledstate = srcFuncs->pushpopupsenabledstate;
    dstFuncs->poppopupsenabledstate = srcFuncs->poppopupsenabledstate;
    dstFuncs->enumerate = srcFuncs->enumerate;
    dstFuncs->pluginthreadasynccall = srcFuncs->pluginthreadasynccall;
    dstFuncs->construct = srcFuncs->construct;
}
