
#ifndef H_TEMPLATEPLUGIN
#define H_TEMPLATEPLUGIN

#include "PluginCore.h"

class TemplatePlugin : public FB::PluginCore
{
public:
    static void StaticInitialize();
    static void StaticDeinitialize();

public:
    TemplatePlugin();
    virtual ~TemplatePlugin();

public:
    virtual FB::JSAPI* createJSAPI();
    virtual bool IsWindowless() { return false; }
    virtual bool HandleEvent(FB::PluginEvent *evt);
};

#endif