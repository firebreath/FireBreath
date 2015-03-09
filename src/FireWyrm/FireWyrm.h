/*********************************************************************\
    Original Author: Richard Bateman

    Created:    Jan 18, 2015
    License:    Dual license model; choose one of two:
                New BSD License
                http://www.opensource.org/licenses/bsd-license.php
                - or -
                GNU Lesser General Public License, version 2.1
                http://www.gnu.org/licenses/lgpl-2.1.html

    Copyright 2015 Richard Bateman and the Firebreath development team
\*************************************************&&&&****************/

#pragma once
#ifndef H_FIREWYRM
#define H_FIREWYRM

#include <cstdint>

typedef uint8_t  FW_RESULT;
typedef uint32_t FW_INST;
typedef uint32_t FW_MSG;


#define FW_SUCCESS
#define FW_ERR_UNKNOWN (FW_SUCCESS + 1)
#define FW_ERR_INVALID_JSON (FW_SUCCESS + 2)
#define FW_ERR_BAD_FORMAT (FW_SUCCESS + 3)
#define FW_ERR_INVALID_COMMAND (FW_SUCCESS + 4)

typedef FW_RESULT (*FW_AsyncCall)(const void* pData);

///
// FireWyrmHost calls -- calls made back into whatever is hosting the plugin, such as the browser
///
typedef FW_RESULT (*FWH_PerformAsyncCall)(const FW_AsyncCall* call, const void* pData);

typedef FW_RESULT (*FW_Command)(const FW_INST colonyId, const uint32_t cmdId, const char* strCommand, uint32_t strCommandLen);
typedef FW_RESULT (*FW_CommandCallback)(const FW_INST colonyId, const uint32_t cmdId, const char* strResp, uint32_t strRespLen);

/**
    FireWyrm messages are structured as Arrays.  Note that the colonyId will usually just be 0.
    cmdId should be unique for any currently outstanding command; the response will be sent with
    a matching cmdId to allow it to be mapped back to the original command.

    Supported argument types:
        These types are permitted as arguments or return values for any property or function
            - int
            - double
            - bool
            - null
            - string
            - binary Data (in the format {$type: "binary", data: "LONG_BASE64_STRING"})
            - object reference (in the format {$type: ref, data: object_id})
            - json data by value
                - By default, all javascript objects are passed by reference
                - If desired to pass by value (much more efficient for passing large amounts of
                  data) then this type is used
                - In the format {$type: json, data: mixed} where the "mixed" value is used without
                  any transformation (no looking through for special types, no references, etc)

    Scoping:
        Everything is scoped to a colonyId
        spawn_id: each spawn_id is unique inside a given colony
        object_id: each object_id is unique to a specific spawn / instance of the plugin

    Messages:
        Arg1 - Command name

    Response:
        Arg1 - Response status
               - "error"
               - "success"

    Specific Messages:
        Message : ["NewI", []]
            - create new instance.
        Response: ["success", id]
            - where id is a numeric (int32) identifier for the created instance

        Message : ["DestroyI", [id]]
            - destroy existing instance
        Response: ["success", id]
            - where id is the numeric (int32) identifier for the instance that was destroyed

        Message : ["Invoke", spawn_id, object_id, "funcName", (Array)[arg1, arg2, arg3]]
            - Invoke method funcName on a given object
            - spawn_id matches an identifier returned by NewI which has not been destroyed
            - object_id matches a known and live object, including 0 which is the "default" or root object for the instance
            - The argument array may be empty but must be provided
            - if "funcName" is "" (empty string) then the object will be invoked as a function
        Response: ["success", retVal]
            - retVal can be any supported argument type
        Alternate Response: ["error", {"error": "error type", "message": "specific error message"}]
            - For example, if "" is the function name but the object is not invokable, you will get:
                ["error", {"error": "could not invoke", "message": "The object is not invokable"}]

        Message : ["GetP", spawn_id, object_id, "propName"]
            - Get the named property for the given object
            - spawn_id matches an identifier returned by NewI which has not been destroyed
            - object_id matches a known and live object, including 0 which is the "default" or root object for the instance
            - If "propName" is "" this will resolve to itself, returning its own object_id
        Response: ["success", retVal]
            - retVal can be any supported argument type
        Alternate Response: ["error", {"error": "error type", "message": "specific error message"}]
            - For example, if the argument name does not exist and is not valid, you will get:
                ["error", {"error": "could not get property", "message": "Property does not exist on this object"}]

        Message : ["SetP", spawn_id, object_id, "propName", value]
            - Set the named property for the given object
            - spawn_id matches an identifier returned by NewI which has not been destroyed
            - object_id matches a known and live object, including 0 which is the "default" or root object for the instance
            - If "propName" is "" this will return an error
        Response: ["success", null]
            - There is no return value for this call
        Alternate Response: ["error", {"error": "error type", "message": "specific error message"}]
            - For example, if the argument name does not exist and is not valid, you will get:
                ["error", {"error": "could not set property", "message": "Property does not exist on this object"}]

        IMPORTANT!
        Any objects returned *must* be released or memory leaks will result

        Message : ["RelObj", spawn_id, object_id]
            - Releases the specified object (so it can be freed)
            - spawn_id matches an identifier returned by NewI which has not been destroyed
            - object_id matches a known and live object, including 0 which is the "default" or root object for the instance
        Response: ["success", null]
            - There is no return value for this call
        Alternate Response: ["error", {"error": "error type", "message": "specific error message"}]
            - For example, if the object is invalid or has already been released, you will get:
                ["error", {"error": "invalid", "message": "The object does not exist"}]
**/

typedef struct _FireWyrmHostFuncs
{
    uint16_t size;
    uint16_t version;
    FW_Command call;
    FW_CommandCallback cmdCallback;
    FWH_PerformAsyncCall doAsyncCall;
} FWHostFuncs;

typedef struct _FireWyrmColonyFuncs
{
    uint16_t size;
    uint16_t version;
    FW_Command call;
    FW_CommandCallback cmdCallback;
} FWColonyFuncs;



#endif // H_FIREWYRM


