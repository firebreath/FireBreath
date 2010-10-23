#/**********************************************************\ 
# Auto-Generated Plugin Configuration file
# for FBTestPlugin
#\**********************************************************/

set(PLUGIN_NAME "FBTestPlugin")
set(PLUGIN_PREFIX "FBTP")
set(COMPANY_NAME "FBDevTeam")

# ActiveX constants:
set(FBTYPELIB_NAME FBTestPluginLib)
set(FBTYPELIB_DESC "FBTestPlugin 1.0 Type Library")
set(IFBControl_DESC "FBTestPlugin Control Interface")
set(FBControl_DESC "FBTestPlugin Control Class")
set(IFBComJavascriptObject_DESC "FBTestPlugin IComJavascriptObject Interface")
set(FBComJavascriptObject_DESC "FBTestPlugin ComJavascriptObject Class")
set(IFBComEventSource_DESC "FBTestPlugin IFBComEventSource Interface")
set(AXVERSION_NUM "1")

# NOTE: THESE GUIDS *MUST* BE UNIQUE TO YOUR PLUGIN/ACTIVEX CONTROL!  YES, ALL OF THEM!
set(FBTYPELIB_GUID 43a2a216-7fea-549e-b063-e490e5e1a49a)
set(IFBControl_GUID 027bcc17-6e67-5e3f-afea-979e8572ae93)
set(FBControl_GUID 9e506282-69d3-5aba-9c1d-15994b37f4ac)
set(IFBComJavascriptObject_GUID a7f89003-620a-56e0-aeea-ec0e8da18fb3)
set(FBComJavascriptObject_GUID 0861570f-19df-54f3-a7ed-4b6db6f943bc)
set(IFBComEventSource_GUID abc530a7-7627-5b5c-9285-f43d6c23c957)

# these are the pieces that are relevant to using it from Javascript
set(ACTIVEX_PROGID "FBDevTeam.FBTestPlugin")
set(MOZILLA_PLUGINID "@firebreath.googlecode.com/FBTestPlugin")

# strings
set(FBSTRING_CompanyName "Firebreath Dev Team")
set(FBSTRING_FileDescription "Firebreath Test Plugin - Plugin for testing all interfaces and features")
set(FBSTRING_PLUGIN_VERSION "1.0.0")
set(FBSTRING_LegalCopyright "Copyright 2009 Firebreath Dev Team")
set(FBSTRING_PluginFileName "np${PLUGIN_NAME}.dll")
set(FBSTRING_ProductName "FBTestPlugin")
set(FBSTRING_FileExtents "")
set(FBSTRING_PluginName "FBTestPlugin")
set(FBSTRING_MIMEType "application/x-fbtestplugin")

# mac settings
# The plugin doesn't do any drawing and doesn't need mouse / keyboard events,
# so turn the following off:
set (FBMAC_USE_CARBON        1)#0)
set (FBMAC_USE_COCOA         1)
set (FBMAC_USE_QUICKDRAW     1)#0)
set (FBMAC_USE_COREGRAPHICS  1)
set (FBMAC_USE_COREANIMATION 1)#0)

add_firebreath_library(log4cplus)
