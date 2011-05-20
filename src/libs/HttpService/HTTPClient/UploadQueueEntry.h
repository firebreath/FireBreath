/**********************************************************\ 
Original Author: Dan Weatherford

Imported into FireBreath:    Oct 4, 2010
License:    Dual license model; choose one of two:
            New BSD License
            http://www.opensource.org/licenses/bsd-license.php
            - or -
            GNU Lesser General Public License, version 2.1
            http://www.gnu.org/licenses/lgpl-2.1.html

Copyright 2010 Dan Weatherford and Facebook, Inc
\**********************************************************/

#pragma once
#ifndef H_HTTP_UPLOADQUEUEENTRY
#define H_HTTP_UPLOADQUEUEENTRY

#include "APITypes.h"
#include "../HTTPCommon/HTTPDatablock.h"

namespace HTTP {
    struct UploadQueueEntry {
        std::wstring source_path; // fully qualified path to file (like "/Users/me/Pictures/IMG_0001.JPG")
        std::wstring filename; // file name only (like "IMG_0001.JPG")
        FB::URI target;
    
        HTTPDatablock* datablock; // source datablock for upload
    
        uint32_t filesize; // given that the datablock can be an ImageDatablock which is lazy-evaluated, this might
        // not be an accurate size. It's just for guessing when the queue will be finished, anyway...
    
        // non-indexed variables, mutable for convenience
        mutable std::string result;
        mutable std::string post_field;
    
        enum Status {
            ENTRY_ERROR = -1,
            ENTRY_WAITING = 1,
            ENTRY_IN_PROGRESS = 2,
            ENTRY_COMPLETE = 3
        } status;
    
        void setStatus(Status _st) { status = _st; }
        void onFailure(const std::string& errmsg) { status = ENTRY_ERROR; result = errmsg; }
    
        UploadQueueEntry() : status(ENTRY_WAITING) {}
    };
};

#endif // H_HTTP_UPLOADQUEUEENTRY

