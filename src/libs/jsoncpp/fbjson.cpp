/**********************************************************\
Original Author: Richard Bateman
 
Created:    Jun 28, 2011
License:    Dual license model; choose one of two:
            New BSD License
            http://www.opensource.org/licenses/bsd-license.php
            - or -
            GNU Lesser General Public License, version 2.1
            http://www.gnu.org/licenses/lgpl-2.1.html

Copyright 2011 Facebook, Inc
\**********************************************************/

#include "fbjson.h"

namespace FB { 
    FB::variant jsonValueToVariant( Json::Value root )
    {
        Json::Value def;
        if (root.isString())
            return root.asString();
        else if (root.isBool())
            return root.asBool();
        else if (root.isDouble())
            return root.asDouble();
        else if (root.isInt())
            return root.asInt();
        else if (root.isUInt())
            return root.asUInt();
        else if (root.isNull())
            return FB::FBNull();
        else if (root.isArray()) {
            FB::VariantList outList;
            for (size_t i = 0; i < root.size(); ++i) {
                outList.push_back(jsonValueToVariant(root.get(i, def)));
            }
            return outList;
        } else if (root.isObject()) {
            Json::Value::Members members = root.getMemberNames();
            FB::VariantMap outMap;
            for (Json::Value::Members::iterator it = members.begin(); it != members.end(); ++it)
            {
                outMap[*it] = jsonValueToVariant(root.get(*it, def));
            }
            return outMap;
        } else {
            return FB::FBVoid();
        }
    }

    FB::variant jsonToVariantValue(const std::string& json)
    {
        Json::Reader rdr;
        Json::Value root;

        rdr.parse(json, root, false);

        return jsonValueToVariant(root);
    }

    Json::Value variantToJsonValue(const FB::variant &val)
    {
        if (val.is_of_type<std::string>()) {
            return Json::Value(val.convert_cast<std::string>());
        } else if (val.is_of_type<FB::VariantMap>()) {
            Json::Value retVal(Json::objectValue);
            FB::VariantMap map = val.cast<FB::VariantMap>();
            for (FB::VariantMap::iterator it = map.begin(); it != map.end(); ++it) {
                retVal[it->first] = variantToJsonValue(it->second);
            }
            return retVal;
        } else if (val.is_of_type<FB::VariantList>()) {
            Json::Value retVal(Json::arrayValue);
            FB::VariantList list = val.cast<FB::VariantList>();
            for (FB::VariantList::iterator it = list.begin(); it != list.end(); ++it) {
                retVal.append(variantToJsonValue(*it));
            }
            return retVal;
        } else if (val.is_of_type<int>()
                || val.is_of_type<char>()
                || val.is_of_type<short>()
                || val.is_of_type<long>()) {
            return Json::Value((Json::Int)val.convert_cast<long>());
        } else if (val.is_of_type<unsigned int>()
                || val.is_of_type<unsigned short>()
                || val.is_of_type<unsigned char>()
                || val.is_of_type<unsigned long>()) {
            return Json::Value((Json::UInt) val.convert_cast<unsigned long>());
        } else if (val.is_of_type<double>() || val.is_of_type<float>()) {
            return Json::Value(val.convert_cast<double>());
        } else if (val.is_of_type<bool>()) {
            return Json::Value(val.convert_cast<bool>());
        } else {
            return Json::Value(Json::nullValue);
        }
    }

};
