#/**********************************************************\ 
#Original Author: Richard Bateman (taxilian)
#
#Created:    Nov 20, 2009
#License:    Dual license model; choose one of two:
#            New BSD License
#            http://www.opensource.org/licenses/bsd-license.php
#            - or -
#            GNU Lesser General Public License, version 2.1
#            http://www.gnu.org/licenses/lgpl-2.1.html
#            
#Copyright 2009 PacketPass, Inc and the Firebreath development team
#\**********************************************************/

set(PLUGIN_NAME "BasicMediaPlayer")
set(PLUGIN_PREFIX "BasicMediaPlayer")
set(COMPANY_NAME "FBDevTeam")

# ActiveX constants:
set(FBTYPELIB_NAME FireBreathWinLib)
set(FBTYPELIB_DESC "BasicMediaPlayer 1.0 Type Library")
set(IFBControl_DESC "BasicMediaPlayer FBControl Interface")
set(FBControl_DESC "BasicMediaPlayer FBControl Class")
set(IFBComJavascriptObject_DESC "BasicMediaPlayer IComJavascriptObject Interface")
set(FBComJavascriptObject_DESC "BasicMediaPlayer ComJavascriptObject Class")
set(IFBComEventSource_DESC "BasicMediaPlayer IFBComEventSource Interface")
set(AXVERSION_NUM "1")

# NOTE: THESE GUIDS *MUST* BE UNIQUE TO YOUR PLUGIN/ACTIVEX CONTROL!  YES, ALL OF THEM!
set(FBTYPELIB_GUID 967C9EC7-8E2F-405d-8492-C903DDCEABB1)
set(IFBControl_GUID F54F82DB-DCE0-4a35-AB9E-43A1BE7DD721)
set(FBControl_GUID E4C14033-C0B7-45ef-B5F7-66621858DA10)
set(IFBComJavascriptObject_GUID 67244427-D855-44b5-8213-2433A05A9585)
set(FBComJavascriptObject_GUID 5C244822-4155-4e60-AA1C-DD53EF191915)
set(IFBComEventSource_GUID 77D5AF47-72BF-4015-AA3B-03FF24D59447)

# these are the pieces that are relevant to using it from Javascript
set(ACTIVEX_PROGID "FireBreath.BasicMediaPlayer")
set(MOZILLA_PLUGINID "@firebreath.googlecode.com/Firebreath BasicMediaPlayer Plugin")

# strings
set(FBSTRING_CompanyName "FireBreath")
set(FBSTRING_PluginDescription "BasicMediaPlayer Plugin for the FireBreath Project")
set(FBSTRING_PLUGIN_VERSION "1.0.0")
set(FBSTRING_LegalCopyright "Copyright 2009 Firebreath development team")
set(FBSTRING_PluginFileName "npBasicMediaPlayer.dll")
set(FBSTRING_ProductName "FireBreath BasicMediaPlayer Plugin")
set(FBSTRING_FileExtents "")
set(FBSTRING_PluginName "FireBreath BasicMediaPlayer Plugin")
set(FBSTRING_MIMEType "application/x-vnd.FBBasicMediaPlayer")

# mac settings
set (FBMAC_USE_CARBON                    1)
set (FBMAC_USE_COCOA                     1)
set (FBMAC_USE_QUICKDRAW                 1)
set (FBMAC_USE_COREGRAPHICS              1)
set (FBMAC_USE_COREANIMATION             1)
set (FBMAC_USE_INVALIDATINGCOREANIMATION 1)

#set (FBWIN_WINDOWLESS 0)

add_firebreath_library(log4cplus)
