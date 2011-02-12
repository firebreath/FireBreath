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

#include "APITypes.h"
#include "Status.h"

FB::VariantMap HTTP::Status::asDict() const {
    FB::VariantMap d;
    d["state"] = state;
    d["bytes_sent"] = bytes_sent;
    d["send_total"] = send_total;
    d["bytes_received"] = bytes_received;
    d["receive_total"] = receive_total;
    d["bytes_per_second_send"] = bytes_per_second_send;
    d["bytes_per_second_receive"] = bytes_per_second_receive;
    if (!last_error.empty()) d["error"] = last_error;
    return d;
}

