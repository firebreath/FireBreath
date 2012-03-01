
/**********************************************************\

  Unit test config.h

\**********************************************************/

#define FBSTRING_CompanyName "FireBreath Developers"
#define FBSTRING_PLUGIN_VERSION "1.0"
#define FBSTRING_LegalCopyright "Copyright 2011"
#define FBSTRING_PluginFileName "npNoPluginFilename.dll"
#define FBSTRING_ProductName "UnitTests"
#define FBSTRING_PluginName "SomePluginName"

#define FBSTRING_MIMETypeList \
                              "application/x-nothing|" \
                              ""

#define FBSTRING_MIMEType0 "application/x-nothing"
#define FBSTRING_X11MIMEType0 "application/x-nothing::Some plugin test"
#define FBSTRING_FileDescription0 "Some plugin test"
#define FBSTRING_PluginDescription0 "Some plugin test"
#define FBSTRING_FileExtents0 ""
#define FBSTRING_PluginDescription FBSTRING_FileDescription0
#define FBSTRING_FileDescription FBSTRING_FileDescription0
#define FBSTRING_FileExtents FBSTRING_FileExtents0

#define FBSTRING_X11MIMEType \
                            "application/x-nothing::Some plugin test;" \
                            ""

#define FBSTRING_MIMEType   FBSTRING_MIMETypeList

#define FB_GUI_DISABLED         1

#if FB_GUI_DISABLED == 1

#define FBMAC_USE_CARBON                    0
#define FBMAC_USE_COCOA                     0
#define FBMAC_USE_QUICKDRAW                 0
#define FBMAC_USE_COREGRAPHICS              0
#define FBMAC_USE_COREANIMATION             0
#define FBMAC_USE_INVALIDATINGCOREANIMATION 0

#else

#define FBMAC_USE_CARBON                    0
#define FBMAC_USE_COCOA                     0
#define FBMAC_USE_QUICKDRAW                 0
#define FBMAC_USE_COREGRAPHICS              0
#define FBMAC_USE_COREANIMATION             0
#define FBMAC_USE_INVALIDATINGCOREANIMATION 0

#endif

