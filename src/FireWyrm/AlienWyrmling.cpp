/**********************************************************\
Original Author: Richard Bateman (taxilian)

Created:    Mar 16, 2015
License:    Dual license model; choose one of two:
            New BSD License
            http://www.opensource.org/licenses/bsd-license.php
            - or -
            GNU Lesser General Public License, version 2.1
            http://www.gnu.org/licenses/lgpl-2.1.html

Copyright 2015 Richard Bateman, Firebreath development team
\**********************************************************/

#include "AlienWyrmling.h"
#include "WyrmColony.h"
#include "logging.h"

using namespace FB::FireWyrm;
using FB::variantPromise;
using FB::variant;
using FB::VariantList;

AlienLarvae::AlienLarvae(WyrmColony* colony, FW_INST spawnId, uint32_t objId) : colony(colony), spawnId(spawnId), objId(objId) {
    m_membersDfd = colony->DoCommand(FB::VariantList{ "Enum", spawnId, objId })
        .then< std::vector<std::string> >([](FB::variant members) -> std::vector<std::string> {
        if (!members.is_of_type<FB::VariantList>()) {
            throw FB::invalid_arguments();
        }
        std::vector< std::string > names;
        for (auto m : members.cast<FB::VariantList>()) {
            names.emplace_back(m.convert_cast<std::string>());
        }
        return names;
    });
}

FB::FireWyrm::AlienLarvae::~AlienLarvae() {
    
}

AlienWyrmlingPtr AlienWyrmling::create(const WyrmBrowserHostPtr h, AlienLarvaePtr larvae) {
    auto wyrmling = std::make_shared<AlienWyrmling>(h, PrivateStruct());
    wyrmling->init(larvae);
    return wyrmling;
}

AlienWyrmling::AlienWyrmling(const WyrmBrowserHostPtr h, PrivateStruct)
    : JSObject(h), m_browser(h), m_startup(false), m_valid(false), m_spawnId(0), m_objId(0) { }

AlienWyrmling::~AlienWyrmling(void) {
    invalidate();
}

void AlienWyrmling::init(AlienLarvaePtr larvae) {
    m_spawnId = larvae->spawnId;
    m_objId = larvae->objId;
    m_valid = true;
    auto ref = std::dynamic_pointer_cast<AlienWyrmling>(shared_from_this());
    m_startup = larvae->m_membersDfd.then<void>([ref](std::vector<std::string> members) -> void {
        ref->m_memberNames = members;
    });
    m_startup.fail([ref](std::exception_ptr ep) {
        ref->m_valid = false;
        try {
            if (ep) {
                std::rethrow_exception(ep);
            }
        } catch(const std::exception& e) {
            FBLOG_WARN("AlienWyrmling", "AlienWyrmling startup failure!" << e.what());
        }
    });
}

void AlienWyrmling::invalidate() {
    if (!m_valid) return;
    if (m_objId > 0) {
        getHost()->DoCommand(FB::VariantList{ "RelObj", m_spawnId, m_objId });
    }
    m_valid = false;
    m_browser.reset();
}

variantPromise AlienWyrmling::GetProperty(std::string propertyName) {
    if (!m_valid) {
        throw FB::object_invalidated();
    }
    WyrmBrowserHostPtr host = getHost();
    auto spawnId = m_spawnId, objId = m_objId;
    return m_startup.thenPipe<FB::variant>([host, propertyName, spawnId, objId]() -> FB::variantPromise {
        return host->DoCommand(FB::VariantList{ "GetP", spawnId, objId, propertyName });
    });
}

void AlienWyrmling::SetProperty(std::string propertyName, const variant& value) {
    if (!m_valid) {
        throw FB::object_invalidated();
    }
    WyrmBrowserHostPtr host = getHost();
    auto spawnId = m_spawnId, objId = m_objId;
    m_startup.done([host, propertyName, value, spawnId, objId]() {
        host->DoCommand(FB::VariantList{ "SetP", spawnId, objId, propertyName, value });
    });
}

void AlienWyrmling::RemoveProperty(std::string propertyName) {
    if (!m_valid) {
        throw FB::object_invalidated();
    }
    WyrmBrowserHostPtr host = getHost();
    auto spawnId = m_spawnId, objId = m_objId;
    m_startup.done([host, propertyName, spawnId, objId]() {
        host->DoCommand(FB::VariantList{ "DelP", spawnId, objId, propertyName });
    });
}

variantPromise AlienWyrmling::Invoke(std::string methodName, const VariantList& args) {
    if (!m_valid) {
        throw FB::object_invalidated();
    }
    WyrmBrowserHostPtr host = getHost();
    auto spawnId = m_spawnId, objId = m_objId;
    return m_startup.thenPipe<FB::variant>([host, methodName, args, spawnId, objId]() -> FB::variantPromise {
        return host->DoCommand(FB::VariantList{ "Invoke", spawnId, objId, methodName, args });
    });
}

void FB::FireWyrm::AlienWyrmling::callMultipleFunctions(std::string name, const FB::VariantList& args, const std::vector<JSObjectPtr>& direct, const std::vector<JSObjectPtr>& ifaces) {
    throw std::logic_error("Not implemented");
}
