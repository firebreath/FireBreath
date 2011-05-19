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
#ifndef UploadQueue_h__
#define UploadQueue_h__

#include <list>
#include "URI.h"
#include "UploadQueueEntry.h"
#include "PluginEventSource.h"
#include "PluginEvent.h"

namespace HTTP {
    class UploadQueue;
    class HTTPRequest;
    struct Status;
    typedef boost::shared_ptr<UploadQueue> UploadQueuePtr;

    class UploadQueue : public FB::PluginEventSource {
    public:
        class UploadErrorEvent : public FB::PluginEvent {
        public:
            UploadErrorEvent(const FB::VariantMap& status) : status(status) { }
            virtual ~UploadErrorEvent() {}

            FB::VariantMap status;
        };
        class StatusUpdateEvent : public FB::PluginEvent {
        public:
            StatusUpdateEvent(const FB::VariantMap& status) : status(status) { }
            virtual ~StatusUpdateEvent() {}

            FB::VariantMap status;
        };
    public:
        UploadQueue(const std::string& _name);
        virtual ~UploadQueue();

        UploadQueuePtr shared_from_this() { return FB::ptr_cast<UploadQueue>(FB::PluginEventSource::shared_from_this()); }
    
        bool hasFile(const std::wstring& filename);
        void addFile(const UploadQueueEntry& qe);
        bool removeFile(const std::wstring& filename);
        FB::VariantMap getStatusDict(bool include_filenames = true);
        static FB::VariantMap getEmptyProgressDict();
    
        void dispatch();
        void cancel();
    
        void addCompletionHandler(const FB::URI& uri);
    
        // ---
        std::string name;
        enum UploadQueueStatus {
            UPLOAD_ERROR = -1,
            UPLOAD_IDLE = 0,
            UPLOAD_WAITING = 1,
            UPLOAD_IN_PROGRESS = 2,
            UPLOAD_COMPLETE = 3
        } status;
        
        std::map<std::string, std::string> cookies;
        std::map<std::string, std::string> post_vars;
    
        std::list<UploadQueueEntry> queue;
        // bytes, files with status = WAITING
        uint32_t current_queue_bytes; 
        // bytes (original -- pre resize or any other postprocessing), files in current batch
        uint32_t current_batch_bytes; 
        // sum of bytes of files added to the queue since it was last emptied
        uint32_t total_queue_bytes; 
        // count of files added to the queue since it was last emptied
        uint32_t total_queue_files; 
        // count of files in WAITING state
        uint32_t files_waiting; 
    
        std::set<std::wstring> current_upload_files;
        HTTPRequest* current_upload_request;
        unsigned int current_batch_retry; // incremented every time we retry a batch due to networking failure, zeroed on a new batch.
    
        boost::function<void(UploadQueuePtr)> queue_finished_callback;
        std::list<FB::URI> completion_handlers;
        unsigned int batch_size;
        unsigned int max_retries;
    protected:
        void sendUpdateEvent();
        void start_next_upload();
        void upload_request_status_changed(const HTTP::Status& status);
    };
};
#endif // UploadQueue_h__

