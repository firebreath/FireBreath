/**********************************************************\ 
Original Author: Richard Bateman (taxilian)

Created:    Oct 19, 2009
License:    Dual license model; choose one of two:
            New BSD License
            http://www.opensource.org/licenses/bsd-license.php
            - or -
            GNU Lesser General Public License, version 2.1
            http://www.gnu.org/licenses/lgpl-2.1.html

Copyright 2009 Richard Bateman, Firebreath development team
\**********************************************************/

#pragma once
#ifndef H_FactoryDefinitions
#define H_FactoryDefinitions

// It is unfortunate that we have to include all of these, but otherwise the shared_ptrs don't work real well
#include "PluginCore.h"

// These global functions *must* be defined in your main plugin project -- that's the one that
// actually builds into a DLL that everything links to.  These are used to determine what 
// JSAPI object is used as the root JSAPI, what type of Plugin object to use, etc, as well
// as providing a way for the plugin to find out such information as MIME type, plugin name, etc

////////////////////////////////////////////////////////////////////////////////////////////////////
/// @fn FB::PluginCore *_getMainPlugin()
///
/// @brief  Responsible for creating the main plugin (PluginCore-derived) object.  This is called
/// 		by the browser to get your base class
///
/// @return a ptr to the plugin
////////////////////////////////////////////////////////////////////////////////////////////////////
FB::PluginCore *_getMainPlugin();

////////////////////////////////////////////////////////////////////////////////////////////////////
/// @fn void GlobalPluginInitialize()
///
/// @brief  This global function will be called when the number of plugins active goes from 0
/// 		to 1.
/// 		
/// Note that this could happen without the library being unloaded, but will
/// not be called again until after GlobalPluginDeinitialize is called
////////////////////////////////////////////////////////////////////////////////////////////////////
void GlobalPluginInitialize();

////////////////////////////////////////////////////////////////////////////////////////////////////
/// @fn void GlobalPluginDeinitialize()
///
/// @brief  This global function will be called when the number of plugins active goes to 0.
/// 		
/// Note that there is no guarantee that the library will be unloaded after this occurs and before
/// another plugin is created, thus causing GlobalPluginInitialize to be called.
////////////////////////////////////////////////////////////////////////////////////////////////////
void GlobalPluginDeinitialize();

#endif