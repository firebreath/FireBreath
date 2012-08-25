#/**********************************************************\ 
#
# Auto-Generated Plugin Configuration file
# for DrawingTest
#
#\**********************************************************/

set(PLUGIN_NAME "DrawingTest")
set(PLUGIN_PREFIX "DTE")
set(COMPANY_NAME "FireBreath")

# ActiveX constants:
set(FBTYPELIB_NAME DrawingTestLib)
set(FBTYPELIB_DESC "DrawingTest 1.0 Type Library")
set(IFBControl_DESC "DrawingTest Control Interface")
set(FBControl_DESC "DrawingTest Control Class")
set(IFBComJavascriptObject_DESC "DrawingTest IComJavascriptObject Interface")
set(FBComJavascriptObject_DESC "DrawingTest ComJavascriptObject Class")
set(IFBComEventSource_DESC "DrawingTest IFBComEventSource Interface")
set(AXVERSION_NUM "1")

# NOTE: THESE GUIDS *MUST* BE UNIQUE TO YOUR PLUGIN/ACTIVEX CONTROL!  YES, ALL OF THEM!
set(FBTYPELIB_GUID 6ec1d40b-f998-5a9d-8aa4-c68d9356f5a8)
set(IFBControl_GUID cf6c0ddf-fb2d-53b8-8289-b4596132df4e)
set(FBControl_GUID 1e052f58-2cd8-571d-88ea-e86c8cf23f35)
set(IFBComJavascriptObject_GUID 73a3f0fb-37ad-5394-94c9-0145a07d838e)
set(FBComJavascriptObject_GUID 9a7cb249-2399-5db5-b492-186f32982bf5)
set(IFBComEventSource_GUID dd855e64-b302-5936-9177-be90ca02ff81)

# these are the pieces that are relevant to using it from Javascript
set(ACTIVEX_PROGID "FireBreath.DrawingTest")
set(MOZILLA_PLUGINID "firebreath.com/DrawingTest")

# strings
set(FBSTRING_CompanyName "FireBreath")
set(FBSTRING_PluginDescription "Example that demonstrates cross platform drawing")
set(FBSTRING_PLUGIN_VERSION "1.0.0.0")
set(FBSTRING_LegalCopyright "Copyright 2012 Firebreath development team")
set(FBSTRING_PluginFileName "np${PLUGIN_NAME}.dll")
set(FBSTRING_ProductName "DrawingTest")
set(FBSTRING_FileExtents "")
set(FBSTRING_PluginName "DrawingTest")
set(FBSTRING_MIMEType "application/x-drawingtest")

# Uncomment this next line if you're not planning on your plugin doing
# any drawing:

#set (FB_GUI_DISABLED 1)

# Mac plugin settings. If your plugin does not draw, set these all to 0
set(FBMAC_USE_QUICKDRAW 0)
set(FBMAC_USE_CARBON 1)
set(FBMAC_USE_COCOA 1)
set(FBMAC_USE_COREGRAPHICS 1)
set(FBMAC_USE_COREANIMATION 0)
set(FBMAC_USE_INVALIDATINGCOREANIMATION 0)

# If you want to register per-machine on Windows, uncomment this line
#set (FB_ATLREG_MACHINEWIDE 1)
