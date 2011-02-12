/**********************************************************\

  Auto-generated COM_config.h

\**********************************************************/

#define FBTYPELIB_NAME ${FBTYPELIB_NAME}
#define FBTYPELIB_DESC "${FBTYPELIB_DESC}"
#define FBTYPELIB_GUID ${FBTYPELIB_GUID}

#define IFBControl_GUID ${IFBControl_GUID}
#define IFBControl_DESC "${IFBControl_DESC}"

@@foreach (FBControl_GUID CUR_GUID ACTIVEX_PROGID PROGID)
#define FBControl${i}_GUID ${CUR_GUID}
#define ACTIVEX_PROGID${i} "${PROGID}"
@@endforeach
#define FBControl_GUID FBControl0_GUID
#define ACTIVEX_PROGID ACTIVEX_PROGID0
#define FBControl_DESC "${FBControl_DESC}"

#define IFBComJavascriptObject_GUID ${IFBComJavascriptObject_GUID}
#define IFBComJavascriptObject_DESC "${IFBComJavascriptObject_DESC}"

#define FBComJavascriptObject_GUID ${FBComJavascriptObject_GUID}
#define FBComJavascriptObject_DESC "${FBComJavascriptObject_DESC}"

#define IFBComEventSource_GUID ${IFBComEventSource_GUID}
#define IFBComEventSource_DESC "${IFBComEventSource_DESC}"

#define MOZILLA_PLUGINID "${MOZILLA_PLUGINID}"
#define VERSION_NUM "${AXVERSION_NUM}"

