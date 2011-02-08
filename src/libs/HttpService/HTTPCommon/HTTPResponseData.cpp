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

#include "HTTPResponseData.h"

using namespace HTTP;

HTTPDatablock* HTTPResponseData::coalesceBlocks() {
  if (data.empty()) {
    data.push_back(new HTTPStringDatablock());
  } else if (data.size() != 1) {
    std::string result;
    for (std::list<HTTPDatablock*>::iterator it = data.begin(); it != data.end(); ++it) {
      result.append((*it)->data(), (*it)->size());
      delete *it;
    }
    data.clear();
    data.push_back(new HTTPStringDatablock(result));
  }
  return *data.begin();
}

HTTPResponseData::~HTTPResponseData() {
    for (std::list<HTTPDatablock*>::iterator it = data.begin(); it != data.end(); ++it) {
        delete *it;
    }
}

void HTTPResponseData::setNoncacheable() {
  headers.insert(std::make_pair("Expires", "Sat, 26 Jul 1997 05:00:00 GMT"));
  headers.insert(std::make_pair("Cache-Control", "private, no-store, no-cache, must-revalidate, post-check=0, pre-check=0"));
  headers.insert(std::make_pair("Pragma", "no-cache"));
}

