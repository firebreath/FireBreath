#/**********************************************************\ 
#
# Auto-Generated Plugin Configuration file
# for {{PLUGIN_NAME}}
#
#\**********************************************************/

set(PLUGIN_NAME "{{PLUGIN_ID}}")
set(PLUGIN_PREFIX "{{PLUGIN_PREFIX}}")

# ActiveX constants:
set(FBTYPELIB_NAME {{PLUGIN_ID}}Lib)
set(FBTYPELIB_DESC "{{PLUGIN_ID}} 1.0 Type Library")
set(IFBControl_DESC "{{PLUGIN_ID}} Control Interface")
set(FBControl_DESC "{{PLUGIN_ID}} Control Class")
set(IFBComJavascriptObject_DESC "{{PLUGIN_ID}} IComJavascriptObject Interface")
set(FBComJavascriptObject_DESC "{{PLUGIN_ID}} ComJavascriptObject Class")
set(IFBComEventSource_DESC "{{PLUGIN_ID}} IFBComEventSource Interface")
set(AXVERSION_NUM "1")

# NOTE: THESE GUIDS *MUST* BE UNIQUE TO YOUR PLUGIN/ACTIVEX CONTROL!  YES, ALL OF THEM!
set(FBTYPELIB_GUID {{GUID_TYPELIB}})
set(IFBControl_GUID {{GUID_CONTROLIF}})
set(FBControl_GUID {{GUID_CONTROL}})
set(IFBComJavascriptObject_GUID {{GUID_JSIF}})
set(FBComJavascriptObject_GUID {{GUID_JSOBJ}})
set(IFBComEventSource_GUID {{GUID_EVTSRC}})

# these are the pieces that are relevant to using it from Javascript
set(ACTIVEX_PROGID "{{COMPANY_ID}.{{PLUGIN_ID}}")
set(MOZILLA_PLUGINID "@{{COMPANY_DOMAIN}/{{PLUGIN_NAME}}")

# strings
set(FBSTRING_CompanyName "{{COMPANY_NAME}}")
set(FBSTRING_FileDescription "{{PLUGIN_DESC}}")
set(FBSTRING_PLUGIN_VERSION "1.0.0")
set(FBSTRING_LegalCopyright "Copyright {{YEAR}} {{COMPANY_NAME}}")
set(FBSTRING_PluginFileName "np${PLUGIN_NAME}.dll")
set(FBSTRING_ProductName "{{PLUGIN_NAME}")
set(FBSTRING_FileExtents "")
set(FBSTRING_PluginName "{{PLUGIN_NAME}}")
set(FBSTRING_MIMEType "{{MIMETYPE}}")

