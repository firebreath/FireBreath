
/**********************************************************\

  Auto-generated config.h

\**********************************************************/

#define FBSTRING_CompanyName "${FBSTRING_CompanyName}"
#define FBSTRING_PLUGIN_VERSION "${FBSTRING_PLUGIN_VERSION}"
#define FBSTRING_LegalCopyright "${FBSTRING_LegalCopyright}"
#define FBSTRING_PluginFileName "${FBSTRING_PluginFileName}.dll"
#define FBSTRING_ProductName "${FBSTRING_ProductName}"
#define FBSTRING_PluginName "${FBSTRING_PluginName}"

// Format is Major.Minor.Patch.Build

#define FBSTRING_VERSION_MAJOR ${FBSTRING_VERSION_MAJOR}
#define FBSTRING_VERSION_MINOR ${FBSTRING_VERSION_MINOR}
#define FBSTRING_VERSION_PATCH ${FBSTRING_VERSION_PATCH}
#define FBSTRING_VERSION_BUILD ${FBSTRING_VERSION_BUILD}

#define FBSTRiNG_MIMETypeCombined

#define FBSTRING_MIMETypeList "${FBMIMETYPE_LIST}"

@@foreach (FBSTRING_MIMEType CURMIME FBSTRING_FileExtents CUREXTENT FBSTRING_PluginDescription CURDESC)
#define FBSTRING_MIMEType${i} "${CURMIME}"
#define FBSTRING_X11MIMEType${i} "${CURMIME}:${CUREXTENT}:${CURDESC}"
#define FBSTRING_PluginDescription${i} "${CURDESC}"
#define FBSTRING_FileDescription${i} "${CURDESC}"
#define FBSTRING_FileExtents${i} "${CUREXTENT}"
@@endforeach
#define FBSTRING_PluginDescription FBSTRING_PluginDescription0
#define FBSTRING_FileDescription FBSTRING_FileDescription0
#define FBSTRING_FileExtents FBSTRING_FileExtents0

#define FBSTRING_X11MIMEType \
@@foreach (FBSTRING_MIMEType CURMIME FBSTRING_FileExtents CUREXTENT FBSTRING_PluginDescription CURDESC)
                            "${CURMIME}:${CUREXTENT}:${CURDESC};" \
@@endforeach
                            ""

#define FBSTRING_MIMEType   FBSTRING_MIMETypeList

#define FB_GUI_DISABLED         ${FB_GUI_DISABLED}

#cmakedefine FB_ATLREG_MACHINEWIDE 1

#if FB_GUI_DISABLED == 1

#define FBMAC_USE_CARBON                    0
#define FBMAC_USE_COCOA                     1
#define FBMAC_USE_QUICKDRAW                 0
#define FBMAC_USE_COREGRAPHICS              1
#define FBMAC_USE_COREANIMATION             0
#define FBMAC_USE_INVALIDATINGCOREANIMATION 0

#else

#define FBMAC_USE_CARBON                    ${FBMAC_USE_CARBON}
#define FBMAC_USE_COCOA                     ${FBMAC_USE_COCOA}
#define FBMAC_USE_QUICKDRAW                 ${FBMAC_USE_QUICKDRAW}
#define FBMAC_USE_COREGRAPHICS              ${FBMAC_USE_COREGRAPHICS}
#define FBMAC_USE_COREANIMATION             ${FBMAC_USE_COREANIMATION}
#define FBMAC_USE_INVALIDATINGCOREANIMATION ${FBMAC_USE_INVALIDATINGCOREANIMATION}

#endif

#define FB_

