/**********************************************************\
Original Author: Richard Bateman

Created:    Mar 17, 2015
License:    Dual license model; choose one of two:
            New BSD License
            http://www.opensource.org/licenses/bsd-license.php
            - or -
            GNU Lesser General Public License, version 2.1
            http://www.gnu.org/licenses/lgpl-2.1.html

Copyright 2015 GradeCam, Richard Bateman, and the
               Firebreath development team
\**********************************************************/

#include "ReadLoop.h"
#include <iostream>
#include <cstdio>

using std::cin;

void ReadLoop::start() {
    m_thread = std::thread(&ReadLoop::run, this);
}

void ReadLoop::run() {
    // Begin read loop thread
    std::cerr << "Starting read loop" << std::endl;
    
    uint32_t len = 0;

    while (cin.good()) {
        // First read the 4 byte length of the input message
        // TODO: Use fread here
        cin.read((char*) &len, sizeof(len));

        // Allocate a string that is long enough
        std::string inMessage;
        inMessage.reserve(len);
        // Read the message into the string; this is a bit of a hack, but if it works we're using it
        size_t res = fread(&inMessage[0], sizeof(char), len, stdin);
        if (res != len) {
            // Note: "Ack!" is a technical term
            std::cerr << "Ack! Expected to read " << len << " bytes but instead read " << res << " bytes." << std::endl;
        } else if (!cin.good()) {
            // There was an error reading; time to bail
            std::cerr << "Could not read from stdin! Bailing" << std::endl;
        } else {
            m_main.messageIn(inMessage);
        }
    }

    std::cerr << "Read loop ending" << std::endl;
    // End thread
}
