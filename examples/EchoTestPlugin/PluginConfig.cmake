#/**********************************************************\ 
#
# Auto-Generated Plugin Configuration file
# for Echo Test Plugin
#
#\**********************************************************/

set(PLUGIN_NAME "EchoTestPlugin")
set(PLUGIN_PREFIX "ETP")
set(COMPANY_NAME "FBDevTeam")
set(COMPANY_REVDOMAIN "org.firebreath")

# NOTE: If you are going to deploy your own extension you *must* change the
#       key and ID to your own key and matching ID.  You can use cmake/makeid.py
#       (https://gist.github.com/taxilian/d42b8768b2e3ef5c6998) to generate them
set(PLUGIN_CRX_NAME "${PLUGIN_NAME} Plugin Adapter")
set(PLUGIN_CRX_KEY "MIGfMA0GCSqGSIb3DQEBAQUAA4GNADCBiQKBgQDOqMBmB8ThMqFT3/9s/bfShwIXIlksxCwmo/VJdLScKqCc28xU2WaJmjfV4qa1K9K3SRiDTwpk53alWv/xoRXLjDnnvxmdu9AD57o7DEQAfdSRAv0grVt00/o7oudFaRlyvrHMmvjAM4fTnA3URQfEKz2L2wb/jfJt0ogcnYkWswIDAQAB")
set(PLUGIN_CRX_ID "dlfdfkalmjbghacnadegnkmbcnpmnmfn")
set(PLUGIN_CRX_WYRMHOLE_NAME ${COMPANY_NAME})
set(PLUGIN_CRX_NATIVEHOST_NAME "${COMPANY_REVDOMAIN}.firewyrmhost")
set(PLUGIN_CRX_NATIVEHOST_DESCRIPTION "FireBreath FireWyrm Native Messaging Wyrmhole")

# ActiveX constants:
set(FBTYPELIB_NAME EchoTestPluginLib)
set(FBTYPELIB_DESC "EchoTestPlugin 1.0 Type Library")
set(IFBControl_DESC "EchoTestPlugin Control Interface")
set(FBControl_DESC "EchoTestPlugin Control Class")
set(IFBComJavascriptObject_DESC "EchoTestPlugin IComJavascriptObject Interface")
set(FBComJavascriptObject_DESC "EchoTestPlugin ComJavascriptObject Class")
set(IFBComEventSource_DESC "EchoTestPlugin IFBComEventSource Interface")
set(AXVERSION_NUM "1")

# NOTE: THESE GUIDS *MUST* BE UNIQUE TO YOUR PLUGIN/ACTIVEX CONTROL!  YES, ALL OF THEM!
set(FBTYPELIB_GUID 41fa03a7-5d88-5eaf-a1e8-25175998ec23)
set(IFBControl_GUID 6a8473ed-a1f0-5da2-ba98-88e8316cd12a)
set(FBControl_GUID fbc31335-da8d-5648-8b52-4ccc26dddda5)
set(IFBComJavascriptObject_GUID 32a77996-1311-5559-adbe-0355950a4112)
set(FBComJavascriptObject_GUID bca1cd7f-3120-5be8-bc62-14124ec5c669)
set(IFBComEventSource_GUID ff4c5578-b485-5b47-b2a4-d88adaf336f9)
if ( FB_PLATFORM_ARCH_32 )
    set(FBControl_WixUpgradeCode_GUID d4da592d-db85-5e1a-a91a-20bb688de17e)
else ( FB_PLATFORM_ARCH_32 )
    set(FBControl_WixUpgradeCode_GUID 5b607016-4908-5dad-98e5-d80125858c0d)
endif ( FB_PLATFORM_ARCH_32 )

# these are the pieces that are relevant to using it from Javascript
set(ACTIVEX_PROGID "FBDevTeam.EchoTestPlugin")
if ( FB_PLATFORM_ARCH_32 )
    set(MOZILLA_PLUGINID "firebreath.org/EchoTestPlugin")  # No 32bit postfix to maintain backward compatability.
else ( FB_PLATFORM_ARCH_32 )
    set(MOZILLA_PLUGINID "firebreath.org/EchoTestPlugin_${FB_PLATFORM_ARCH_NAME}")
endif ( FB_PLATFORM_ARCH_32 )

# strings
set(FBSTRING_CompanyName "FireBreath Development Team")
set(FBSTRING_PluginDescription "Echo test plugin for unit testish thingies")
set(FBSTRING_PLUGIN_VERSION "1.0.0.0")
set(FBSTRING_LegalCopyright "Copyright 2015 FireBreath Development Team")
set(FBSTRING_PluginFileName "np${PLUGIN_NAME}")
if (APPLE)
    # On apple, np is not needed
    set(FBSTRING_PluginFileName "${PLUGIN_NAME}")
endif()
set(FBSTRING_ProductName "Echo Test Plugin")
set(FBSTRING_FileExtents "")
if ( FB_PLATFORM_ARCH_32 )
    set(FBSTRING_PluginName "Echo Test Plugin")  # No 32bit postfix to maintain backward compatability.
else ( FB_PLATFORM_ARCH_32 )
    set(FBSTRING_PluginName "Echo Test Plugin_${FB_PLATFORM_ARCH_NAME}")
endif ( FB_PLATFORM_ARCH_32 )
set(FBSTRING_MIMEType "application/x-echotestplugin")

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
