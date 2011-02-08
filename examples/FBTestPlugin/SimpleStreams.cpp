/**********************************************************\
Original Author: Matthias (nitrogenycs)

Created:    Feb 28, 2010
License:    Dual license model; choose one of two:
            New BSD License
            http://www.opensource.org/licenses/bsd-license.php
            - or -
            GNU Lesser General Public License, version 2.1
            http://www.gnu.org/licenses/lgpl-2.1.html

Copyright 2010 PacketPass Inc, Georg Fritzsche,
               Firebreath development team
\**********************************************************/

#include "SimpleStreams.h"
#include "BrowserStream.h"
#include "BrowserHost.h"
#include "DefaultBrowserStreamHandler.h"

using namespace FB;

StreamsTest::StreamsTest( BrowserHostPtr Host ) : host(Host)
{
}


// examples of stream handlers
class MyStreamHandler1 : public FB::DefaultBrowserStreamHandler
{
public:
    virtual bool onStreamDataArrived(FB::StreamDataArrivedEvent *evt, FB::BrowserStream *)
    {
        // see evt->data, evt->length, evt->progress, evt->dataPosition
        return true;
    }
};

class MyStreamHandler2 : public FB::DefaultBrowserStreamHandler
{
public:
    MyStreamHandler2( BrowserHostPtr Host ) : host(Host)
    {}

    virtual bool onStreamOpened(FB::StreamOpenedEvent *evt, FB::BrowserStream *)
    {
        // see also these properties, accessible through evt->stream->{property}:
        // std::string      url;
        // bool             seekable;
        // bool             cached;
        // size_t           internalBufferSize;
        // std::wstring     cacheFilename;
        // size_t           length;
        // std::string      mimeType;
        // bool             opened;
        // bool             completed;
        // std::string      headers;

        // since this handler handles a seeking stream, request two ranges
        host->ScheduleAsyncCall( performRead, this );
        return true;
    }

    static void performRead( void* data )
    {
        MyStreamHandler2* handler = static_cast<MyStreamHandler2*>( data );
        if ( handler->stream ) handler->stream->readRange( 100, 100 + 15 );
    }

    virtual bool onStreamDataArrived(FB::StreamDataArrivedEvent *evt, FB::BrowserStream *)
    {
        size_t end = evt->getDataPosition() + evt->getLength();
        FB_UNUSED_VARIABLE(end);
        return true;
    }

    BrowserHostPtr host;
};


class MyStreamHandler3 : public FB::DefaultBrowserStreamHandler
{
public:
};


bool StreamsTest::run()
{
    // create a sequential, cached stream (the default if you omit the parameters)
    FB::PluginEventSinkPtr streamHandler1 = boost::make_shared<MyStreamHandler1>();
    FB::BrowserStreamPtr testStream1 = host->createStream( "http://colonelpanic.net/", streamHandler1, true, false );
    FB_UNUSED_VARIABLE(testStream1);

    // create a seekable, non-cached stream
    FB::PluginEventSinkPtr streamHandler2 = boost::make_shared<MyStreamHandler2>( host );
    FB::BrowserStreamPtr testStream2 = host->createStream( "http://upload.wikimedia.org/wikipedia/commons/thumb/6/63/Wikipedia-logo.png/100px-Wikipedia-logo.png", streamHandler2, false, true );
    FB_UNUSED_VARIABLE(testStream2);
 
    // try to access a page which does not exists (e.g. 404 or dns fails)
    FB::PluginEventSinkPtr streamHandler3 = boost::make_shared<MyStreamHandler3>();
    FB::BrowserStreamPtr testStream3 = host->createStream( "http://www.idontexist.invalid/index.html", streamHandler3, true, false );
    FB_UNUSED_VARIABLE(testStream3);

    return true;
}



