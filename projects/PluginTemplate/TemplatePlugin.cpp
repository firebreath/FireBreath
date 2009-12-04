
#include "MathAPI.h"

#include "TemplatePlugin.h"

void TemplatePlugin::StaticInitialize()
{
    // Place one-time initialization stuff here; note that there isn't an absolute guarantee that
    // this will only execute once per process, just a guarantee that it won't execute again until
    // after StaticDeinitialize is called
}

void TemplatePlugin::StaticDeinitialize()
{
    // Place one-time deinitialization stuff here
}


TemplatePlugin::TemplatePlugin()
{
}

TemplatePlugin::~TemplatePlugin()
{
}

FB::JSAPI* TemplatePlugin::createJSAPI()
{
    return new MathAPI();
}

bool TemplatePlugin::HandleEvent(FB::PluginEvent *evt)
{
    return false;
}

