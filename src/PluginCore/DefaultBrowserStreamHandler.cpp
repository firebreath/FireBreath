/**********************************************************\ 
Original Author: Matthias (nitrogenycs)

Created:    Feb 28, 2010
License:    Dual license model; choose one of two:
            New BSD License
            http://www.opensource.org/licenses/bsd-license.php
            - or -
            GNU Lesser General Public License, version 2.1
            http://www.gnu.org/licenses/lgpl-2.1.html

Copyright 2010 PacketPass, Inc and the Firebreath development team
\**********************************************************/

#include "precompiled_headers.h" // On windows, everything above this line in PCH
#include "DefaultBrowserStreamHandler.h"

using namespace FB;

// default stream handler
DefaultBrowserStreamHandler::DefaultBrowserStreamHandler()
{
}

DefaultBrowserStreamHandler::~DefaultBrowserStreamHandler()
{
}

bool DefaultBrowserStreamHandler::onStreamCreated(FB::StreamCreatedEvent *evt, FB::BrowserStream* Stream)
{
    assert(Stream != NULL);
    setStream( ptr_cast<BrowserStream>(Stream->shared_from_this()) );
    return false;
}

bool DefaultBrowserStreamHandler::onStreamDestroyed(FB::StreamDestroyedEvent *evt, FB::BrowserStream *)
{
    clearStream();
    return false;
}

bool DefaultBrowserStreamHandler::onStreamDataArrived(FB::StreamDataArrivedEvent *evt, FB::BrowserStream *)
{
    // process data
    return false;
}

bool DefaultBrowserStreamHandler::onStreamFailedOpen(FB::StreamFailedOpenEvent *evt, FB::BrowserStream *)
{
    return false;
}

bool DefaultBrowserStreamHandler::onStreamOpened(FB::StreamOpenedEvent *evt, FB::BrowserStream *)
{
    return false;
}

bool DefaultBrowserStreamHandler::onStreamCompleted(FB::StreamCompletedEvent *evt, FB::BrowserStream *)
{
     return false;
}

const FB::BrowserStreamPtr& DefaultBrowserStreamHandler::getStream() const
{
     return stream;
}

void DefaultBrowserStreamHandler::setStream(const FB::BrowserStreamPtr& Stream)
{
    if ( this->stream && Stream )
    {
        // attempt to overwrite the stream, strange
        assert(false);
    }
    this->stream = Stream;
}

void DefaultBrowserStreamHandler::clearStream() 
{
    stream.reset();
}

