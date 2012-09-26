/**********************************************************\
Original Author: Richard Bateman (taxilian)

Created:    Sep 25, 2012
License:    Dual license model; choose one of two:
            New BSD License
            http://www.opensource.org/licenses/bsd-license.php
            - or -
            GNU Lesser General Public License, version 2.1
            http://www.gnu.org/licenses/lgpl-2.1.html

Copyright 2009 Richard Bateman, Firebreath development team
\**********************************************************/
#pragma once
#ifndef BrowserStreamRequest_h__
#define BrowserStreamRequest_h__

#include "SimpleStreamHelper.h"
#include <string>

#define k_DEFAULT_REQUEST_BUFFER 128 * 1024

namespace FB {

    FB_FORWARD_PTR(PluginEventSink);

    class BrowserStreamRequest
    {
    public:
        BrowserStreamRequest(const std::string& uri, const std::string method = "GET", const bool accept = true)
            : uri(uri), method(method), accepted(accept), lastModified(0), seekable(false),
            internalBufferSize(k_DEFAULT_REQUEST_BUFFER), cache(false)
        {

        }

        BrowserStreamRequest(const FB::URI& uri, const std::string method = "GET", const bool accept = true)
            : uri(uri), method(method), accepted(accept), lastModified(0), seekable(false),
            internalBufferSize(k_DEFAULT_REQUEST_BUFFER), cache(false)
        {

        }

        FB::URI uri;
        std::string method;
        FB::HeaderMap headers;
        uint32_t lastModified;
        bool seekable;
        size_t internalBufferSize;
        bool cache;

    private:
        PluginEventSinkPtr sinkPtr;
        HttpCallback callback;
        bool accepted;
        std::string postdata;
        std::string postheaders;

    public: // Helper methods
        void accept() { accepted = true; }
        void setSeekable(bool s) { seekable = s; }
        void setCacheable(bool c) { cache = c; }
        bool wasAccepted() const { return accepted; }
        void addQueryData(const std::string& key, const std::string& val) {
            uri.addQueryData(key, val);
        }
        void setHeaders(const std::string& headers) {
            this->headers = SimpleStreamHelper::parse_http_headers(headers);
        }
        void setLastModified(uint32_t ts) { lastModified = ts; }
        void setBufferSize(size_t size) { internalBufferSize = size; }

        void setEventSink(const PluginEventSinkPtr& ptr) { sinkPtr = ptr; this->accept(); }
        PluginEventSinkPtr getEventSink() const { return sinkPtr; }

        void setCallback( const HttpCallback& cb ) { callback = cb; }
        HttpCallback getCallback() const { return callback; }

        void setPostData(const std::string& data, const std::string& post_headers = "") { postdata = data; postheaders = post_headers; }
        std::string getPostData() const { return postdata; }
        std::string getPostHeaders() const { return postheaders; }
    };

};

#endif // BrowserStreamRequest_h__