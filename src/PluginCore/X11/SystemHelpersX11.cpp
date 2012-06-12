/**********************************************************\
 Original Author: Max Amanshauser

 Created:    Dec 02, 2011
 License:    Dual license model; choose one of two:
 New BSD License
 http://www.opensource.org/licenses/bsd-license.php
 - or -
 GNU Lesser General Public License, version 2.1
 http://www.gnu.org/licenses/lgpl-2.1.html

 Copyright 2011 Max Amanshauser, Firebreath development team
 \**********************************************************/

#include "../SystemHelpers.h"
#include "logging.h"
#include <sys/types.h>
#include <pwd.h>
#include <unistd.h>
#include <stdexcept>
#include <stdlib.h>

// Thread-safe
std::string FB::System::getHomeDirPath()
{
    int bufsize = sysconf(_SC_GETPW_R_SIZE_MAX);
    if(bufsize == -1)
        bufsize = 16384;        // Should be more than enough

    char* buf = (char*) malloc(bufsize);
    if (buf == NULL) {
        FBLOG_ERROR("SystemHelpersX11", "malloc failed.");
        throw new std::runtime_error("malloc failed.");
    }

    struct passwd pwd;
    struct passwd* result;

    int s = getpwuid_r(getuid(), &pwd, buf, bufsize, &result);
    if (result == NULL) {
        free(buf);

        if (s == 0) {
            FBLOG_ERROR("SystemHelpersX11", "User not found.");
            throw new std::runtime_error("User not found.");
        }else{
            std::string exs ("getpwuid_r error: ");
            exs += s;

            FBLOG_ERROR("SystemHelpersX11", exs);
            throw new std::runtime_error(exs);
        }
    }

    std::string x (result->pw_dir);
    free(buf);

    return x;
}

std::string FB::System::getAppDataPath(const std::string& appName)
{
    std::string s = getHomeDirPath();

    return s + "/." + appName;
}

std::string FB::System::getLocalAppDataPath(const std::string& appName)
{
    return getAppDataPath(appName);
}

std::string FB::System::getTempPath()
{
    char* c = getenv("TMPDIR");

    if(c == NULL)
        return std::string("/tmp");

    return std::string (c);
}
