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
#ifndef H_FB_NPAPI_NPAPISTREAM
#define H_FB_NPAPI_NPAPISTREAM

#include "NpapiTypes.h"
#include "NpapiBrowserHost.h"
#include "BrowserStream.h"

namespace FB { namespace Npapi {

    FB_FORWARD_PTR(NpapiPlugin);
    FB_FORWARD_PTR(NpapiStream);

    class NpapiStream : public FB::BrowserStream
    {
    public:
        NpapiStream( const std::string& url, bool cache, bool seekable, size_t internalBufferSize, const NpapiBrowserHostConstPtr &host );
        virtual ~NpapiStream();
        
        //virtual bool is_open();
        virtual bool    readRanges( const std::vector<Range>& ranges );
        virtual bool    write(const char* data, size_t dataLength, size_t& written);
        virtual bool    close();
        virtual void    setCreated();
        virtual void    setDestroyed();
        virtual void    setNotified();

    protected:
        virtual int32_t     signalDataArrived(void* buffer, int32_t len, int32_t offset);
        virtual void        signalOpened();
        virtual void        signalFailedOpen();
        virtual void        signalCompleted(bool success);
        virtual void        signalCacheFilename(const std::wstring& cacheFilename);

        virtual void        setStream(NPStream* stream);
        virtual NPStream*   getStream() const;
        virtual NpapiBrowserHostConstPtr   getHost() const;
    private:
        NPStream*               stream;
        NpapiBrowserHostWeakConstPtr       host;
        NpapiStreamPtr m_selfReference;
        bool m_streamDestroyed;
        bool m_streamNotified;
        friend class NpapiPlugin;
    };

}; }; // FB::Npapi

#endif

