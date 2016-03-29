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

bool canRead() {
    return !feof(stdin) && !ferror(stdin);
}


void ReadLoop::run() {
    // Begin read loop thread
	log("Starting read loop");
    
    uint32_t len = 0;

    while (canRead()) {
        // First read the 4 byte length of the input message
        // TODO: Use fread here
        size_t res = fread((char*) &len, sizeof(len), 1, stdin);

        // Allocate a string that is long enough
        char *inString = new char[len];
        res = fread(&inString[0], sizeof(char), len, stdin);
        std::string inMessage(inString, len);
        if (res == 0) {
            // Exit signaled
            m_main.shutdown();
            return;
        } else if (res != len) {
            // Note: "Ack!" is a technical term
			std::ostringstream oss;
			oss << "Ack! Expected to read " << len << " bytes but instead read " << res << " bytes.";
			log(oss.str());
        } else if (!canRead()) {
            // There was an error reading; time to bail
			log("Could not read from stdin! Bailing");
        } else {
            //log(inMessage);
            m_main.messageIn(inMessage);
        }
        delete[] inString;
    }

	log("Read loop ending");
    // End thread
}
