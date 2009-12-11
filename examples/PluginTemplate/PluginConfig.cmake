#/**********************************************************\ 
#Original Author: Richard Bateman (taxilian)
#
#Created:    Nov 20, 2009
#License:    Dual license model; choose one of two:
#            Eclipse Public License - Version 1.0
#            http://www.eclipse.org/legal/epl-v10.html
#            - or -
#            GNU Lesser General Public License, version 2.1
#            http://www.gnu.org/licenses/lgpl-2.1.html
#            
#Copyright 2009 PacketPass, Inc and the Firebreath development team
#\**********************************************************/

set(PLUGIN_NAME "PluginTemplate")
set(PLUGIN_PREFIX "TPL")

if (NOT GEN_DIR)
    get_filename_component (GEN_DIR "${CMAKE_CURRENT_BINARY_DIR}/../gen" ABSOLUTE)
    message("Found plugin config at ${GEN_DIR}")
endif()

# ActiveX constants:
set(FBTYPELIB_NAME FireBreathWinLib)
set(FBTYPELIB_DESC "TemplatePlugin 1.0 Type Library")
set(IFBControl_DESC "TemplatePlugin FBControl Interface")
set(FBControl_DESC "TemplatePlugin FBControl Class")
set(IFBComJavascriptObject_DESC "TemplatePlugin IComJavascriptObject Interface")
set(FBComJavascriptObject_DESC "TemplatePlugin ComJavascriptObject Class")
set(IFBComEventSource_DESC "TemplatePlugin IFBComEventSource Interface")
set(AXVERSION_NUM "1")

# NOTE: THESE GUIDS *MUST* BE UNIQUE TO YOUR PLUGIN/ACTIVEX CONTROL!  YES, ALL OF THEM!
set(FBTYPELIB_GUID 0C20AAD9-E132-4a31-9C73-C517F58CB735)
set(IFBControl_GUID 4874B73A-7753-4216-BDB2-89C9F0F8CC31)
set(FBControl_GUID F0E7F20B-9ED2-4f25-8804-9152EA9C58CD)
set(IFBComJavascriptObject_GUID 8E6C3ADA-BFEF-43c2-9C7F-99D93DF11A48)
set(FBComJavascriptObject_GUID FE6A7645-750D-4768-B790-50AFE5709CDC)
set(IFBComEventSource_GUID BF0E93BA-B8DF-4f3d-B28F-F37E7C041917)

# these are the pieces that are relevant to using it from Javascript
set(ACTIVEX_PROGID "FireBreath.TemplatePlugin")
set(MOZILLA_PLUGINID "@firebreath.googlecode.com/Firebreath Template Plugin")

# strings
set(FBSTRING_CompanyName "No particular company")
set(FBSTRING_FileDescription "Plugin template for the FireBreath Browser Plugin Project")
set(FBSTRING_PLUGIN_VERSION "1.0.0")
set(FBSTRING_LegalCopyright "Copyright 2009 Firebreath development team, PacketPass Inc")
set(FBSTRING_PluginFileName "npFireBreath.dll")
set(FBSTRING_ProductName "FireBreath Template Plugin")
set(FBSTRING_FileExtents "")
set(FBSTRING_PluginName "FireBreath Template Plugin")
set(FBSTRING_MIMEType "application/x-vnd.FirebreathTemplatePlugin")

