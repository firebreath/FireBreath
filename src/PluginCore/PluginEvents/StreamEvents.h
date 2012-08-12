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
#ifndef H_FB_PLUGINEVENTS_STREAMEVENTS
#define H_FB_PLUGINEVENTS_STREAMEVENTS

#include "../PluginEvent.h"

namespace FB {
    class BrowserStream;

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// @class  StreamEvent
    ///
    /// @brief  Base class for all stream events.
    ///
    /// @author Matthias
    ////////////////////////////////////////////////////////////////////////////////////////////////////
    class StreamEvent : public PluginEvent
    {
    public:
        StreamEvent(BrowserStream* Stream) : stream(Stream)
        {}

    public:
        BrowserStream* stream;
    };

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// @class  StreamCreatedEvent
    ///
    /// @brief  This event is fired when the given stream was created
    ///
    /// @author Matthias
    ////////////////////////////////////////////////////////////////////////////////////////////////////
    class StreamCreatedEvent : public StreamEvent
    {
    public:
        StreamCreatedEvent(BrowserStream* stream) : StreamEvent(stream)
        {}
    };

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// @class  StreamCreatedEvent
    ///
    /// @brief  This event is fired when the given stream is destroyed.
    ///
    /// @author Matthias
    ////////////////////////////////////////////////////////////////////////////////////////////////////
    class StreamDestroyedEvent : public StreamEvent
    {
    public:
        StreamDestroyedEvent(BrowserStream* stream) : StreamEvent(stream)
        {}
    };

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// @class  StreamCreatedEvent
    ///
    /// @brief  This event is fired when data on the given stream has arrived.
    ///
    /// @author Matthias
    ////////////////////////////////////////////////////////////////////////////////////////////////////
    class StreamDataArrivedEvent : public StreamEvent
    {
    public:
        StreamDataArrivedEvent( BrowserStream* stream, const void* Data, const size_t Length, const size_t DataPosition, const float Progress ) : StreamEvent(stream), data(Data), length(Length), dataPosition(DataPosition), progress(Progress)
        {}

        /// read-only pointer to the data
        const void* getData() const
        {
            return data;
        }
    
        /// returns the length of the data in bytes
        const size_t getLength() const
        {
            return length;
        }

        /// the position in the stream where the data starts (e.g. 0 if the data starts at the beginning)
        const size_t getDataPosition() const
        {
            return dataPosition;
        }

        /// gets the download progress in percent (0-100). 0 if progress is unknown (stream length not known in advance).
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

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// @class  StreamFailedOpenEvent
    ///
    /// @brief  This event is fired when a stream failed to open, e.g. the url was invalid or a seekable
    ///         stream was requested while the server provided only a non-seekable stream.
    ///
    /// @author Matthias
    ////////////////////////////////////////////////////////////////////////////////////////////////////
    class StreamFailedOpenEvent : public StreamEvent
    {
    public:
        StreamFailedOpenEvent(BrowserStream* stream) : StreamEvent(stream)
        {}
    };

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// @class  StreamFailedOpenEvent
    ///
    /// @brief  This event is fired when a stream was opened successfully.
    ///
    /// @author Matthias
    ////////////////////////////////////////////////////////////////////////////////////////////////////
    class StreamOpenedEvent : public StreamEvent
    {
    public:
        StreamOpenedEvent(BrowserStream* stream) : StreamEvent(stream)
        {}
    };

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// @class  StreamCompletedEvent
    ///
    /// @brief  This event is fired when a stream has completed downloading.
    ///
    /// @author Matthias
    ////////////////////////////////////////////////////////////////////////////////////////////////////
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

