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
#include <iostream>

int main(int argc, char* argv[]) {

  if (argv[1] != NULL) {
    std::cout.sync_with_stdio(false);
    std::cin.sync_with_stdio(false);

    MainLoop& mainLoop = MainLoop::get(argv[1]);
    ReadLoop reader(mainLoop);

    // Start the thread reading from cin
    reader.start();

    // Start the main program loop
    mainLoop.run();

    // TODO: Join the reader thread
  } else {
      std::cerr << "This application is intended to be run from Chrome as a Native Messaging extension" << std::endl;
  }
  return 0;
}