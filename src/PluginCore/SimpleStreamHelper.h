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
#include <boost/tuple/tuple.hpp>
#include <string>
#include <list>
#include <map>

namespace FB {
    FB_FORWARD_PTR(BrowserHost);
    FB_FORWARD_PTR(SimpleStreamHelper);

    typedef std::multimap<std::string, std::string> HeaderMap;
    typedef boost::function<void (bool, const FB::HeaderMap&, const boost::shared_array<uint8_t>&, const size_t)> HttpCallback;
    
    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// @class  HttpStreamResponse
    ///
    /// @brief  Data structure to hold the response to an HTTP Get request
    ///
    /// This struct should always be used in a shared_ptr to avoid copying data
    ///
    /// @see SimpleStreamHelper
    /// @see SimpleStreamHelper::AsyncGet
    /// @see SimpleStreamHelper::SynchronousGet
    ////////////////////////////////////////////////////////////////////////////////////////////////////
    struct HttpStreamResponse : boost::noncopyable
    {
        HttpStreamResponse(bool success, const FB::HeaderMap& headers,
            const boost::shared_array<uint8_t>& data, const size_t size) : success(success),
            headers(headers), data(data), size(size) { }
        // True if the request succeeded
        bool success;
        // Multimap of headers
        const FB::HeaderMap headers;
        // shared_array containing the data returned by the server
        const boost::shared_array<uint8_t> data;
        // The size of data
        const size_t size;
    };
    typedef boost::shared_ptr<HttpStreamResponse> HttpStreamResponsePtr;

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// @class  SimpleStreamHelper
    ///
    /// @brief  Helper class for making simple HTTP requests
    ///
    /// This class can be used as a helper class to make HTTP requests easily using BrowserStreams
    ///
    /// @see FB::SimpleStreamHelper::AsyncGet
    /// @see FB::SimpleStreamHelper::SynchronousGet
    ////////////////////////////////////////////////////////////////////////////////////////////////////
    class SimpleStreamHelper : public FB::DefaultBrowserStreamHandler
    {
    public:
        ////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @fn public static FB::SimpleStreamHelperPtr FB::SimpleStreamHelper::AsyncGet(const FB::BrowserHostPtr& host, const FB::URI& uri, const HttpCallback& callback, const bool cache = true, const size_t bufferSize = 128*1024)
        ///
        /// @brief  Starts an asynchronous HTTP get request
        ///
        /// Call this static method to make an asyncronous HTTP get request through the browser.
        /// Note that you can hold onto the ptr to the request but you don't have to -- it will be
        /// cleaned up automatically when the request finishes.
        ///
        /// @code
        /// void FBTestPluginAPI::getURL(const std::string& url, const FB::JSObjectPtr& callback)
        /// {
        ///    FB::SimpleStreamHelper::AsyncGet(m_host, FB::URI::fromString(url),
        ///        boost::bind(&FBTestPluginAPI::getURLCallback, this, callback, _1, _2, _3, _4));
        /// }
        ///
        /// void FBTestPluginAPI::getURLCallback(const FB::JSObjectPtr& callback, bool success,
        ///    const FB::HeaderMap& headers, const boost::shared_array<uint8_t>& data, const size_t size) {
        ///    if (success) {
        ///        std::string dstr(reinterpret_cast<const char*>(data.get()), size);
        ///        // Do something here; dstr is a string containing the full body
        ///        // headers is a multimap with all the headers
        ///    } else {
        ///        // The request could not be completed
        ///    }
        /// }
        /// @endcode
        ///
        /// @param  host        const FB::BrowserHostPtr&   BrowserHostPtr to use to create the stream
        /// @param  uri         const FB::URI&              URI to request
        /// @param  callback    const HttpCallback &        Function to call with the response
        /// @param  cache       const bool                  true if cache can be used
        /// @param  bufferSize  const size_t                Specifies the size of the buffer to use internally
        /// @returns FB::SimpleStreamHelperPtr
        /// @since 1.4b3
        /// @see SynchronousGet
        /// @see FB::URI
        ////////////////////////////////////////////////////////////////////////////////////////////////////
        static FB::SimpleStreamHelperPtr AsyncGet(const FB::BrowserHostPtr& host, const FB::URI& uri, const HttpCallback& callback,
            bool cache = true, size_t bufferSize = 128*1024);

        
        ////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @fn public static FB::SimpleStreamHelperPtr FB::SimpleStreamHelper::AsyncPost(const FB::BrowserHostPtr& host, const FB::URI& uri, const std::string& postdata, const HttpCallback& callback, const bool cache = true, const size_t bufferSize = 128*1024)
        ///
        /// @brief  Starts an asynchronous HTTP post request
        ///
        /// Call this static method to make an asyncronous HTTP post request through the browser.
        /// Note that you can hold onto the ptr to the request but you don't have to -- it will be
        /// cleaned up automatically when the request finishes.
        ///
        /// @code
        /// void FBTestPluginAPI::postURL(const std::string& url, const std::string& postdata, concst const FB::JSObjectPtr& callback)
        /// {
        ///    FB::SimpleStreamHelper::AsyncPost(m_host, FB::URI::fromString(url), postdata,
        ///        boost::bind(&FBTestPluginAPI::getURLCallback, this, callback, _1, _2, _3, _4));
        /// }
        ///
        /// void FBTestPluginAPI::getURLCallback(const FB::JSObjectPtr& callback, bool success,
        ///    const FB::HeaderMap& headers, const boost::shared_array<uint8_t>& data, const size_t size) {
        ///    if (success) {
        ///        std::string dstr(reinterpret_cast<const char*>(data.get()), size);
        ///        // Do something here; dstr is a string containing the full body
        ///        // headers is a multimap with all the headers
        ///    } else {
        ///        // The request could not be completed
        ///    }
        /// }
        /// @endcode
        ///
        /// @param  host        const FB::BrowserHostPtr&   BrowserHostPtr to use to create the stream
        /// @param  uri         const FB::URI&              URI to request
        /// @param  postdata    const std::string&          Post data to send
        /// @param  callback    const HttpCallback &        Function to call with the response      
        /// @param  cache       const bool                  true if cache can be used
        /// @param  bufferSize  const size_t                Specifies the size of the buffer to use internally
        /// @returns FB::SimpleStreamHelperPtr
        /// @since 1.4b3
        /// @see SynchronousGet
        /// @see FB::URI
        ////////////////////////////////////////////////////////////////////////////////////////////////////
        static FB::SimpleStreamHelperPtr AsyncPost(const FB::BrowserHostPtr& host, const FB::URI& uri, const std::string& postdata, const HttpCallback& callback,
            bool cache = true, size_t bufferSize = 128*1024);


        ////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @fn public static FB::HttpStreamResponsePtr FB::SimpleStreamHelper::SynchronousGet(const FB::BrowserHostPtr& host, const FB::URI& uri, const bool cache = true, const size_t bufferSize = 128*1024)
        ///
        /// @brief  Do not call from the main thread! Starts a Synchronous HTTP get request. 
        ///
        /// Call this static method to make a synchronous HTTP get request through the browser from
        /// a thread that you have started.  *IMPORTANT*: This must *never* be called on the main thread!
        ///
        /// @code
        /// void FBTestPluginAPI::getURL(const std::string& url, const FB::JSObjectPtr& callback)
        /// {
        ///     FB::HttpStreamResponsePtr resp = FB::SimpleStreamHelper::SynchronousGet(m_host, FB::URI::fromString(url));
        ///     if (resp->success) {
        ///         std::string dstr(reinterpret_cast<const char*>(resp->data.get()), size);
        ///     } else {
        ///         // The request failed
        ///     }
        /// }
        /// }
        /// @endcode
        ///
        /// @param  host        const FB::BrowserHostPtr&   BrowserHostPtr to use to create the stream
        /// @param  uri         const FB::URI&              URI to request
        /// @param  cache       const bool                  true if cache can be used
        /// @param  bufferSize  const size_t                Specifies the size of the buffer to use internally
        /// @returns FB::HttpStreamResponsePtr
        /// @since 1.4b3
        /// @see AsyncGet
        /// @see FB::URI
        /// @see FB::HttpStreamResponse
        ////////////////////////////////////////////////////////////////////////////////////////////////////
        static HttpStreamResponsePtr SynchronousGet(const FB::BrowserHostPtr& host, const FB::URI& uri, 
            const bool cache = true, const size_t bufferSize = 128*1024);
        
        
        ////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @fn public static FB::HttpStreamResponsePtr FB::SimpleStreamHelper::SynchronousPost(const FB::BrowserHostPtr& host, const FB::URI& uri, const std::string& postdata, const bool cache = true, const size_t bufferSize = 128*1024)
        ///
        /// @brief  Do not call from the main thread! Starts a Synchronous HTTP POST request. 
        ///
        /// Call this static method to make a synchronous HTTP POST request through the browser from
        /// a thread that you have started.  *IMPORTANT*: This must *never* be called on the main thread!
        ///
        /// @code
        /// void FBTestPluginAPI::getURL(const std::string& url, const FB::JSObjectPtr& callback)
        /// {
        ///     FB::HttpStreamResponsePtr resp = FB::SimpleStreamHelper::SynchronousPost(m_host, FB::URI::fromString(url));
        ///     if (resp->success) {
        ///         std::string dstr(reinterpret_cast<const char*>(resp->data.get()), size);
        ///     } else {
        ///         // The request failed
        ///     }
        /// }
        /// }
        /// @endcode
        ///
        /// @param  host        const FB::BrowserHostPtr&   BrowserHostPtr to use to create the stream
        /// @param  uri         const FB::URI&              URI to request
        /// @param  postdata    const std::string&          Post data to send
        /// @param  cache       const bool                  true if cache can be used
        /// @param  bufferSize  const size_t                Specifies the size of the buffer to use internally
        /// @returns FB::HttpStreamResponsePtr
        /// @since 1.6RC2
        /// @see SynchronousGet
        /// @see FB::URI
        /// @see FB::HttpStreamResponse
        ////////////////////////////////////////////////////////////////////////////////////////////////////
        static HttpStreamResponsePtr SynchronousPost(const FB::BrowserHostPtr& host, const FB::URI& uri, const std::string& postdata, const bool cache = true, const size_t bufferSize = 128*1024);


    public:
        SimpleStreamHelper( const HttpCallback& callback, const size_t blockSize = 128*1024 );

        virtual bool onStreamDataArrived(FB::StreamDataArrivedEvent *evt, FB::BrowserStream *);
        virtual bool onStreamOpened(FB::StreamOpenedEvent *evt, FB::BrowserStream *);
        virtual bool onStreamCompleted(FB::StreamCompletedEvent *evt, FB::BrowserStream *);
        static FB::HeaderMap parse_http_headers(const std::string& headers );

    protected:
        typedef std::list<boost::shared_array<uint8_t> > BlockList;
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

