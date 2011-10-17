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
        
        ////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @fn std::string FB::System::getHomeDirPath()
        ///
        /// @brief  Returns the current user's homedir
        ///
        /// @returns std::string
        /// @since 1.6rc2
        ////////////////////////////////////////////////////////////////////////////////////////////////////

        std::string getHomeDirPath();
        
        ////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @fn std::string FB::System::getAppDataPath(const std::string& appName)
        ///
        /// @brief  Returns a path to a directory suitable for storing application-specific data
        ///
        /// @returns std::string
        /// @since 1.6rc2
        ////////////////////////////////////////////////////////////////////////////////////////////////////
        
        std::string getAppDataPath(const std::string& appName);

        ////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @fn std::string FB::System::getLocalAppDataPath(const std::string& appName)
        ///
        /// @brief  Returns a path to a local directory suitable for storing application-specific data
        ///
        /// The directory returned by this function will be local. This can be useful in configurations where profile data resides remotely, e.g. in Windows Server domains.
        ///
        /// @returns std::string
        /// @since 1.6rc2
        ////////////////////////////////////////////////////////////////////////////////////////////////////

        std::string getLocalAppDataPath(const std::string& appName);

        ////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @fn std::string FB::System::getTempPath()
        ///
        /// @brief  Returns a path to a directory suitable for temporary files
        ///
        /// @returns std::string
        /// @since 1.6rc2
        ////////////////////////////////////////////////////////////////////////////////////////////////////

        std::string getTempPath();
    };
};
