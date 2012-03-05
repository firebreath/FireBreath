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
set(IFBComJavascriptObject_GUID a7f89003-620a-56e0-aeea-ec0e8da18fb3)
set(FBComJavascriptObject_GUID 0861570f-19df-54f3-a7ed-4b6db6f943bc)
set(IFBComEventSource_GUID abc530a7-7627-5b5c-9285-f43d6c23c957)

# these are the pieces that are relevant to using it from Javascript
set(MOZILLA_PLUGINID "@firebreath.googlecode.com/FBTestPlugin")

# strings
set(FBSTRING_CompanyName "Firebreath Dev Team")
set(FBSTRING_PLUGIN_VERSION "1.0.0")
set(FBSTRING_LegalCopyright "Copyright 2009 Firebreath Dev Team")
set(FBSTRING_PluginFileName "np${PLUGIN_NAME}.dll")
set(FBSTRING_ProductName "FBTestPlugin")
set(FBSTRING_PluginName "FBTestPlugin")

#these should be lists if we are going to support multiple MIME types:
# x-fbtestplugin is main plugin, x-fbtestplugin-math is math object from main plugin, x-fbtestmathplugin is math plugin
set(FBSTRING_MIMEType
    "application/x-fbtestplugin"
    "application/x-fbtestplugin-math"
    "application/x-fbtestmathplugin"
    )
set(ACTIVEX_PROGID
    "FBDevTeam.FBTestPlugin"
    "FBDevTeam.FBTestPlugin_Math"
    "FBDevTeam.FBTestMathPlugin"
    )
set(FBControl_GUID
    9E506282-69D3-5ABA-9C1D-15994B37F4AC
    9E506282-69D3-5ABA-9C1D-15994B37F4AD
    9E506282-69D3-5ABA-9C1D-15994B37F4AE
    )
set(FBSTRING_FileExtents "")
set(FBSTRING_PluginDescription
    "Firebreath Test Plugin - Plugin for testing all interfaces and features"
    "Firebreath Test Plugin Math - Helper created from Main Plugin Object"
    "Firebreath Test Plugin SimpleMath - Helper created from Second Plugin Object"
    )

# If you want to register per-machine, uncomment this line
#set (FB_ATLREG_MACHINEWIDE 1)

#set (FB_GUI_DISABLED 1)

# mac settings
# The plugin doesn't do any drawing and doesn't need mouse / keyboard events,
# so turn the following off:
set (FBMAC_USE_CARBON                    0)
set (FBMAC_USE_COCOA                     0)
set (FBMAC_USE_QUICKDRAW                 0)
set (FBMAC_USE_COREGRAPHICS              0)
set (FBMAC_USE_COREANIMATION             0)
set (FBMAC_USE_INVALIDATINGCOREANIMATION 0)

add_firebreath_library(log4cplus)
