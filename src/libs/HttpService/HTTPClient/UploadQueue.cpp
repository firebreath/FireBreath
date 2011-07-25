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

#include "UploadQueue.h"
#include "HTTPRequest.h"
#include "JSAPI.h"
#include <boost/algorithm/string.hpp>
#include "../HTTPCommon/Status.h"
#include "../HTTPCommon/Utils.h"
#include "logging.h"

using namespace HTTP;
using namespace boost::algorithm;

UploadQueue::UploadQueue( const std::string& _name )
    : name(_name), status(UPLOAD_IDLE), current_queue_bytes(0), current_batch_bytes(0), total_queue_bytes(0),
    total_queue_files(0), files_waiting(0), current_upload_request(NULL), current_batch_retry(0),
    batch_size(8), max_retries(3)
{

}

UploadQueue::~UploadQueue()
{

}

FB::VariantMap UploadQueue::getEmptyProgressDict() {
    FB::VariantMap d;
    d["queue_name"] = std::string();
    d["current_files"] = FB::VariantList();
    d["current_queue_bytes_remaining"] = 0;
    d["current_queue_files_remaining"] = 0;
    d["total_queue_bytes"] = 0;
    d["total_queue_files"] = 0;
    d["batches_remaining"] = 0;
    d["status"] = "Unknown";

    return d;
}

FB::VariantMap UploadQueue::getStatusDict(bool include_filenames) {
    FB::VariantMap d;
    // whole-queue stats
    d["queue_name"] = name;
    d["current_queue_bytes_remaining"] = current_queue_bytes;
    d["total_queue_bytes"] = total_queue_bytes;
    d["total_queue_files"] = total_queue_files;
    d["current_queue_files_remaining"] = files_waiting;
    d["batches_remaining"] = ceil(static_cast<float>(files_waiting) / static_cast<float>(batch_size));
    d["status"] = "Uploading";

    // this-batch stats
    if (include_filenames) {
        FB::VariantList cf;
        for (std::set<std::wstring>::const_iterator it = current_upload_files.begin();
            it != current_upload_files.end(); ++it)  {
                cf.push_back(*it);
        }
        d["current_files"] = cf;
    }
    d["current_batch_bytes_total"] = current_batch_bytes;

    double current_batch_pct_complete = 0.0;

    if (current_upload_request) {
        HTTP::Status st = current_upload_request->getStatus();
        FB::VariantMap status(st.asDict());
        d.insert(status.begin(), status.end());
        if (st.send_total) {
            current_batch_pct_complete = static_cast<double>(st.bytes_sent)
                / static_cast<double>(st.send_total);
        }
    }

    d["current_batch_pct_complete"] = current_batch_pct_complete;
    d["overall_progress_pct"] = static_cast<double>(
        (total_queue_bytes - (current_queue_bytes + current_batch_bytes))
            + (current_batch_pct_complete *  current_batch_bytes))
        / static_cast<double>(total_queue_bytes);

    return d;
}

void UploadQueue::dispatch() {
    status = UploadQueue::UPLOAD_IN_PROGRESS;
    start_next_upload();

    // Deliver the first status dict to HTTP server. We need it to be there before this
    // function returns, because otherwise there's a race between the HTTP request posting
    // actual progress data and the JS setting up the polling for it, where JS could see
    // an empty progress dict and think the uploads are all done.
    // TODOTODO
    //{
    //    boost::shared_ptr<UploadStatusHandler> ph = http_status_handler_weak.lock();
    //    if (ph) ph->postUpdate(getStatusDict());
    //}
    sendUpdateEvent();
}

void UploadQueue::start_next_upload() {
    unsigned int files_started = 0;

    boost::shared_ptr<HTTPRequestData> data(new HTTPRequestData);
    data->method = "POST";
    data->cookies = cookies;

    current_upload_files.clear();
    current_batch_bytes = 0;
    current_batch_retry = 0;

    for (std::list<UploadQueueEntry>::iterator it = queue.begin(); it != queue.end(); ++it) {
        if (it->status != UploadQueueEntry::ENTRY_WAITING) continue;
        try {
            UploadQueueEntry& qe = *it;

            // Open and process the image.
            // We do this now so we can catch errors and attribute them to individual images;
            // letting the HTTPRequest do it would fail the whole request if a single image
            // was unreadable.
            qe.datablock->resolve();

            // Pull in post params
            for (std::map<std::string, std::string>::iterator pvit = post_vars.begin();
                pvit != post_vars.end(); ++pvit) {
                qe.target.query_data[pvit->first] = pvit->second;
            }

            if (files_started == 0) {
                // First match sets the target uri
                data->uri = qe.target;
            } else {
                // Subsequently, we can only batch up more uploads to the same endpoint
                if (!(data->uri == qe.target)) continue;
            }

            it->setStatus(UploadQueueEntry::ENTRY_IN_PROGRESS);
            --files_waiting;
            current_queue_bytes -= qe.filesize;
            
            std::stringstream ss;
            ss << "file" << files_started;
            qe.post_field = ss.str();
            data->addFile(ss.str(), FB::wstring_to_utf8(qe.filename), "application/octet-stream", qe.datablock);

            current_upload_files.insert(qe.source_path);
            current_batch_bytes += qe.filesize;
            ++files_started;
            if (files_started >= batch_size) break;

        } catch (const std::exception& e) {
            it->result = e.what();
            it->setStatus(UploadQueueEntry::ENTRY_ERROR);
        }
    }

    if (files_started) {
        // We had enough images left in the queue for another request, so kick that off now.
        current_upload_request = HTTPRequest::create();
        current_upload_request->onStatusChanged(
            bind(&UploadQueue::upload_request_status_changed, this, _1)
            );
        current_upload_request->startRequest(data);

        // As long as we're doing uploads, we want to keep the HTTP server
        // up to provide progress to the chat bar widget -- so enable deferred shutdown.
        // TODOTODO
        //try {
        //    boost::shared_ptr<HTTPService> h = http_srv_inst_weak.lock();
        //    if (h) h->setDeferShutdown(true);
        //} catch (...) {}

    } else {
#ifndef NDEBUG
        FBLOG_DEBUG("UploadQueue",
            "start_next_upload() found no waiting files, running completion handlers\n");
#endif
        // All done, post upload finished callback to all instances
        FB::VariantMap d = getEmptyProgressDict();
        FB::VariantMap failures;

        for (std::list<UploadQueueEntry>::iterator it = queue.begin(); it != queue.end(); ++it) {
            if (it->status == UploadQueueEntry::ENTRY_ERROR) {
                failures[FB::wstring_to_utf8(it->source_path)] = it->result;
#ifndef NDEBUG
                FBLOG_WARN("UploadQueue", "Reporting file \"" << it->source_path.c_str()
                    << "\" as failed: " << it->result.c_str() << std::endl);
#endif        
            }
        }
        queue.clear();

        if (! failures.empty()) d["failed_files"] = failures;

        d["status"] = "Complete";
        status = UploadQueue::UPLOAD_COMPLETE;
        // fire completion handlers, if available
        for (std::list<FB::URI>::iterator it = completion_handlers.begin();
            it != completion_handlers.end(); ++it) {
            boost::shared_ptr<HTTPRequestData> reqdata(new HTTPRequestData(*it));
            reqdata->cookies = cookies;
            HTTPRequest::asyncStartRequest(reqdata);
        }

        if (queue_finished_callback)
            queue_finished_callback(shared_from_this());
        
        StatusUpdateEvent evt(d);
        SendEvent(&evt);
    }
}

void UploadQueue::upload_request_status_changed(const HTTP::Status &status) {
    // Deliver status dict to interested observers
    sendUpdateEvent();

    uint32_t this_batch_size = 0;
    if (status.state == HTTP::Status::HTTP_ERROR) {
        if ((++current_batch_retry) < max_retries) {
            // Retry on networking error.
#ifndef NDEBUG
            FBLOG_WARN("UploadQueue", "Retrying current batch on networking error ("
                << status.last_error.c_str() << ")" << std::endl);
#endif
            // Retake ownership of the request data, dispose this request, and start a new one.
            // The request data doesn't get mangled by the HTTPRequest object, it should be safe
            // to reuse.
            boost::shared_ptr<HTTPRequestData> req_data = current_upload_request->getRequest();
            current_upload_request->threadSafeDestroy();
            {
                FB::VariantMap err;
                // Notify the plugin that there was an error
                err["status"] = "Error";
                err["retries_remaining"] = max_retries - current_batch_retry;
                err["message"] = status.last_error;
                err["queueStatus"] = getStatusDict();
                UploadErrorEvent evt(err);
                SendEvent(&evt);
            }

            current_upload_request = HTTPRequest::create();
            current_upload_request->onStatusChanged(
                bind(&UploadQueue::upload_request_status_changed, this, _1)
                );
            current_upload_request->startRequest(req_data);
            return;
        }
        {
            // Notify the plugin that there was an error
            FB::VariantMap err;
            err["status"] = "Failed";
            err["retries_remaining"] = 0;
            err["message"] = status.last_error;
            UploadErrorEvent evt(err);
            SendEvent(&evt);
        }
        for (std::list<UploadQueueEntry>::iterator it = queue.begin(); it != queue.end(); ++it) {
            if (it->status == UploadQueueEntry::ENTRY_IN_PROGRESS) {
                this_batch_size += it->filesize;
                it->onFailure(status.last_error);
            }
        }
    } else if (status.state == HTTP::Status::COMPLETE) {
        HTTPDatablock* db = current_upload_request->getResponse()->coalesceBlocks();

        std::multimap<std::string, std::string> file_statuses;

        try {
            std::vector<std::string> status_lines;
            std::string db_str(db->data(), db->size());
            std::istringstream data_stream(db_str);

            while (!data_stream.eof()) {
                std::string tmp;
                std::getline(data_stream, tmp);
                trim(tmp);
                if (tmp.size() && tmp.at(tmp.size() - 1) == '\r') tmp.resize(tmp.size() - 1);
                if (tmp.empty()) break;
                status_lines.push_back(tmp);
            }
            file_statuses = parse_http_headers(status_lines.begin(), status_lines.end());
        } catch (const std::exception& e) {
#ifndef NDEBUG
            printf("Parse error while reading file statuses back from endpoint: %s\n", e.what());
#endif
        }

        for (std::list<UploadQueueEntry>::iterator it = queue.begin(); it != queue.end(); ++it) {
            if (it->status == UploadQueueEntry::ENTRY_IN_PROGRESS) {
                this_batch_size += it->filesize;

                bool had_error = false;

                // See if we got an error condition back from the endpoint
                std::multimap<std::string, std::string>::iterator stat_it
                    = file_statuses.find(it->post_field);
                if (stat_it != file_statuses.end()) {
                    std::string lcase_status = to_lower_copy(stat_it->second);
                    if (lcase_status.size() >= 5 && (lcase_status.substr(0,5) == "error")) {
#ifndef NDEBUG
                        FBLOG_WARN("UploadQueue", "Upload endpoint signaled an error for field "
                            << it->post_field.c_str() << ":" << stat_it->second.c_str() << std::endl);
#endif
                        it->onFailure(stat_it->second);
                        had_error = true;
                    }
                } else {
#ifndef NDEBUG
                    FBLOG_WARN("UploadQueue", "Upload endpoint didn't return a status for field "
                        << it->post_field.c_str() << std::endl);
#endif
                }

                if (!had_error) it->setStatus(UploadQueueEntry::ENTRY_COMPLETE);
            }
        }
    } else return;

    boost::shared_ptr<HTTPResponseData> response = current_upload_request->getResponse();
    if (response) {
        for (std::map<std::string, std::string>::iterator it = response->cookies.begin();
            it != response->cookies.end(); ++it) {
            cookies[it->first] = it->second;
        }
    }


    // using threadSafeDestroy since our caller is probably the i/o service of that request...
    current_upload_request->threadSafeDestroy();
    current_upload_request = NULL;

    // Since this is done (error or not), find another queue entry that's WAITING and start that.
    start_next_upload();
}

void UploadQueue::cancel() {
    if (current_upload_request) {
        HTTPRequest* r = current_upload_request;
        current_upload_request = NULL;

        r->cancel();
        delete r;
    }

    queue.clear();
    status = UploadQueue::UPLOAD_COMPLETE;
    if (queue_finished_callback) queue_finished_callback(shared_from_this());
}

void UploadQueue::sendUpdateEvent()
{
    StatusUpdateEvent evt(getStatusDict());
    SendEvent(&evt);
}

void HTTP::UploadQueue::addCompletionHandler( const FB::URI& uri )
{
    if (status != UPLOAD_IDLE) throw FB::script_error("UploadQueue::addCompletionHandler(): queue has been dispatched, can't be modified");
    completion_handlers.push_back(uri);
}

void HTTP::UploadQueue::addFile( const UploadQueueEntry& qe )
{
    if (status != UPLOAD_IDLE) throw FB::script_error("UploadQueue::addFile(): queue has been dispatched, can't be modified");

    current_queue_bytes += qe.filesize;
    total_queue_bytes += qe.filesize;
    ++total_queue_files;
    ++files_waiting;
    queue.push_back(qe);
}

bool HTTP::UploadQueue::removeFile( const std::wstring& source_path )
{
    if (status != UPLOAD_IDLE) throw FB::script_error("UploadQueue::addFile(): queue has been dispatched, can't be modified");

    for (std::list<UploadQueueEntry>::iterator it = queue.begin(); it != queue.end(); ++it) {
        if (it->source_path == source_path) {
            current_queue_bytes -= it->filesize;
            total_queue_bytes -= it->filesize;
            --total_queue_files;
            --files_waiting;
            queue.erase(it);
            return true;
        }
    }

    return false; // not found
}

bool HTTP::UploadQueue::hasFile( const std::wstring& filename )
{
    for (std::list<UploadQueueEntry>::iterator it = queue.begin(); it != queue.end(); ++it) {
        if (it->source_path == filename) {
            return true;
        }
    }
    return false;
}

void sendUpdateEvent();

