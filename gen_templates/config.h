
/**********************************************************\

  Auto-generated config.h

\**********************************************************/

#define FBSTRING_CompanyName "${FBSTRING_CompanyName}"
#define FBSTRING_FileDescription "${FBSTRING_FileDescription}"
#define FBSTRING_PLUGIN_VERSION "${FBSTRING_PLUGIN_VERSION}"
#define FBSTRING_LegalCopyright "${FBSTRING_LegalCopyright}"
#define FBSTRING_PluginFileName "${FBSTRING_PluginFileName}"
#define FBSTRING_ProductName "${FBSTRING_ProductName}"
#define FBSTRING_FileExtents "${FBSTRING_FileExtents}"
#define FBSTRING_PluginName "${FBSTRING_PluginName}"
#define FBSTRING_MIMEType "${FBSTRING_MIMEType}"
#define FBSTRING_X11MIMEType "${FBSTRING_MIMEType}:${FBSTRING_FileExtents}:${FBSTRING_FileDescription}"

#define FB_GUI_DISABLED         ${FB_GUI_DISABLED}

#if FB_GUI_DISABLED == 1

#define FBMAC_USE_CARBON        0
#define FBMAC_USE_COCOA         0
#define FBMAC_USE_QUICKDRAW     0
#define FBMAC_USE_COREGRAPHICS  0
#define FBMAC_USE_COREANIMATION 0

#define FBWIN_WINDOWLESS        0

#else

#define FBMAC_USE_CARBON        ${FBMAC_USE_CARBON}
#define FBMAC_USE_COCOA         ${FBMAC_USE_COCOA}
#define FBMAC_USE_QUICKDRAW     ${FBMAC_USE_QUICKDRAW}
#define FBMAC_USE_COREGRAPHICS  ${FBMAC_USE_COREGRAPHICS}
#define FBMAC_USE_COREANIMATION ${FBMAC_USE_COREANIMATION}

#define FBWIN_WINDOWLESS ${FBWIN_WINDOWLESS}

#endif
