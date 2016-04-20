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

#include "MainLoop.h"
#include "ReadLoop.h"
#include "HostMain.h"
#if __linux && !FB_GUI_DISABLED
#include <gtk/gtk.h>
#endif
#ifdef WIN32
#include <intrin.h>
#include <io.h>
#include <fcntl.h>
#include <windows.h>
#endif

//#define DEBUG_BREAK

void log(std::string message)
{
#ifdef DEBUG
	std::cerr << message << std::endl;
#endif
}

int main(int argc, char* argv[]) {

#if __linux && !FB_GUI_DISABLED
    gtk_init (&argc, &argv);
#endif

#ifdef WIN32
#ifdef DEBUG_BREAK
    __debugbreak();
#endif
    _setmode(_fileno(stdout), _O_BINARY);
    _setmode(_fileno(stdin), _O_BINARY);
    _setmode(_fileno(stderr), _O_BINARY);
    SetStdHandle(STD_INPUT_HANDLE, NULL);
    SetStdHandle(STD_OUTPUT_HANDLE, NULL);
#endif
	log("Starting FireWyrm native message host");
    if (argv[1] != NULL) {
        MainLoop& mainLoop = MainLoop::get(argv[1]);
        ReadLoop reader(mainLoop);

        // Start the thread reading from cin
        reader.start();

        // Start the main program loop
        mainLoop.run();
        reader.join();

        // TODO: Join the reader thread
    } else {
		log("This application is intended to be run from Chrome as a Native Messaging extension");
    }
    return 0;
}
