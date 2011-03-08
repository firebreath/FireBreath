/**********************************************************\ 
Original Author: Matthias (nitrogenycs)

Created:    Feb 28, 2010
License:    Dual license model; choose one of two:
            New BSD License
            http://www.opensource.org/licenses/bsd-license.php
            - or -
            GNU Lesser General Public License, version 2.1
            http://www.gnu.org/licenses/lgpl-2.1.html

Copyright 2010 Richard Bateman, Firebreath development team
\**********************************************************/

#pragma once
#ifndef H_ACTIVEXSTREAM
#define H_ACTIVEXSTREAM

#include "BrowserStream.h"
#include <set>
#include "boost/shared_ptr.hpp"

namespace FB { namespace ActiveX {
    class ActiveXBindStatusCallback;
    class ActiveXStreamRequest;
    typedef boost::shared_ptr<ActiveXStreamRequest> ActiveXStreamRequestPtr;

    class ActiveXStream : public FB::BrowserStream
    {
    public:
        ActiveXStream(const std::string& url, bool cache, bool seekable, size_t internalBufferSize );
        ActiveXStream(const std::string& url, bool cache, bool seekable, size_t internalBufferSize, const std::string& verbdata );
        virtual ~ActiveXStream();
        
        virtual bool readRanges( const std::vector<Range>& ranges );
        virtual bool write(const char* data, size_t dataLength, size_t& written);
        virtual bool close();

        virtual std::string getVerbData() const;

    protected:
        virtual bool    init();
        virtual bool    addRequest( const ActiveXStreamRequest& request );

        virtual size_t  signalDataArrived(void* buffer, size_t len, size_t offset);
        virtual void    signalOpened();
        virtual void    signalFailedOpen();
        virtual void    signalCompleted(bool success);
        virtual void    signalRequestCompleted(ActiveXStreamRequestPtr request, bool success);
        virtual void    signalCacheFilename(const std::wstring& cacheFilename);

        friend class ActiveXBrowserHost;
        friend class ActiveXBindStatusCallback;

    protected:
        std::set< ActiveXStreamRequestPtr > requests;
        bool    closing;
        std::string data;
    };
    typedef boost::shared_ptr<ActiveXStream> ActiveXStreamPtr;
} }


#endif

