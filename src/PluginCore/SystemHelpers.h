/**********************************************************\
Original Author: Richard Bateman (taxilian)

Created:    Sep 27, 2011
License:    Dual license model; choose one of two:
            New BSD License
            http://www.opensource.org/licenses/bsd-license.php
            - or -
            GNU Lesser General Public License, version 2.1
            http://www.gnu.org/licenses/lgpl-2.1.html

Copyright 2011 GradeCam, Inc and the Firebreath development team
\**********************************************************/

#include <string>

namespace FB {
    namespace System {
        std::string getHomeDirPath();
        std::string getLocalAppDataPath(const std::string& appName);
        std::string getAppDataPath(const std::string& appName);
        std::string getTempPath();
    };
};
