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

#ifdef _WIN32
#include "win_targetver.h"
#include <boost/asio.hpp>
#include "../Platform/windows_defs.h"
#endif

#include <boost/algorithm/string.hpp>
#include <boost/format.hpp>
#include <boost/lexical_cast.hpp>
#include <cassert>
#include <openssl/ssl.h>
#include <openssl/x509v3.h>
#include <curl/curl.h>
#include "../HTTPService/BasicService.h"
#include "../HTTPCommon/Utils.h"

#include "HTTPRequest.h"
using namespace boost::algorithm;
using namespace boost::asio;
using namespace boost::asio::ip;
using boost::format;
using boost::lexical_cast;
using boost::mutex;
using boost::thread;
using std::list;
using std::multimap;
using std::string;
using std::vector;
using std::map;

static const char* user_agent = "FBPlugin";

using namespace HTTP;

HTTPProxyConfig HTTPRequest::static_proxy_config;

boost::shared_ptr<HTTPResponseData> parse_http_response(boost::asio::streambuf& response_stream);

/*static*/ void HTTPRequest::setProxyConfig(const HTTPProxyConfig& _cfg) {
  static_proxy_config = _cfg;
}

void onStatusChanged_do_nothing(HTTP::Status state) {
}

/*static*/ HTTPRequest* HTTPRequest::create() {
  return new HTTPRequest();
}

static void asyncrequest_status_handler(HTTPRequest* req, HTTP::Status status) {
  if (status.state == HTTP::Status::HTTP_ERROR || status.state == HTTP::Status::CANCELLED || status.state == HTTP::Status::COMPLETE) {
    req->threadSafeDestroy();
  }
}

/*static*/ void HTTPRequest::asyncStartRequest(boost::shared_ptr<HTTPRequestData> data) {
  HTTPRequest* req = create();
  req->onStatusChanged(boost::bind(asyncrequest_status_handler, req, _1));
  req->startRequest(data);
}

// The cert chain that we're looking for looks like:
// Entrust Secure Server Certification Authority (this cert)
// signs DigiCert High Assurance EV Root CA
// signs DigiCert High Assurance EV CA-1
// signs Facebook's cert for www.facebook.com
//
// so we load up the entrust cert in memory and let openssl do the rest.
// (this avoids depending on shipping a curl-ca-cert bundle on windows)

/*
        Version: 3 (0x2)
        Serial Number: 927650371 (0x374ad243)
        Signature Algorithm: sha1WithRSAEncryption
        Issuer: C=US, O=Entrust.net, OU=www.entrust.net/CPS incorp. by ref. (limits liab.), OU=(c) 1999 Entrust.net Limited, CN=Entrust.net Secure Server Certification Authority
        Validity
            Not Before: May 25 16:09:40 1999 GMT
            Not After : May 25 16:39:40 2019 GMT
        Subject: C=US, O=Entrust.net, OU=www.entrust.net/CPS incorp. by ref. (limits liab.), OU=(c) 1999 Entrust.net Limited, CN=Entrust.net Secure Server Certification Authority
        Subject Public Key Info:
            Public Key Algorithm: rsaEncryption
            RSA Public Key: (1024 bit)
                Modulus (1024 bit):
                    00:cd:28:83:34:54:1b:89:f3:0f:af:37:91:31:ff:
                    af:31:60:c9:a8:e8:b2:10:68:ed:9f:e7:93:36:f1:
                    0a:64:bb:47:f5:04:17:3f:23:47:4d:c5:27:19:81:
                    26:0c:54:72:0d:88:2d:d9:1f:9a:12:9f:bc:b3:71:
                    d3:80:19:3f:47:66:7b:8c:35:28:d2:b9:0a:df:24:
                    da:9c:d6:50:79:81:7a:5a:d3:37:f7:c2:4a:d8:29:
                    92:26:64:d1:e4:98:6c:3a:00:8a:f5:34:9b:65:f8:
                    ed:e3:10:ff:fd:b8:49:58:dc:a0:de:82:39:6b:81:
                    b1:16:19:61:b9:54:b6:e6:43
                Exponent: 3 (0x3)
        X509v3 extensions:
            Netscape Cert Type: 
                SSL CA, S/MIME CA, Object Signing CA
            X509v3 CRL Distribution Points: 
                DirName:/C=US/O=Entrust.net/OU=www.entrust.net/CPS incorp. by ref. (limits liab.)/OU=(c) 1999 Entrust.net Limited/CN=Entrust.net Secure Server Certification Authority/CN=CRL1
                URI:http://www.entrust.net/CRL/net1.crl

            X509v3 Private Key Usage Period: 
                Not Before: May 25 16:09:40 1999 GMT, Not After: May 25 16:09:40 2019 GMT
            X509v3 Key Usage: 
                Certificate Sign, CRL Sign
            X509v3 Authority Key Identifier: 
                keyid:F0:17:62:13:55:3D:B3:FF:0A:00:6B:FB:50:84:97:F3:ED:62:D0:1A

            X509v3 Subject Key Identifier: 
                F0:17:62:13:55:3D:B3:FF:0A:00:6B:FB:50:84:97:F3:ED:62:D0:1A
            X509v3 Basic Constraints: 
                CA:TRUE
            1.2.840.113533.7.65.0: 
                0

*/
//static const char* certdata_Entrust_Secure_Server_CA =
//"-----BEGIN CERTIFICATE-----\n"
//"MIIE2DCCBEGgAwIBAgIEN0rSQzANBgkqhkiG9w0BAQUFADCBwzELMAkGA1UE\n"
//"BhMCVVMxFDASBgNVBAoTC0VudHJ1c3QubmV0MTswOQYDVQQLEzJ3d3cuZW50\n"
//"cnVzdC5uZXQvQ1BTIGluY29ycC4gYnkgcmVmLiAobGltaXRzIGxpYWIuKTEl\n"
//"MCMGA1UECxMcKGMpIDE5OTkgRW50cnVzdC5uZXQgTGltaXRlZDE6MDgGA1UE\n"
//"AxMxRW50cnVzdC5uZXQgU2VjdXJlIFNlcnZlciBDZXJ0aWZpY2F0aW9uIEF1\n"
//"dGhvcml0eTAeFw05OTA1MjUxNjA5NDBaFw0xOTA1MjUxNjM5NDBaMIHDMQsw\n"
//"CQYDVQQGEwJVUzEUMBIGA1UEChMLRW50cnVzdC5uZXQxOzA5BgNVBAsTMnd3\n"
//"dy5lbnRydXN0Lm5ldC9DUFMgaW5jb3JwLiBieSByZWYuIChsaW1pdHMgbGlh\n"
//"Yi4pMSUwIwYDVQQLExwoYykgMTk5OSBFbnRydXN0Lm5ldCBMaW1pdGVkMTow\n"
//"OAYDVQQDEzFFbnRydXN0Lm5ldCBTZWN1cmUgU2VydmVyIENlcnRpZmljYXRp\n"
//"b24gQXV0aG9yaXR5MIGdMA0GCSqGSIb3DQEBAQUAA4GLADCBhwKBgQDNKIM0\n"
//"VBuJ8w+vN5Ex/68xYMmo6LIQaO2f55M28Qpku0f1BBc/I0dNxScZgSYMVHIN\n"
//"iC3ZH5oSn7yzcdOAGT9HZnuMNSjSuQrfJNqc1lB5gXpa0zf3wkrYKZImZNHk\n"
//"mGw6AIr1NJtl+O3jEP/9uElY3KDegjlrgbEWGWG5VLbmQwIBA6OCAdcwggHT\n"
//"MBEGCWCGSAGG+EIBAQQEAwIABzCCARkGA1UdHwSCARAwggEMMIHeoIHboIHY\n"
//"pIHVMIHSMQswCQYDVQQGEwJVUzEUMBIGA1UEChMLRW50cnVzdC5uZXQxOzA5\n"
//"BgNVBAsTMnd3dy5lbnRydXN0Lm5ldC9DUFMgaW5jb3JwLiBieSByZWYuIChs\n"
//"aW1pdHMgbGlhYi4pMSUwIwYDVQQLExwoYykgMTk5OSBFbnRydXN0Lm5ldCBM\n"
//"aW1pdGVkMTowOAYDVQQDEzFFbnRydXN0Lm5ldCBTZWN1cmUgU2VydmVyIENl\n"
//"cnRpZmljYXRpb24gQXV0aG9yaXR5MQ0wCwYDVQQDEwRDUkwxMCmgJ6AlhiNo\n"
//"dHRwOi8vd3d3LmVudHJ1c3QubmV0L0NSTC9uZXQxLmNybDArBgNVHRAEJDAi\n"
//"gA8xOTk5MDUyNTE2MDk0MFqBDzIwMTkwNTI1MTYwOTQwWjALBgNVHQ8EBAMC\n"
//"AQYwHwYDVR0jBBgwFoAU8BdiE1U9s/8KAGv7UISX8+1i0BowHQYDVR0OBBYE\n"
//"FPAXYhNVPbP/CgBr+1CEl/PtYtAaMAwGA1UdEwQFMAMBAf8wGQYJKoZIhvZ9\n"
//"B0EABAwwChsEVjQuMAMCBJAwDQYJKoZIhvcNAQEFBQADgYEAkNwwAvpkdMKn\n"
//"CqV8IY00F6j7Rw7/JXyNEwr75Ji174z4xRAN95K+8cPV1ZVqBLssziY2Zcgx\n"
//"xufuP+NXdYR6Ee9GTxj005i7qIcyunL2POI9n9cd2cNgQ4xYDiKWL2KjLB+6\n"
//"rQXvqzJ4h6BUcxm1XAX5Uj5tLUUL9wqT6u0G+bI=\n"
//"-----END CERTIFICATE-----\n";

static std::set<std::string> CA_certs;
static bool clean_CAList(true);

void HTTPRequest::registerCACert(const std::string& cert)
{
    CA_certs.insert(cert);
}

static CURLcode sslctx_function(CURL* curl, SSL_CTX* sslctx, void* param) {
    X509_STORE* store;
    if (clean_CAList) {
        store = X509_STORE_new();
        SSL_CTX_set_cert_store(sslctx, store);
    } else {
        store = SSL_CTX_get_cert_store(sslctx);
    }
    for(std::set<std::string>::iterator it=CA_certs.begin(); it != CA_certs.end(); ++it)
    {
        BIO* mem = BIO_new_mem_buf(const_cast<char*>(it->c_str()), -1);
        BIO_set_close(mem, BIO_NOCLOSE); // don't want BIO_free() to free the buffer, it's a string
        X509* current_CA = PEM_read_bio_X509_AUX(mem, NULL, NULL, NULL);
        if (current_CA) {
            X509_STORE_add_cert(store, current_CA);
        }
        BIO_free(mem);
    }

    return CURLE_OK;
}

void HTTPRequest::_internal_threadSafeDestroy() {
  delete this;
}

void HTTPRequest::threadSafeDestroy() {
  boost::thread t(boost::bind(&HTTPRequest::_internal_threadSafeDestroy, this));
}

HTTPRequest::HTTPRequest() : req(NULL), cancellation_requested(false), status_callback(onStatusChanged_do_nothing) {

}

HTTPRequest::~HTTPRequest() {
  if (worker_thread) {
    if (! cancellation_requested) {
      if (!(last_status.state == Status::IDLE || last_status.state == Status::COMPLETE || last_status.state == Status::HTTP_ERROR)) {
#ifndef NDEBUG
        printf("HTTPRequest: destroyed with active worker thread, probably bad.\n");
#endif
      }
      cancel();
    }
    awaitCompletion();
  }
}

void HTTPRequest::cancel() {
  cancellation_requested = true;
}

void HTTPRequest::awaitCompletion() {
  worker_thread->join();
}

HTTP::Status HTTPRequest::getStatus() const {
  return last_status;
}

void HTTPRequest::onStatusChanged(const callback_fn_t& _callback) {
    status_callback = _callback;
}

boost::shared_ptr<HTTPRequestData> HTTPRequest::getRequest() {
  return request_data;
}

boost::shared_ptr<HTTPResponseData> HTTPRequest::getResponse() {
  return response_data;
}

void HTTPRequest::startRequest(boost::shared_ptr<HTTPRequestData> _request_data) {
    request_data = _request_data;
  proxy_config = static_proxy_config;
  
  // We build the HTTP request in another thread to avoid blocking the caller if the request is large or resolving the size of the attached content takes a while.
    worker_thread = boost::shared_ptr<thread>(new thread(boost::bind(&HTTPRequest::startRequest_thread, this)));
}

static size_t httprequest_writefn(void* ptr, size_t size, size_t nmemb, HTTPResponseData* resp) {
  size_t total = size * nmemb;
  if (total) {
    resp->data.push_back(new HTTPStringDatablock(reinterpret_cast<char*>(ptr), total));
  }
  return total;
}

static int httprequest_progress(void *clientp, double dltotal, double dlnow, double ultotal, double ulnow) {
  return reinterpret_cast<HTTPRequest*>(clientp)->curl_progress(dltotal, dlnow, ultotal, ulnow);
}

static size_t httprequest_header(void *ptr, size_t size, size_t nmemb, void *clientp) {
  reinterpret_cast<HTTPRequest*>(clientp)->curl_header(reinterpret_cast<const char*>(ptr), size * nmemb);
  return size * nmemb;
}

void HTTPRequest::startRequest_thread() {
  struct curl_httppost* formpost = NULL;
  struct curl_httppost* lastptr = NULL;
  struct curl_slist* headerlist = NULL;
  
  char errorbuffer[CURL_ERROR_SIZE];

  try {
    response_data = boost::shared_ptr<HTTPResponseData>(new HTTPResponseData);

    req = curl_easy_init();
    
    curl_easy_setopt(req, CURLOPT_NOSIGNAL, 1); // disable signals for multithreaded use
    curl_easy_setopt(req, CURLOPT_ERRORBUFFER, errorbuffer);
    curl_easy_setopt(req, CURLOPT_FAILONERROR, 1);

    curl_easy_setopt(req, CURLOPT_SSL_CTX_FUNCTION, sslctx_function);

    curl_easy_setopt(req, CURLOPT_SSL_VERIFYPEER, 1);
    curl_easy_setopt(req, CURLOPT_SSL_VERIFYHOST, 2);

    // THESE ARE REALLY DANGEROUS!!!! DON'T LEAVE THEM ACTIVE!
    //curl_easy_setopt(req, CURLOPT_SSL_VERIFYPEER, 0);
    //curl_easy_setopt(req, CURLOPT_SSL_VERIFYHOST, 0);
   
    std::map<std::string, std::string> post_data = request_data->uri.query_data;    

    
    std::string uri_string; // curl doesn't copy this, so we have to hold onto it
    
    { // We need a copy of the URI without the query data for curl
      FB::URI bare_uri = request_data->uri;
      bare_uri.query_data.clear();
      uri_string = bare_uri.toString();
    }
    
    curl_easy_setopt(req, CURLOPT_URL, uri_string.c_str());
    curl_easy_setopt(req, CURLOPT_WRITEFUNCTION, httprequest_writefn);
    curl_easy_setopt(req, CURLOPT_WRITEDATA, response_data.get());
    curl_easy_setopt(req, CURLOPT_USERAGENT, user_agent);
    
    bool have_post_data = (post_data.size() || request_data->files.size());
    if (have_post_data) {
     
      for (std::map<std::string, std::string>::const_iterator it = post_data.begin(); it != post_data.end(); ++it) {
        curl_formadd(&formpost, &lastptr,
          CURLFORM_PTRNAME, it->first.c_str(),
          CURLFORM_PTRCONTENTS, it->second.c_str(),
          CURLFORM_CONTENTSLENGTH, it->second.size(),
          CURLFORM_END);
      }
      for (std::map<std::string, HTTPFileEntry>::iterator it = request_data->files.begin(); it != request_data->files.end(); ++it) {
        curl_formadd(&formpost, &lastptr,
          CURLFORM_PTRNAME, it->first.c_str(),
          CURLFORM_BUFFER, it->second.filename.c_str(),
          CURLFORM_BUFFERPTR, it->second.contents->data(),
          CURLFORM_BUFFERLENGTH, it->second.contents->size(),
          CURLFORM_CONTENTTYPE, it->second.content_type.c_str(),
          CURLFORM_END);
      }
      
      curl_easy_setopt(req, CURLOPT_POST, 1);
      curl_easy_setopt(req, CURLOPT_HTTPPOST, formpost);
    }
    
    std::string cookie_string = build_cookie_header(request_data->cookies);
    curl_easy_setopt(req, CURLOPT_COOKIE, cookie_string.c_str());
    
    for (std::multimap<std::string, std::string>::iterator it = request_data->headers.begin(); it != request_data->headers.end(); ++it) {
      headerlist = curl_slist_append(headerlist, (it->first + string(": ") + it->second).c_str());
    }
    // remove the Expect header that cURL insists on adding. this is dangerous
    // and results in 417 errors from old/misconfigured proxies as well as lighttpd
    // versions prior to 0.5. yes, this is how you disable headers in cURL; you pass
    // the header name with no data on the right side. classy.
    headerlist = curl_slist_append(headerlist, "Expect:");
      
    curl_easy_setopt(req, CURLOPT_HTTPHEADER, headerlist);
    curl_easy_setopt(req, CURLOPT_FOLLOWLOCATION, 0); // no redirects for security reasons

    // proxy setup
    switch (proxy_config.type) {
      default:
      case HTTPProxyConfig::kNoProxy:
        break;
      case HTTPProxyConfig::kSOCKS4Proxy:
        curl_easy_setopt(req, CURLOPT_PROXYTYPE, CURLPROXY_SOCKS4);
        break;
      case HTTPProxyConfig::kHTTPProxy:
        curl_easy_setopt(req, CURLOPT_PROXYTYPE, CURLPROXY_HTTP);
        break;
    };
    if (proxy_config.type != HTTPProxyConfig::kNoProxy) {
      curl_easy_setopt(req, CURLOPT_PROXY, proxy_config.hostname.c_str());
      curl_easy_setopt(req, CURLOPT_PROXYPORT, proxy_config.port);
    }

    curl_easy_setopt(req, CURLOPT_NOPROGRESS, 0);
    curl_easy_setopt(req, CURLOPT_PROGRESSFUNCTION, httprequest_progress);
    curl_easy_setopt(req, CURLOPT_PROGRESSDATA, this);
    curl_easy_setopt(req, CURLOPT_HEADERFUNCTION, httprequest_header);
    curl_easy_setopt(req, CURLOPT_WRITEHEADER, this);

    last_status.state = Status::CONNECTING;
    status_callback(last_status);

    if (curl_easy_perform(req) != 0) {
      if (cancellation_requested) {
        last_status.state = Status::CANCELLED;
        status_callback(last_status);
      } else {
        throw std::runtime_error(errorbuffer);
      }
    } else {
      last_status.state = Status::COMPLETE;
      status_callback(last_status);
    }
  } catch (const std::exception& e) {
    last_status.state = Status::HTTP_ERROR;
    last_status.last_error = e.what();
    status_callback(last_status);
  }
  
  long code;
  curl_easy_getinfo(req, CURLINFO_RESPONSE_CODE, &code);
  response_data->code = code;
  
  curl_slist_free_all(headerlist);
  curl_formfree(formpost);
  curl_easy_cleanup(req);
  req = NULL;
}

void HTTPRequest::curl_header(const char* data, size_t size) {
  std::string s(data, size);
  size_t l = s.find(':');
  if (l == std::string::npos) return;
  string k = s.substr(0,l);
  string v = s.substr(l+1);
  trim(k);
  trim(v);
  response_data->headers.insert(std::make_pair(k,v));
}

int HTTPRequest::curl_progress(double dltotal, double dlnow, double ultotal, double ulnow) {
  if (cancellation_requested) {
    return -1;
  }
  
  if (ultotal) {
    last_status.bytes_sent = static_cast<size_t>(ulnow);
    last_status.send_total = static_cast<size_t>(ultotal);
    last_status.state = Status::SEND_REQUEST;
  }
  
  if (dltotal) {
    last_status.state = Status::READ_RESPONSE;
    last_status.bytes_received = static_cast<size_t>(dlnow);
    last_status.receive_total = static_cast<size_t>(dltotal);
  }
  
  curl_easy_getinfo(req, CURLINFO_SPEED_DOWNLOAD, &last_status.bytes_per_second_receive);
  curl_easy_getinfo(req, CURLINFO_SPEED_UPLOAD, &last_status.bytes_per_second_send);
  status_callback(last_status);
  return 0;
}


// libcurl global init/fini
struct _curl_static_init_t {
  _curl_static_init_t() { curl_global_init(CURL_GLOBAL_ALL); }
  ~_curl_static_init_t() { curl_global_cleanup(); }
};
static _curl_static_init_t _curl_static_init;

