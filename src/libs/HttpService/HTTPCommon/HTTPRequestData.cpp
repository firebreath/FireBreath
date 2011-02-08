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

#include "HTTPRequestData.h"

using namespace HTTP;

HTTPRequestData::HTTPRequestData(const FB::URI& in_uri, const std::string& in_method) : uri(in_uri), method(in_method) {
  if (method.empty()) method = uri.query_data.size() ? "POST" : "GET";
}

HTTPRequestData::~HTTPRequestData() {
  for (std::map<std::string, HTTPFileEntry>::iterator it = files.begin(); it != files.end(); ++it) {
    delete it->second.contents;
  }
}

void HTTPRequestData::addFile(const std::string& fieldname, const std::string& filename, const std::string& content_type, HTTPDatablock* contents) {
  HTTPFileEntry fe;
  fe.filename = filename;
  fe.content_type = content_type;
  fe.contents = contents;
  files[fieldname] = fe;
}

