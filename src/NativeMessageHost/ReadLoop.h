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

#pragma once
#ifndef ReadLoop_h__
#define ReadLoop_h__

#include <thread>
#include "MainLoop.h"

class ReadLoop
{
public:
    ReadLoop(MainLoop& main) : m_main(main) { }

    void start();
    void run();
    void join() {
        m_thread.join();
    }

private:
    std::thread m_thread;
    MainLoop& m_main;
};

#endif // ReadLoop_h__