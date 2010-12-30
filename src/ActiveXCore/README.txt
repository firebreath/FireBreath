If you are changing any files in this directory, it should be things
that should have been in FireBreath to start out with -- things that
you might consider contributing back (whether you are actually going
to or not is your business).  It should not be things that are
specific to your project.

These files are used to create the ActiveX control.  Quick
documentation on what files are here and what they do:

ActiveXBrowserHost.cpp/.h - This is the ActiveX specific
    implementation of BrowserHostWrapper.  It handles operations that
    are browser specific as well as providing functionality on ActiveX
    for operations that need to be possible on any browser.  You will
    never create this directly and in a plugin should not need to
    access anything on this that is not exposed by BrowserHostWrapper.

axstream.cpp/.h - This contains the base class for creating "streams"
    in ActiveX.  You should not need to use this directly in a plugin.

axstream_impl.cpp/.h - This is a templated class that makes use of
    axstream to implement a specific stream type in ActiveX.  You
    should not need to use this directly in a plugin.

axutil.cpp/.h - This contains a helper function for putting in
    registry entries.  It is only important on VS2005 or older and
    prevents entries from getting stuck in HKLM instead of HKCU.

COMJavascriptObject.h - This is the COM/ActiveX object that is used
    when a JSAPI object is passed back to the browser (except for the
    root JSAPI object, which is a FBControl object).  You should not
    need to directly use this class.  Note that most functionality is
    contained in the JSAPI_IDispatchEx object that this inherits from.

FBControl.h/.cpp - The .h file contains most of the code.  This is the
    actual COM object that is created for each <object> tag.  This
    object drives the plugin lifecycle, and the PluginCore object is
    contained in this object.  You should not have to work with this
    object directly, however, because calls into the PluginCore object
    should provide hooks for all parts of the lifecycle.  All
    javascript/JSAPI related functionality for this class are
    contained in the JSAPI_IDispatchEx object that this inherits from.
    One thing to be aware of is that this object is a templated object,
    and so new versions are created for each supported MIME type

IDispatchAPI.cpp/.h - This is the inverse of COMJavascriptObject; it
    is a JSAPI object that wraps a IDispatch object.  You shouldn't
    need to deal with this directly, as you will normally use the
    JSObject object from ScriptingCore that this extends.

JSAPI_IDispatchEx.h - This is the base class for COMJavascriptObject
    and FBControl.  It implements IDispatchEx and wraps a JSAPI
    object, allowing Internet Explorer to interact with a COM object
    and see only the JSAPI object attached to it.

registrymap.hpp - Used to extend the ATL .rgs registry mapping tools
    to allow a lot more modularity.  Thanks to Michael Geddes who
    posted this on codeproject.com
