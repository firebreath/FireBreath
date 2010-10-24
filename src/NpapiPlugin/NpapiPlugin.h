/**********************************************************\ 
Original Author: Richard Bateman (taxilian)

Created:    Oct 19, 2009
License:    Dual license model; choose one of two:
            New BSD License
            http://www.opensource.org/licenses/bsd-license.php
            - or -
            GNU Lesser General Public License, version 2.1
            http://www.gnu.org/licenses/lgpl-2.1.html

Copyright 2009 Richard Bateman, Firebreath development team
\**********************************************************/

#pragma once
#ifndef H_FB_NPAPI_NPAPIPLUGIN
#define H_FB_NPAPI_NPAPIPLUGIN

#include "NpapiTypes.h"
#include "NpapiBrowserHost.h"
#include "NPJavascriptObject.h"
#include "BrowserPlugin.h"

namespace FB { namespace Npapi {

    struct PluginCreateError : std::runtime_error
    {
        PluginCreateError(const std::string& error)
          : std::runtime_error(error)
        { }
    };
    
    class NpapiPlugin;
    typedef boost::shared_ptr<NpapiPlugin> NpapiPluginPtr;

    class NpapiPlugin : public FB::BrowserPlugin
    {
    public:
        NpapiPlugin(const NpapiBrowserHostPtr& host);
        virtual ~NpapiPlugin(void);

        virtual void init(NPMIMEType pluginType, int16_t argc, char* argn[], char *argv[]);
        virtual void shutdown();

        virtual void setReady();

    protected:
        NPJavascriptObject *m_obj;
        NpapiBrowserHostPtr m_npHost;
        bool m_retainReturnedNPObject;
        bool m_isReady;

        NPObject *getScriptableObject();
        static void signalStreamOpened(void* stream);

    public:
        // These calls are proxied from the NpapiPluginModule to this object, and are
        // the NPP_ functions given to the browser; essentially, the entrypoints for the
        // plugin instance
        virtual NPError SetWindow(NPWindow* window);
        virtual NPError NewStream(NPMIMEType type, NPStream* stream, NPBool seekable, uint16_t* stype);
        virtual NPError DestroyStream(NPStream* stream, NPReason reason);
        virtual int32_t WriteReady(NPStream* stream);
        virtual int32_t Write(NPStream* stream, int32_t offset, int32_t len, void* buffer);
        virtual void StreamAsFile(NPStream* stream, const char* fname);
        virtual void Print(NPPrint* platformPrint);
        virtual int16_t HandleEvent(void* event);
        virtual void URLNotify(const char* url, NPReason reason, void* notifyData);
        virtual NPError GetValue(NPPVariable variable, void *);
        virtual NPError SetValue(NPNVariable variable, void *);
    };

}; }; // FB::Npapi

#endif
