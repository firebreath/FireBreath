/**********************************************************\ 
Original Author: Richard Bateman (taxilian)

Created:    Oct 19, 2009
License:    Eclipse Public License - Version 1.0
            http://www.eclipse.org/legal/epl-v10.html

Copyright 2009 Richard Bateman, Firebreath development team
\**********************************************************/

#ifndef H_FB_NPAPI_NPAPIPLUGIN
#define H_FB_NPAPI_NPAPIPLUGIN

#include "NpapiTypes.h"
#include "NpapiBrowserHost.h"
#include "NPJavascriptObject.h"
#include "BrowserPlugin.h"

namespace FB { namespace Npapi {

    class NpapiPlugin : public FB::BrowserPlugin
    {
    public:
        NpapiPlugin(NpapiBrowserHost *host);
        virtual ~NpapiPlugin(void);

        virtual void shutdown();

    protected:
        NPJavascriptObject *m_obj;
        AutoPtr<NpapiBrowserHost> m_npHost;
        NPObject *getScriptableObject();

    public:
        // These calls are proxied from the NpapiPluginModule to this object, and are
        // the NPP_ functions given to the browser; essentially, the entrypoints for the
        // plugin instance
        NPError SetWindow(NPWindow* window);
        NPError NewStream(NPMIMEType type, NPStream* stream, NPBool seekable, uint16* stype);
        NPError DestroyStream(NPStream* stream, NPReason reason);
        int32 WriteReady(NPStream* stream);
        int32 Write(NPStream* stream, int32 offset, int32 len, void* buffer);
        void StreamAsFile(NPStream* stream, const char* fname);
        void Print(NPPrint* platformPrint);
        int16 HandleEvent(void* event);
        void URLNotify(const char* url, NPReason reason, void* notifyData);
        NPError GetValue(NPPVariable variable, void *);
        NPError SetValue(NPNVariable variable, void *);
    };

}; }; // FB::Npapi

#endif