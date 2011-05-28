/**********************************************************\
Original Author: Richard Bateman

Created:    Jan 25, 2011
License:    Dual license model; choose one of two:
            New BSD License
            http://www.opensource.org/licenses/bsd-license.php
            - or -
            GNU Lesser General Public License, version 2.1
            http://www.gnu.org/licenses/lgpl-2.1.html

Copyright 2011 Richard Bateman, 
               Firebreath development team
\**********************************************************/

#include "BrowserStream.h"
#include "precompiled_headers.h" // On windows, everything above this line in PCH

#include <boost/lambda/lambda.hpp>
#include <boost/lambda/bind.hpp>
#include <boost/thread/shared_mutex.hpp>
#include <algorithm>

#include "BrowserStreamManager.h"

FB::BrowserStreamManager::BrowserStreamManager()
{

}

FB::BrowserStreamManager::~BrowserStreamManager()
{
    boost::recursive_mutex::scoped_lock _l(m_xtmutex);
    using namespace boost::lambda;

    // Force close all of the streams so that they get a callback
    std::for_each(m_retainedStreams.begin(), m_retainedStreams.end(), 
        boost::lambda::bind(&BrowserStream::close, 
            boost::lambda::bind(&BrowserStreamPtr::get, boost::lambda::_1)));
    m_retainedStreams.clear();
}

void FB::BrowserStreamManager::retainStream( const BrowserStreamPtr& stream )
{
    boost::recursive_mutex::scoped_lock _l(m_xtmutex);
    stream->AttachObserver(shared_from_this());
    m_retainedStreams.insert(stream);
}

void FB::BrowserStreamManager::releaseStream( const BrowserStreamPtr& stream )
{
    boost::recursive_mutex::scoped_lock _l(m_xtmutex);
    stream->DetachObserver(shared_from_this());
    m_retainedStreams.erase(stream);
}

bool FB::BrowserStreamManager::onStreamCompleted( FB::StreamCompletedEvent *evt, FB::BrowserStream *stream )
{
    releaseStream(FB::ptr_cast<FB::BrowserStream>(stream->shared_from_this()));
    return false;
}

