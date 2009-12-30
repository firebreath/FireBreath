#/**********************************************************\ 
#
# Auto-Generated Plugin Configuration file
# for @{PLUGIN.name}
#
#\**********************************************************/

set(PLUGIN_NAME "@{PLUGIN.ident}")
set(PLUGIN_PREFIX "@{PLUGIN.prefix}")
set(COMPANY_NAME "@{COMPANY.ident}")

# ActiveX constants:
set(FBTYPELIB_NAME @{PLUGIN.ident}Lib)
set(FBTYPELIB_DESC "@{PLUGIN.ident} 1.0 Type Library")
set(IFBControl_DESC "@{PLUGIN.ident} Control Interface")
set(FBControl_DESC "@{PLUGIN.ident} Control Class")
set(IFBComJavascriptObject_DESC "@{PLUGIN.ident} IComJavascriptObject Interface")
set(FBComJavascriptObject_DESC "@{PLUGIN.ident} ComJavascriptObject Class")
set(IFBComEventSource_DESC "@{PLUGIN.ident} IFBComEventSource Interface")
set(AXVERSION_NUM "1")

# NOTE: THESE GUIDS *MUST* BE UNIQUE TO YOUR PLUGIN/ACTIVEX CONTROL!  YES, ALL OF THEM!
set(FBTYPELIB_GUID @{GUID.generate("TYPELIB")})
set(IFBControl_GUID @{GUID.generate("CONTROLIF")})
set(FBControl_GUID @{GUID.generate("CONTROL")})
set(IFBComJavascriptObject_GUID @{GUID.generate("JSIF")})
set(FBComJavascriptObject_GUID @{GUID.generate("JSOBJ")})
set(IFBComEventSource_GUID @{GUID.generate("EVTSRC")})

# these are the pieces that are relevant to using it from Javascript
set(ACTIVEX_PROGID "@{COMPANY.ident}.@{PLUGIN.ident}")
set(MOZILLA_PLUGINID "@{COMPANY.domain}/@{PLUGIN.ident}")

# strings
set(FBSTRING_CompanyName "@{COMPANY.name}")
set(FBSTRING_FileDescription "@{PLUGIN.desc}")
set(FBSTRING_PLUGIN_VERSION "1.0.0")
<##import time ##>   
set(FBSTRING_LegalCopyright "Copyright @{time.strftime("%Y")} @{COMPANY.name}")
set(FBSTRING_PluginFileName "np${PLUGIN_NAME}.dll")
set(FBSTRING_ProductName "@{PLUGIN.name}")
set(FBSTRING_FileExtents "")
set(FBSTRING_PluginName "@{PLUGIN.name}")
set(FBSTRING_MIMEType "@{PLUGIN.mimetype}")

