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

#ifndef H_FB_PLUGINEVENTS_STREAMEVENTS
#define H_FB_PLUGINEVENTS_STREAMEVENTS

#include "PluginEvent.h"

namespace FB {
    class BrowserStream;

    class StreamEvent : public PluginEvent
    {
    public:
        StreamEvent(BrowserStream* Stream) : stream(Stream)
        {}

    public:
        BrowserStream* stream;
    };

    class StreamCreatedEvent : public StreamEvent
    {
    public:
        StreamCreatedEvent(BrowserStream* stream) : StreamEvent(stream)
        {}
    };

    class StreamDestroyedEvent : public StreamEvent
    {
    public:
        StreamDestroyedEvent(BrowserStream* stream) : StreamEvent(stream)
        {}
    };

    class StreamDataArrivedEvent : public StreamEvent
    {
    public:
        StreamDataArrivedEvent( BrowserStream* stream, const void* Data, const size_t Length, const size_t DataPosition, const float Progress ) : StreamEvent(stream), data(Data), length(Length), dataPosition(DataPosition), progress(Progress)
        {}

        const void* getData() const
        {
            return data;
        }
    
        const size_t getLength() const
        {
            return length;
        }

        const size_t getDataPosition() const
        {
            return dataPosition;
        }

        const float getProgress() const
        {
            return progress;
        }
    protected:
        const void*     data;           // the data
        const size_t    length;         // length of the data in bytes
        const size_t    dataPosition;   // the position in the stream where the data starts (e.g. 0 if the data starts at the beginning)
        const float     progress;       // the current progress in percent (0-100). 0 if progress is unknown (stream length not known in advance).
    };

    class StreamFailedOpenEvent : public StreamEvent
    {
    public:
        StreamFailedOpenEvent(BrowserStream* stream) : StreamEvent(stream)
        {}
    };

    class StreamOpenedEvent : public StreamEvent
    {
    public:
        StreamOpenedEvent(BrowserStream* stream) : StreamEvent(stream)
        {}
    };

    class StreamCompletedEvent : public StreamEvent
    {
    public:
        StreamCompletedEvent(BrowserStream* stream, bool Success) : StreamEvent(stream), success(Success)
        {}

    public:
        bool success;
    };
};

#endif // H_FB_PLUGINEVENTS_STREAMEVENTS
