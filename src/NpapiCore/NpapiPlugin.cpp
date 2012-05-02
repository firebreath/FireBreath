/**********************************************************\
Original Author: Richard Bateman (taxilian)

Created:    Oct 19, 2009
License:    Dual license model; choose one of two:
            New BSD License
            http://www.opensource.org/licenses/bsd-license.php
            - or -
            GNU Lesser General Public License, version 2.1
            http://www.gnu.org/licenses/lgpl-2.1.html

Copyright 2009 Richard Bateman, Firebreath development team
\**********************************************************/

#include "NpapiStream.h"
#include "FactoryBase.h"
#include "PluginCore.h"
#include "PluginInfo.h"
#include "BrowserHost.h"
#include "precompiled_headers.h" // On windows, everything above this line in PCH

#include "NpapiPlugin.h"
using namespace FB::Npapi;

NpapiPlugin::NpapiPlugin(const NpapiBrowserHostPtr& host, const std::string& mimetype)
    : FB::BrowserPlugin(mimetype),
      m_obj(NULL),
      m_npHost(host),
      m_retainReturnedNPObject(true),
      m_isReady(false),
      m_mimetype(mimetype),
      m_pluginName(getFactoryInstance()->getPluginName(mimetype)),
      m_pluginDesc(getFactoryInstance()->getPluginDescription(mimetype))
{
    pluginMain->SetHost(host);
}

NpapiPlugin::~NpapiPlugin(void)
{
    if (m_obj != NULL) {
        m_npHost->ReleaseObject(m_obj);
    }
}

bool NpapiPlugin::setReady()
{
    bool rval = false;
    if (!m_isReady)
        rval = m_isReady = pluginMain->setReady();
    return rval;
}

NPObject *NpapiPlugin::getScriptableObject()
{
    if (m_obj == NULL) {
        m_obj = NPJavascriptObject::NewObject(m_npHost, pluginMain->getRootJSAPI());
    }
    /* Bugfix from Facebook: Certain older versions of webkit do a retain when
     * you return an NPObject from NPP_GetValue instead of assuming that we do
     * it before returninglike the NPAPI spec instructs; this results in a memory
     * leak if we don't fix it.
     */
    if (m_retainReturnedNPObject)
        m_npHost->RetainObject(m_obj);

    return m_obj;
}

void NpapiPlugin::shutdown(void)
{
    pluginMain->ClearWindow();
    pluginMain->shutdown();
}

void NpapiPlugin::init(NPMIMEType pluginType, int16_t argc, char* argn[], char *argv[])
{
    FB::VariantMap paramList;
    for (int16_t i = 0; i < argc; i++) {
        if (argv[i] != NULL) {
            paramList[argn[i]] = std::string(argv[i]);
        }
    }
    pluginMain->setParams(paramList);
    if(!FB::pluginGuiEnabled() || pluginMain->isWindowless()) {
        /* Windowless plugins require negotiation with the browser.
        * If the plugin does not set this value it is assumed to be
        * a windowed plugin.
        * See: https://developer.mozilla.org/en/Gecko_Plugin_API_Reference/Drawing_and_Event_Handling
        */
#ifndef XP_MACOSX
        // We don't want to set these if we're in Mac OS X, otherwise Firefox 3.5 will deliver us a
        // null window->window in NPP_SetWindow().
        m_npHost->SetValue(NPPVpluginWindowBool, (void*)false);
        m_npHost->SetValue(NPPVpluginTransparentBool, (void*)true); // Set transparency to true
#endif
    }
    setReady();
}

NPError NpapiPlugin::SetWindow(NPWindow* window)
{
    return NPERR_NO_ERROR;
}

/*
NPP_GetValue retrieves instance variables, among them whether a plug-in is windowed or windowless.

You can use this method as an optional entry point that the browser can call to determine the
plug-in name and description. It returns the requested values, specified by the variable and
value parameters, to the plug-in.

- This also is responsible for returning the core NPObject for the plugin
*/
NPError NpapiPlugin::GetValue(NPPVariable variable, void *value)
{

    switch (variable) {
    case NPPVpluginNameString:
        *((const char **)value) = m_pluginName.c_str();
        FBLOG_INFO("PluginCore", "GetValue(NPPVpluginNameString)");
        break;
    case NPPVpluginDescriptionString:
        *((const char **)value) = m_pluginDesc.c_str();
        FBLOG_INFO("PluginCore", "GetValue(NPPVpluginDescriptionString)");
        break;
    case NPPVpluginScriptableNPObject:
        *(NPObject **)value = getScriptableObject();
        FBLOG_INFO("PluginCore", "GetValue(NPPVpluginScriptableNPObject)");
        break;
    default:
        FBLOG_INFO("PluginCore", "GetValue(Unknown)");
        return NPERR_GENERIC_ERROR;
    }
    return NPERR_NO_ERROR;
}

/*
This call is used to inform plugins of variable information controlled by the browser.

I don't know of any time that this is actually used, but I'm sure there are uses =]
*/
NPError NpapiPlugin::SetValue(NPNVariable variable, void *value)
{
    return NPERR_NO_ERROR;
}

/*
The browser calls NPP_WriteReady before each call to NPP_Write to determine whether a plug-in can
receive data and how many bytes it can receive. This function allows the browser to send only as
much data to the instance as it can handle at one time, making resource use more efficient for
both the browser and plug-in.

The NPP_Write function may pass a larger buffer, but the plug-in is required to consume only the
amount of data returned by NPP_WriteReady.

The browser can write a smaller amount of data if desired or necessary; for example, if only 8K of
data is available in a network buffer. If the plug-in is allocating memory for the entire stream
at once (an AS_FILE stream), it can return a very large number. Because it is not processing
streaming data, the browser can pass as much data to the instance as necessary in a single
NPP_Write.

If the plug-in receives a value of zero, the data flow temporarily stops. The browser checks to
see if the plug-in can receive data again by resending the data at regular intervals.
*/
int32_t NpapiPlugin::WriteReady(NPStream* stream)
{
    NpapiStream* s = static_cast<NpapiStream*>( stream->notifyData );
    // check for streams we did not request or create
    if ( !s ) return -1;

    return s->getInternalBufferSize();
}

/*
The browser calls the NPP_Write function to deliver the data specified in a previous
NPP_WriteReady call to the plug-in. A plug-in must consume at least as many bytes as indicated in
the NPP_WriteReady call.

After a stream is created by a call to NPP_NewStream, the browser calls NPP_Write either:

    * If the plug-in requested a normal-mode stream, the data in the stream is delivered to the
      plug-in instance in a series of calls to NPP_WriteReady and NPP_Write.
    * If the plug-in requested a seekable stream, the NPN_RequestRead function requests reads of
      a specified byte range that results in a series of calls to NPP_WriteReady and NPP_Write.

The plug-in can use the offset parameter to track the bytes that are written. This gives you
different information depending in the type of stream. In a normal-mode stream., the parameter
value increases as the each buffer is written. The buf parameter is not persistent, so the plug-in
must process data immediately or allocate memory and save a copy of it. In a seekable stream with
byte range requests, you can use this parameter to track NPN_RequestRead requests.
*/
int32_t NpapiPlugin::Write(NPStream* stream, int32_t offset, int32_t len, void* buffer)
{
    NpapiStream* s = static_cast<NpapiStream*>( stream->notifyData );
    // check for streams we did not request or create
    if ( !s ) return -1;

    return s->signalDataArrived( buffer, len, offset );
}

/*
When the stream is complete, the browser calls NPP_StreamAsFile to provide the instance with a
full path name for a local file for the stream. NPP_StreamAsFile is called for streams whose mode
is set to NP_ASFILEONLY or NP_ASFILE only in a previous call to NPP_NewStream.

If an error occurs while retrieving the data or writing the file, the file name (fname) is null.
*/
void NpapiPlugin::StreamAsFile(NPStream* stream, const char* fname)
{
    NpapiStream* s = static_cast<NpapiStream*>( stream->notifyData );
    // check for streams we did not request or create
    if ( !s ) return;

    std::string cacheFilename( fname );
    s->signalCacheFilename( std::wstring( cacheFilename.begin(), cacheFilename.end() ) );
}


/*
The browser calls NPP_URLNotify after the completion of a NPN_GetURLNotify or NPN_PostURLNotify
request to inform the plug-in that the request was completed and supply a reason code for the
completion.

The most common reason code is NPRES_DONE, indicating simply that the request completed normally.
Other possible reason codes are NPRES_USER_BREAK, indicating that the request was halted due to a
user action (for example, clicking the Stop button), and NPRES_NETWORK_ERR, indicating that the
request could not be completed, perhaps because the URL could not be found.

The parameter notifyData is the plug-in-private value passed as an argument by a previous
NPN_GetURLNotify or NPN_PostURLNotify call, and can be used as an identifier for the request.
*/
void NpapiPlugin::URLNotify(const char* url, NPReason reason, void* notifyData)
{
    NpapiStream* s = static_cast<NpapiStream*>( notifyData );
    // check for streams we did not request or create
    if ( !s ) return;

    s->signalCompleted( reason == NPRES_DONE );
    s->setNotified();
}

/*
NPP_Print is called when the user requests printing for a web page that contains a visible plug-in
(either embedded or full-page). It uses the print mode set in the NPPrint structure in its
printInfo parameter to determine whether the plug-in should print as an embedded plug-in or as a
full-page plug-in.

    * An embedded plug-in shares printing with the browser; the plug-in prints the part of the
      page it occupies, and the browser handles everything else, including displaying print dialog
      boxes, getting the printer device context, and any other tasks involved in printing, as well
      as printing the rest of the page. For an embedded plug-in, set the printInfo field to
      NPEmbedPrint.
    * A full-page plug-in handles all aspects of printing itself. For a full-page plug-in, set the
      printInfo field to NPFullPrint or null.

For information about printing on your platform, see your platform documentation.
MS Windows

On MS Windows printInfo->print.embedPrint.platformPrint is the device context (DC) handle. Be sure
to cast this to type HDC.

The coordinates for the window rectangle are in TWIPS format. This means that you need to convert
the x-y coordinates using the Windows API call DPtoLP when you output text.
*/
void NpapiPlugin::Print(NPPrint* platformPrint)
{
}

/*
The browser calls NPP_HandleEvent to tell the plug-in when events take place in the plug-in's
window or drawable area. The plug-in either handles or ignores the event, depending on the value
given in the event parameter of this function. For a list of event types the application is
responsible for delivering to the plug-in, see the NPEvent structure.

MS Windows
----------
The browser gives each windowed plug-in its own native window, often a child window of the browser
window, to draw into. The plug-in has complete control over drawing and event handling within that
window.

Mac OS
------
The browser does not give a windowed plug-in a native window, because the Mac OS platform does not
support child windows. Instead, the windowed plug-in draws into the graphics port associated with
the the browser window, at the offset that the browser specifies. For this reason, NPP_HandleEvent
is only way the plug-in can receive events from its host application on Mac OS. When
NPP_HandleEvent is called, the current port is set up so that its origin matches the top-left
corner of the plug-in. A plug-in does not need to set up the current port for mouse coordinate
translation.
*/
int16_t NpapiPlugin::HandleEvent(void* event)
{
    return 0;   // 0, false, indicates that the event was not handled
}


/*
see https://developer.mozilla.org/en/NPP_NewStream

NPP_NewStream notifies the plug-in when a new stream is created. The NPStream* pointer is valid
until the stream is destroyed. The plug-in can store plug-in-private data associated with the
stream in stream->pdata. The MIME type of the stream is provided by the type parameter.

The data in the stream can be the file specified in the SRC attribute of the EMBED tag, for an
embedded instance, or the file itself, for a full-page instance. A plug-in can also request a
stream with the function NPN_GetURL. The browser calls NPP_DestroyStream when the stream completes
(either successfully or abnormally). The plug-in can terminate the stream itself by calling
NPN_DestroyStream.
*/
NPError NpapiPlugin::NewStream(NPMIMEType type, NPStream* stream, NPBool seekable, uint16_t* stype)
{
    NpapiStream* s = static_cast<NpapiStream*>( stream->notifyData );
    // check for streams we did not request or create
    if ( !s ) return NPERR_NO_DATA;

    s->setMimeType( type );
    s->setStream( stream );
    s->setLength( stream->end );
    s->setUrl( stream->url );
    if( stream->headers ) s->setHeaders( stream->headers );
    s->setSeekableByServer( seekable ? true : false );

    if ( s->isSeekableRequested() && !s->isSeekableByServer() )   // requested seekable stream, but stream was not seekable
    {                                                             //  stream can only be made seekable by downloading the entire file
                                                                  //  which we don't want to happen automatically.
        s->signalFailedOpen();
        // If unsuccessful, the function should return one of the NPError Error Codes.
        // This will cause the browser to destroy the stream without calling NPP_DestroyStream.
        s->setStream( 0 );
        return NPERR_STREAM_NOT_SEEKABLE;
    }

    if ( s->isSeekable() ) *stype = NP_SEEK;
    else if ( s->isCached() ) *stype = NP_ASFILE;
    else *stype = NP_NORMAL;

    // first npp_newstream has to finish before the user may perform a RequestRead in the opened handler
    if ( s->isSeekable() ) signalStreamOpened( s ); //m_npHost->ScheduleAsyncCall( signalStreamOpened, s );
    else signalStreamOpened( s );

    return NPERR_NO_ERROR;
}

void NpapiPlugin::signalStreamOpened(FB::BrowserStream* stream)
{
    NpapiStream* s = dynamic_cast<NpapiStream*>(stream);
    if ( s != NULL && !s->isCompleted() ) s->signalOpened();
}


/*
The browser calls the NPP_DestroyStream function when a data stream sent to the plug-in is
finished, either because it has completed successfully or terminated abnormally. After this, the
browser deletes the NPStream object.

You should delete any private data allocated in stream->pdata at this time, and should not make
any further references to the stream object.
*/
NPError NpapiPlugin::DestroyStream(NPStream* stream, NPReason reason)
{
    NpapiStream* s = static_cast<NpapiStream*>( stream->notifyData );
    // check for streams we did not request or create
    if ( !s || !s->getStream() ) return NPERR_NO_ERROR;

    s->setStream( 0 );
    stream->notifyData = 0;

    if ( !s->isCompleted() ) s->signalCompleted( reason == NPRES_DONE );
    s->setDestroyed();

    return NPERR_NO_ERROR;
}


