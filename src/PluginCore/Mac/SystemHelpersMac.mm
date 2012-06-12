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

#include "../SystemHelpers.h"
#import <Foundation/NSPathUtilities.h>
#import <Foundation/NSAutoreleasePool.h>
#import <Foundation/NSFileManager.h>

using std::string;

string FB::System::getHomeDirPath()
{
    NSAutoreleasePool * pool = [NSAutoreleasePool new];
    NSString* dir = NSHomeDirectory();
    string path([dir UTF8String]);
    [pool release];
    return path;
}

string FB::System::getAppDataPath(const string& appName)
{
    NSAutoreleasePool * pool = [NSAutoreleasePool new];
    // find the path to ~/Library/Application Support/
    NSArray* paths = NSSearchPathForDirectoriesInDomains(NSApplicationSupportDirectory, NSUserDomainMask, YES);
    NSString* strPath = [paths objectAtIndex:0];
    
    NSString* appComponent = [NSString stringWithUTF8String:appName.c_str()];
    
    strPath = [strPath stringByAppendingPathComponent:appComponent];
    
    string path([strPath UTF8String]);
    [pool release];
    return path;
}

string FB::System::getLocalAppDataPath(const string& appName)
{
    return FB::System::getAppDataPath(appName);
}

string FB::System::getTempPath()
{
    NSAutoreleasePool * pool = [NSAutoreleasePool new];
    NSString* dir = NSTemporaryDirectory();
    string path([dir UTF8String]);
    [pool release];
    return path;
}

