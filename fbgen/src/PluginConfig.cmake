#/**********************************************************\ 
#
# Auto-Generated Plugin Configuration file
# for @{PLUGIN_name}
#
#\**********************************************************/

set(PLUGIN_NAME "@{PLUGIN_ident}")
set(PLUGIN_PREFIX "@{PLUGIN_prefix}")
set(COMPANY_NAME "@{COMPANY_ident}")

# ActiveX constants:
set(FBTYPELIB_NAME @{PLUGIN_ident}Lib)
set(FBTYPELIB_DESC "@{PLUGIN_ident} 1.0 Type Library")
set(IFBControl_DESC "@{PLUGIN_ident} Control Interface")
set(FBControl_DESC "@{PLUGIN_ident} Control Class")
set(IFBComJavascriptObject_DESC "@{PLUGIN_ident} IComJavascriptObject Interface")
set(FBComJavascriptObject_DESC "@{PLUGIN_ident} ComJavascriptObject Class")
set(IFBComEventSource_DESC "@{PLUGIN_ident} IFBComEventSource Interface")
set(AXVERSION_NUM "1")

# NOTE: THESE GUIDS *MUST* BE UNIQUE TO YOUR PLUGIN/ACTIVEX CONTROL!  YES, ALL OF THEM!
set(FBTYPELIB_GUID @{GUIDS_TYPELIB})
set(IFBControl_GUID @{GUIDS_CONTROLIF})
set(FBControl_GUID @{GUIDS_CONTROL})
set(IFBComJavascriptObject_GUID @{GUIDS_JSIF})
set(FBComJavascriptObject_GUID @{GUIDS_JSOBJ})
set(IFBComEventSource_GUID @{GUIDS_EVTSRC})
if ( FB_PLATFORM_ARCH_32 )
    set(FBControl_WixUpgradeCode_GUID @{GUIDS_INSTUPGR})
else ( FB_PLATFORM_ARCH_32 )
    set(FBControl_WixUpgradeCode_GUID @{GUIDS_INSTUPGR64})
endif ( FB_PLATFORM_ARCH_32 )

# these are the pieces that are relevant to using it from Javascript
set(ACTIVEX_PROGID "@{COMPANY_ident}.@{PLUGIN_ident}")
if ( FB_PLATFORM_ARCH_32 )
    set(MOZILLA_PLUGINID "@{COMPANY_domain}/@{PLUGIN_ident}")  # No 32bit postfix to maintain backward compatability.
else ( FB_PLATFORM_ARCH_32 )
    set(MOZILLA_PLUGINID "@{COMPANY_domain}/@{PLUGIN_ident}_${FB_PLATFORM_ARCH_NAME}")
endif ( FB_PLATFORM_ARCH_32 )

# strings
set(FBSTRING_CompanyName "@{COMPANY_name}")
set(FBSTRING_PluginDescription "@{PLUGIN_desc}")
set(FBSTRING_PLUGIN_VERSION "1.0.0.0")
set(FBSTRING_LegalCopyright "Copyright @{YEAR} @{COMPANY_name}")
set(FBSTRING_PluginFileName "np${PLUGIN_NAME}")
set(FBSTRING_ProductName "@{PLUGIN_name}")
set(FBSTRING_FileExtents "")
if ( FB_PLATFORM_ARCH_32 )
    set(FBSTRING_PluginName "@{PLUGIN_name}")  # No 32bit postfix to maintain backward compatability.
else ( FB_PLATFORM_ARCH_32 )
    set(FBSTRING_PluginName "@{PLUGIN_name}_${FB_PLATFORM_ARCH_NAME}")
endif ( FB_PLATFORM_ARCH_32 )
set(FBSTRING_MIMEType "@{PLUGIN_mimetype}")

# Uncomment this next line if you're not planning on your plugin doing
# any drawing:

@{PLUGIN_disable_gui} 1)

# Mac plugin settings. If your plugin does not draw, set these all to 0
set(FBMAC_USE_QUICKDRAW 0)
set(FBMAC_USE_CARBON @{PLUGIN_disable_gui_mac})
set(FBMAC_USE_COCOA @{PLUGIN_disable_gui_mac})
set(FBMAC_USE_COREGRAPHICS @{PLUGIN_disable_gui_mac})
set(FBMAC_USE_COREANIMATION 0)
set(FBMAC_USE_INVALIDATINGCOREANIMATION 0)

# If you want to register per-machine on Windows, uncomment this line
#set (FB_ATLREG_MACHINEWIDE 1)
