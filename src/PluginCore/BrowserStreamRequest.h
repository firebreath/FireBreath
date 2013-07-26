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

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// @class  BrowserStreamRequest
    ///
    /// @brief  Information about an HTTP request to be made
    ///
    /// Use this class to make HTTP requests through the browser. Create the stream and then
    /// call BrowserHost::createStream
    ///
    /// @see BrowserHost::createStream
    /// @since 1.7
    ////////////////////////////////////////////////////////////////////////////////////////////////////
    class BrowserStreamRequest
    {
    public:
        ////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @fn FB::BrowserStreamRequest::BrowserStreamRequest(const std::string& uri, const std::string method /*= "GET"*/, const bool accept /*= true*/);
        ///
        /// @brief  Create a BrowserStreamRequest from a string containing the URI to call
        ///
        /// @param uri  const std::string &       The string containing the URI to request
        /// @param method  const std::string  Defaults to "GET", can be "GET" or "POST"
        /// @param accept  const bool         Used internally; can be safely omitted
        ///
        /// @author taxilian
        /// @since 1.7
        ////////////////////////////////////////////////////////////////////////////////////////////////////
        BrowserStreamRequest(const std::string& uri, const std::string method = "GET", const bool accept = true)
            : uri(uri), method(method), lastModified(0), seekable(false),
            internalBufferSize(k_DEFAULT_REQUEST_BUFFER), cache(false), accepted(accept)
        {

        }

        ////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @fn    FB::BrowserStreamRequest::BrowserStreamRequest(const FB::URI& uri, const std::string method /*= "GET"*/, const bool accept /*= true*/);
        ///
        /// @brief  Create a BrowserStreamRequest from a FB:URI object
        ///
        /// @param uri  const FB::URI &   FB::URI object containing the URI to be used for the request
        /// @param method  const std::string 
        /// @param accept  const bool 
        ///
        /// @author taxilian
        /// @since 1.7
        ////////////////////////////////////////////////////////////////////////////////////////////////////
        BrowserStreamRequest(const FB::URI& uri, const std::string method = "GET", const bool accept = true)
            : uri(uri), method(method), lastModified(0), seekable(false),
            internalBufferSize(k_DEFAULT_REQUEST_BUFFER), cache(false), accepted(accept)
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
        ////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @fn   void FB::BrowserStreamRequest::setSeekable(bool s);
        ///
        /// @brief  Call with true to request that the stream be seekable; default is false
        ///
        /// @param s  bool to indicate if it should be seekable
        ///
        /// @author taxilian
        ////////////////////////////////////////////////////////////////////////////////////////////////////
        void setSeekable(bool s) { seekable = s; }
        ////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @fn   void FB::BrowserStreamRequest::setCacheable(bool c);
        ///
        /// @brief  Call with true to indicate that the browser's cache may be used for this request;
        ///         default is false.  Currently only honored on ActiveX (IE)
        ///
        /// @param c  bool to indicate if cache should be used
        ///
        /// @author taxilian
        ////////////////////////////////////////////////////////////////////////////////////////////////////
        void setCacheable(bool c) { cache = c; }
        ////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @fn   bool FB::BrowserStreamRequest::wasAccepted();
        ///
        /// @brief  Returns true if the request was accepted; used internally. User-created
        ///         requests are accepted by default
        ///
        /// @author taxilian
        ////////////////////////////////////////////////////////////////////////////////////////////////////
        bool wasAccepted() const { return accepted; }
        ////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @fn   void FB::BrowserStreamRequest::addQueryData(const std::string& key, const std::string& val);
        ///
        /// @brief  Add a GET querystring parameter (... &key=val ...)
        ///
        /// @param key  const std::string &  Key to add
        /// @param val  const std::string &  Value to add
        ///
        /// @author taxilian
        ////////////////////////////////////////////////////////////////////////////////////////////////////
        void addQueryData(const std::string& key, const std::string& val) {
            uri.addQueryData(key, val);
        }
        ////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @fn   void FB::BrowserStreamRequest::setHeaders(const std::string& headers);
        ///
        /// @brief  Used internally to set the headers on this object. Should not be called normally
        ///
        /// @param headers  const std::string & 
        ///
        /// @author taxilian
        ////////////////////////////////////////////////////////////////////////////////////////////////////
        void setHeaders(const std::string& headers) {
            this->headers = SimpleStreamHelper::parse_http_headers(headers);
        }
        ////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @fn   void FB::BrowserStreamRequest::setLastModified(uint32_t ts);
        ///
        /// @brief  Used internally to set the lastmodified timestamp on the object
        ///
        /// @param ts  uint32_t 
        ///
        /// @author taxilian
        ////////////////////////////////////////////////////////////////////////////////////////////////////
        void setLastModified(uint32_t ts) { lastModified = ts; }
        ////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @fn   void FB::BrowserStreamRequest::setBufferSize(size_t size);
        ///
        /// @brief  Call this to indicate the preferred internal buffer size of the BrowserStream object
        ///
        /// @param size  size_t preferred size of the internal buffer
        ///
        /// @author taxilian
        ////////////////////////////////////////////////////////////////////////////////////////////////////
        void setBufferSize(size_t size) { internalBufferSize = size; }

        ////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @fn   void FB::BrowserStreamRequest::setEventSink(const PluginEventSinkPtr& ptr);
        ///
        /// @brief  To use a PluginEventSink such as DefaultBrowserStreamHandler or a derivative
        ///         call this method to specify the object that should be attached to the stream
        ///         as an observer.
        ///
        /// This method cannot be used with setCallback.  One or the other must be called or the request
        /// will be ignored. If you need to receive data as it comes from the browser or a really large
        /// file, use this method. If you just want to be notified when the download completes and
        /// receive a byte buffer with the output data, use setCallback.
        ///
        /// @param ptr  const PluginEventSinkPtr & observer for the stream object
        ///
        /// @author taxilian
        ////////////////////////////////////////////////////////////////////////////////////////////////////
        void setEventSink(const PluginEventSinkPtr& ptr) { sinkPtr = ptr; accepted = true; }
        ////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @fn   FB::PluginEventSinkPtr FB::BrowserStreamRequest::getEventSink();
        ///
        /// @brief  Returns the PluginEventSink assigned to be the observer for the BrowserStream,
        ///         or a NULL BrowserStreamPtr if none
        ///
        /// @author taxilian
        ////////////////////////////////////////////////////////////////////////////////////////////////////
        PluginEventSinkPtr getEventSink() const { return sinkPtr; }

        ////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @fn   void FB::BrowserStreamRequest::setCallback(const HttpCallback& cb);
        ///
        /// @brief  Call this method to provide a callback function that should be called when the
        ///         request completes.  
        ///
        /// This method cannot be used with setEventSink.  One or the other must be called or the request
        /// will be ignored. If you need to receive data as it comes from the browser or a really large
        /// file, use this method. If you just want to be notified when the download completes and
        /// receive a byte buffer with the output data, use setCallback.
        ///
        /// In the case of an unsolicited stream, either this method or setEventSink *must* be called
        /// in order to accept the stream.
        ///
        /// @param cb  const HttpCallback & 
        ///
        /// @author taxilian
        ////////////////////////////////////////////////////////////////////////////////////////////////////
        void setCallback( const HttpCallback& cb ) { callback = cb; accepted = true; }
        ////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @fn   FB::HttpCallback FB::BrowserStreamRequest::getCallback();
        ///
        /// @brief  Returns the HttpCallback functor assigned to the object, or a NULL HttpCallback
        ///         if none
        ///
        /// @author taxilian
        ////////////////////////////////////////////////////////////////////////////////////////////////////
        HttpCallback getCallback() const { return callback; }

        ////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @fn   void FB::BrowserStreamRequest::setPostData(const std::string& data, const std::string& post_headers /*= ""*/);
        ///
        /// @brief  Sets POST data for the request; only used if the type of the request is POST
        ///
        /// @param data  const std::string &  POST data to use. Should be in querystring format (var1=val&var2=val)
        /// @param post_headers  const std::string & HTTP POST headers to send; use only if you know what you're doing. Ignored on ActiveX
        ///
        /// @author taxilian
        ////////////////////////////////////////////////////////////////////////////////////////////////////
        void setPostData(const std::string& data, const std::string& post_headers = "") { postdata = data; postheaders = post_headers; }
        ////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @fn   std::string FB::BrowserStreamRequest::getPostData();
        ///
        /// @brief  Returns POST data for the object, if any
        ///
        /// @author taxilian
        ////////////////////////////////////////////////////////////////////////////////////////////////////
        std::string getPostData() const { return postdata; }
        ////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @fn   std::string FB::BrowserStreamRequest::getPostHeaders();
        ///
        /// @brief  Returns POST headers for the object, if any.
        ///
        /// @author taxilian
        ////////////////////////////////////////////////////////////////////////////////////////////////////
        std::string getPostHeaders() const { return postheaders; }
    };

};

#endif // BrowserStreamRequest_h__