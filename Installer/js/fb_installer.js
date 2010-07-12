/*****************************************************************\
Firebreath JS installer and instantiation script
Original Author: Richard Bateman (taxilian)

Created:    Dec 9, 2009
License:    Dual license model; choose one of two:
            New BSD License
            http://www.opensource.org/licenses/bsd-license.php
            - or -
            GNU Lesser General Public License, version 2.1
            http://www.gnu.org/licenses/lgpl-2.1.html

Copyright 2009 PacketPass, Inc and the Firebreath development team
\*****************************************************************/

if (!FireBreath) {
    FireBreath = { };
}
if (!FireBreath.pluginDefs) {
    FireBreath.pluginDefs = { };
}

/*********************************************\
 Here the data for all the plugins is defined
 - To support multiple plugins with the same
   install script, add additional entries
 - To support one plugin with multiple mime-
   types, pretend that it's a second plugin =]
\*********************************************/

FireBreath.pluginDefs.${PLUGIN_NAME} = {
        "name"          : "${FBSTRING_PluginName}",
        "mimeType"      : "${FBSTRING_MIMEType}",
        "activeXName"   : "${ACTIVEX_PROGID}",
        "guid"          : "${FBControl_GUID}",
        "installURL"    : {
                "win"   : "${PLUGIN_NAME}_installer.msi"
            }
    };




/*********************************************\
 You should not need to modify anything below
 this line
\*********************************************/

if (!FireBreath.$)
{
    FB.$ = function(el)
    {
        if (typeof(el) == "string") {
            return document.getElementById(el);
        } else {
            return el;
        }
    };
}

FB.isPluginInstalled = function(pluginName)
{
    if (!FireBreath.pluginDefs[pluginName]) {
        return null;
    }
    
    if (window.ActiveXObject) {
        // We're running IE
        return FB._isIEPluginInstalled(pluginName);
    } else if (navigator.plugins) {
        // We're running something else
        return FB._isNpapiPluginInstalled(pluginName);
    }
};

FB._isIEPluginInstalled = function(pluginName)
{
    var axname = FireBreath.pluginDefs[pluginName].activeXName;

    var plugin = false;
    try {
        plugin = new ActiveXControl(axname);
    } catch (e) {
        if (e.message != "Object doesn't support this property or method")
        {
            // Couldn't create the plugin; return false
            return false;
        }
    }
    if (plugin) {
        try {
            var version = plugin.version;
            return version;
        } catch (e) {
            return true; // Installed, unknown version
        }
    }
    // Couldn't create the plugin; return false
    return false;
};

FB._isNpapiPluginInstalled = function(pluginName)
{
    var mimeType = FireBreath.pluginDefs[pluginName].mimeType;
    var name = FireBreath.pluginDefs[pluginName].name;
    
    if (typeof(navigator.plugins[name]) != "undefined")
    {
        var re = /([0-9.]+)\.dll/; // look for the version at the end of the filename, before dll

        // Get the filename
        var filename = navigator.plugins[name].filename;
        // Search for the version
        var fnd = re.exec(filename);
        if (fnd === null) {// no version found
            return true; // plugin installed, unknown version
        } else {
            return fnd[1]; // plugin installed, returning version
        }
    }
}
