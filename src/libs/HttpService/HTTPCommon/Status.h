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
#ifndef H_HTTP_STATUS
#define H_HTTP_STATUS

namespace HTTP {

    struct Status {
        FB::VariantMap asDict() const;
        enum State {
            CANCELLED = -2,
            HTTP_ERROR = -1,
            IDLE = 0,
            RESOLVING = 1,
            CONNECTING = 2,
            SEND_REQUEST = 3,
            READ_RESPONSE = 4,
            COMPLETE = 5,
        };
        State state;
        size_t bytes_sent;
        size_t send_total;
        size_t bytes_received;
        size_t receive_total;
        double bytes_per_second_send;
        double bytes_per_second_receive;
        std::string last_error;

        Status() : state(IDLE), bytes_sent(0), send_total(0), bytes_received(0), receive_total(0),
            bytes_per_second_send(0), bytes_per_second_receive(0) {}
    };
};

#endif

