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

#include <cstdio>
#include <vector>
#include <string>

#include "APITypes.h"
#include "variant_list.h"
#include "NPJavascriptObject.h"
#include "NpapiHost.h"
#include "NpapiPluginModule.h"
#include "NpapiBrowserHost.h"
#include "TestJSAPI.h"

#define STRID(_host, _str) (_host->GetStringIdentifier(_str))

using namespace FB::Npapi;

inline NPVariant *getNPVariantParams(NpapiBrowserHostPtr host, std::vector<FB::variant> src)
{
    NPVariant *tmp = new NPVariant[src.size()];
    for (unsigned int i = 0; i < src.size(); i++) {
        host->getNPVariant(&tmp[i], src[i]);
    }
    return tmp;
}

inline void freeNPVariantParams(NpapiBrowserHostPtr host, NPVariant *params, unsigned int count)
{
    for (unsigned int i = 0; i < count; i++) {
        host->ReleaseVariantValue(&params[i]);
    }
    delete [] params;
}

TEST (NPJavascriptObject_CreateTest)
{
    PRINT_TESTNAME;

    NpapiPluginModule module;
    NpapiHost testHost(NULL, NULL, NULL);
    module.setNetscapeFuncs(testHost.getBrowserFuncs());

    NpapiBrowserHostPtr host(new NpapiBrowserHost(&module, testHost.getPluginInstance()));
    host->setBrowserFuncs(testHost.getBrowserFuncs());

    boost::shared_ptr<TestObjectJSAPI> testIf(new TestObjectJSAPI());
    NPJavascriptObject *obj = NPJavascriptObject::NewObject(host, testIf);

    host->ReleaseObject(obj);
    CHECK (true);
}

TEST(NPJavascriptObject_Methods)
{
    PRINT_TESTNAME;

    using FB::variant_list_of;

    NpapiPluginModule module;
    NpapiHost testHost(NULL, NULL, NULL);
    module.setNetscapeFuncs(testHost.getBrowserFuncs());

    NpapiBrowserHostPtr host(new NpapiBrowserHost(&module, testHost.getPluginInstance()));
    host->setBrowserFuncs(testHost.getBrowserFuncs());

    NPVariant res;
    NPVariant oneParam;
    boost::shared_ptr<TestObjectJSAPI> testIf(new TestObjectJSAPI());
    NPJavascriptObject *obj = NPJavascriptObject::NewObject(host, testIf);

    NPVariant *params = getNPVariantParams(host, variant_list_of(0)("This is a test"));
    // Test setting and then getting a string value
    CHECK(host->Invoke(obj, STRID(host, "setValue"), params, 2, &res));
    freeNPVariantParams(host, params, 2);

    try {
        INT32_TO_NPVARIANT(0, oneParam);
        CHECK(host->Invoke(obj, STRID(host, "getValue"), &oneParam, 1, &res));
        CHECK(std::string("This is a test") == res.value.stringValue.UTF8Characters);
    } catch (...) {
        CHECK(false);
    }

    // Test setting and then getting integer values
    for (int i = 0; i < TESTOBJECTJSAPI_ACCESSLISTLENGTH; i++) {
        params = getNPVariantParams(host, variant_list_of(i)(i));
        CHECK(host->Invoke(obj, STRID(host, "setValue"), params, 2, &res));
        freeNPVariantParams(host, params, 2);

        INT32_TO_NPVARIANT(i, oneParam);
        CHECK(host->Invoke(obj, STRID(host, "getValue"), &oneParam, 1, &res));
        CHECK(NPVARIANT_TO_INT32(res) == i);
    }

    // Test setting and then getting double values
    for (int i = 0; i < TESTOBJECTJSAPI_ACCESSLISTLENGTH; i++) {
        params = getNPVariantParams(host, variant_list_of(i)((double)(i * 1.5)));
        CHECK(host->Invoke(obj, STRID(host, "setValue"), params, 2, &res));
        freeNPVariantParams(host, params, 2);

        INT32_TO_NPVARIANT(i, oneParam);
        CHECK(host->Invoke(obj, STRID(host, "getValue"), &oneParam, 1, &res));
        CHECK(NPVARIANT_TO_DOUBLE(res) == (double)(i * 1.5));
    }

    // Test invalid parameters; this should *not* fire an exception *ever*
    try {
        CHECK(!host->Invoke(obj, STRID(host, "setValue"), NULL, 0, &res));
        CHECK(true);
    } catch (...) {
        CHECK(false);
    }

    // Test invalid method name
    try {
        CHECK(!host->Invoke(obj, STRID(host, "someMethodThatDoesntExist"), NULL, 0, &res));
        CHECK(true);
    } catch (...) {
        CHECK(false);
    }
}

TEST(NPJavascriptObject_Properties)
{
    PRINT_TESTNAME;

    NpapiPluginModule module;
    NpapiHost testHost(NULL, NULL, NULL);
    module.setNetscapeFuncs(testHost.getBrowserFuncs());

    NpapiBrowserHostPtr host(new NpapiBrowserHost(&module, testHost.getPluginInstance()));
    host->setBrowserFuncs(testHost.getBrowserFuncs());

    NPVariant res;
    NPVariant oneParam;
    boost::shared_ptr<TestObjectJSAPI> testIf(new TestObjectJSAPI());
    NPJavascriptObject *obj = NPJavascriptObject::NewObject(host, testIf);

    // Test setting and getting many different datatypes with properties
    try {
        for (int i = 0; i < 10000; i++) {
            INT32_TO_NPVARIANT(i, oneParam);
            CHECK(host->SetProperty(obj, STRID(host, "value"), &oneParam));
            CHECK(host->GetProperty(obj, STRID(host, "value"), &res));
            CHECK(NPVARIANT_TO_INT32(res) == i);

            DOUBLE_TO_NPVARIANT((double)(i * 3.74), oneParam);
            CHECK(host->SetProperty(obj, STRID(host, "value"), &oneParam));
            CHECK(host->GetProperty(obj, STRID(host, "value"), &res));
            CHECK(NPVARIANT_TO_DOUBLE(res) == (double)(i * 3.74));

            // Roundabout, but easy, way to convert this to a string =]
            std::string testVal = FB::variant((double)(i * 653.251235)).convert_cast<std::string>();
            host->getNPVariant(&oneParam, FB::variant(testVal));
            CHECK(host->SetProperty(obj, STRID(host, "value"), &oneParam));
            CHECK(host->GetProperty(obj, STRID(host, "value"), &res));
            CHECK(testVal == res.value.stringValue.UTF8Characters);
        }
    } catch (...) {
        CHECK(false);
    }

    // Test exception handling
    try {
        CHECK(!host->GetProperty(obj, STRID(host, "SomePropertyThatDoesntExist"), &res));
        CHECK(true);
    } catch (...) {
        CHECK(false);
    }
}

