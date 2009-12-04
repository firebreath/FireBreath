/**********************************************************\ 
Original Author: Richard Bateman (taxilian)

Created:    Nov 7, 2009
License:    Dual license model; choose one of two:
            Eclipse Public License - Version 1.0
            http://www.eclipse.org/legal/epl-v10.html
            - or -
            GNU Lesser General Public License, version 2.1
            http://www.gnu.org/licenses/lgpl-2.1.html

Copyright 2009 Richard Bateman, Firebreath development team
\**********************************************************/

#include <vector>
#include <string>

#include "APITypes.h"
#include "AutoPtr.h"
#include "vector_list.h"
#include "COMJavascriptObject.h"
#include "ActiveXBrowserHost.h"
#include "TestJSAPI.h"

DISPID getIDForName(IDispatchEx *obj, std::string name)
{
    USES_CONVERSION;
    CA2W wStr(name.c_str());
    OLECHAR *oleStr = wStr;
    DISPID dispId(-1);
    HRESULT hr = obj->GetIDsOfNames(IID_NULL, &oleStr, 1, LOCALE_SYSTEM_DEFAULT, &dispId);
    if (SUCCEEDED(hr)) {
        return dispId;
    } else {
        return -1;
    }
}

CComVariant InvokeMethod(IDispatchEx *obj, std::string methodName, VARIANT* args, size_t count)
{
    DISPPARAMS params;
    params.cArgs = count;
    params.cNamedArgs = 0;
    params.rgvarg = args;

    VARIANT res;
    HRESULT hr = obj->Invoke(getIDForName(obj, methodName), IID_NULL, LOCALE_USER_DEFAULT,
        DISPATCH_METHOD, &params, &res, NULL, NULL);

    if (SUCCEEDED(hr)) {
        return res;
    } else {
        throw FB::script_error("Could not get property");
    }
}

TEST (COMJavascriptObject_CreateTest)
{
    PRINT_TESTNAME;

    FB::AutoPtr<ActiveXBrowserHost> host = new ActiveXBrowserHost(NULL);

    FB::AutoPtr<TestObjectJSAPI> testIf = new TestObjectJSAPI();
    CComPtr<IDispatchEx> obj = COMJavascriptObject::NewObject(host, testIf);

    CHECK (true);
}

TEST(COMJavascriptObject_Methods)
{
    PRINT_TESTNAME;

    FB::AutoPtr<ActiveXBrowserHost> host = new ActiveXBrowserHost(NULL);

    FB::AutoPtr<TestObjectJSAPI> testIf = new TestObjectJSAPI();
    CComPtr<IDispatchEx> obj = COMJavascriptObject::NewObject(host, testIf);

    HRESULT hr = S_OK;

    DISPID func;
    CHECK(func = getIDForName(obj, "setValue") != -1);

    {
        try {
            CComVariant args[2] = {"This is a test", 0};
            InvokeMethod(obj, "setValue", args, 2);
            CHECK(true);
        } catch (...) {
            CHECK(false);
        }
        try {
            CComVariant arg(0);
            CComVariant ret = InvokeMethod(obj, "getValue", &arg, 1);

            CW2A cStr(ret.bstrVal);
            std::string strRes(cStr);
            CHECK(strRes == "This is a test");
        } catch (...) {
            CHECK(false);
        }
    }

    try {
        // Test setting and then getting integer values
        for (int i = 0; i < TESTOBJECTJSAPI_ACCESSLISTLENGTH; i++) {
            CComVariant args[2] = {i, i};
            InvokeMethod(obj, "setValue", args, 2);

            CComVariant arg(i);
            CComVariant ret = InvokeMethod(obj, "getValue", &arg, 1);
            ret.ChangeType(VT_I4);
            CHECK(ret.lVal == i);
        }
    } catch (...) {
        CHECK(FALSE);
    }

    try {
        // Test setting and then getting integer values
        for (int i = 0; i < TESTOBJECTJSAPI_ACCESSLISTLENGTH; i++) {
            CComVariant args[2] = {(double)(i * 1.5), i};
            InvokeMethod(obj, "setValue", args, 2);

            CComVariant arg(i);
            CComVariant ret = InvokeMethod(obj, "getValue", &arg, 1);
            ret.ChangeType(VT_R8);
            CHECK(ret.dblVal == (double)(i * 1.5));
        }
    } catch (...) {
        CHECK(FALSE);
    }

    //// Test invalid parameters; this should *not* fire an exception *ever*
    //try {
    //    CHECK(!host->Invoke(obj, STRID(host, "setValue"), NULL, 0, &res));
    //    CHECK(true);
    //} catch (...) {
    //    CHECK(false);
    //}

    //// Test invalid method name
    //try {
    //    CHECK(!host->Invoke(obj, STRID(host, "someMethodThatDoesntExist"), NULL, 0, &res));
    //    CHECK(true);
    //} catch (...) {
    //    CHECK(false);
    //}
}

TEST(COMJavascriptObject_Properties)
{
    //PRINT_TESTNAME;

    //NpapiPluginModule module;
    //NpapiHost testHost(NULL, NULL, NULL);
    //module.setNetscapeFuncs(testHost.getBrowserFuncs());

    //FB::AutoPtr<NpapiBrowserHost> host = new NpapiBrowserHost(&module, testHost.getPluginInstance());
    //host->setBrowserFuncs(testHost.getBrowserFuncs());

    //NPVariant res;
    //NPVariant oneParam;
    //FB::AutoPtr<TestObjectJSAPI> testIf = new TestObjectJSAPI();
    //NPJavascriptObject *obj = NPJavascriptObject::NewObject(host, testIf);

    //// Test setting and getting many different datatypes with properties
    //try {
    //    for (int i = 0; i < 10000; i++) {
    //        INT32_TO_NPVARIANT(i, oneParam);
    //        CHECK(host->SetProperty(obj, STRID(host, "value"), &oneParam));
    //        CHECK(host->GetProperty(obj, STRID(host, "value"), &res));
    //        CHECK(NPVARIANT_TO_INT32(res) == i);

    //        DOUBLE_TO_NPVARIANT((double)(i * 3.74), oneParam);
    //        CHECK(host->SetProperty(obj, STRID(host, "value"), &oneParam));
    //        CHECK(host->GetProperty(obj, STRID(host, "value"), &res));
    //        CHECK(NPVARIANT_TO_DOUBLE(res) == (double)(i * 3.74));

    //        // Roundabout, but easy, way to convert this to a string =]
    //        std::string testVal = FB::variant((double)(i * 653.251235)).convert_cast<std::string>();
    //        host->getNPVariant(&oneParam, FB::variant(testVal));
    //        CHECK(host->SetProperty(obj, STRID(host, "value"), &oneParam));
    //        CHECK(host->GetProperty(obj, STRID(host, "value"), &res));
    //        CHECK(testVal == res.value.stringValue.utf8characters);
    //    }
    //} catch (...) {
    //    CHECK(false);
    //}

    //// Test exception handling
    //try {
    //    CHECK(!host->GetProperty(obj, STRID(host, "SomePropertyThatDoesntExist"), &res));
    //    CHECK(true);
    //} catch (...) {
    //    CHECK(false);
    //}
}