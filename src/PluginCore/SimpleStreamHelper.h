#pragma once
#ifndef SimpleStreamHelper_h__
#define SimpleStreamHelper_h__

/**********************************************************\
Original Author: Richard Bateman

Created:    Jan 24, 2011
License:    Dual license model; choose one of two:
            New BSD License
            http://www.opensource.org/licenses/bsd-license.php
            - or -
            GNU Lesser General Public License, version 2.1
            http://www.gnu.org/licenses/lgpl-2.1.html

Copyright 2011 Richard Bateman, 
               Firebreath development team
\**********************************************************/

#include "DefaultBrowserStreamHandler.h"
#include "PluginEvents/StreamEvents.h"
#include "BrowserStream.h"
#include "URI.h"
#include <boost/shared_array.hpp>
#include <string>
#include <list>
#include <map>

namespace FB {
    FB_FORWARD_PTR(BrowserHost);
    FB_FORWARD_PTR(SimpleStreamHelper);

    typedef std::multimap<std::string, std::string> HeaderMap;
    typedef boost::function<void (bool, const FB::HeaderMap&, const boost::shared_array<uint8_t>&, const size_t)> HttpCallback;
    class SimpleStreamHelper : public FB::DefaultBrowserStreamHandler
    {
    public:
        static FB::SimpleStreamHelperPtr AsyncGet(const FB::BrowserHostPtr& host, const FB::URI& uri, const HttpCallback& callback,
            const bool cache = true, const size_t bufferSize = 128*1024);

    public:
        SimpleStreamHelper( const BrowserHostPtr& host, const HttpCallback& callback, const size_t blockSize = 128*1024 );

        virtual bool onStreamDataArrived(FB::StreamDataArrivedEvent *evt, FB::BrowserStream *);
        virtual bool onStreamOpened(FB::StreamOpenedEvent *evt, FB::BrowserStream *);
        virtual bool onStreamCompleted(FB::StreamCompletedEvent *evt, FB::BrowserStream *);
        static FB::HeaderMap parse_http_headers(const std::string& headers );

    protected:
        typedef std::list<boost::shared_array<uint8_t> > BlockList;
        BrowserHostPtr host;
        BlockList blocks;
        boost::shared_array<uint8_t> data;
        const size_t blockSize;
        size_t received;
        HttpCallback callback;

    private:
        void keepReference(const SimpleStreamHelperPtr& ptr);
        SimpleStreamHelperPtr self;
    };
};
#endif // SimpleStreamHelper_h__