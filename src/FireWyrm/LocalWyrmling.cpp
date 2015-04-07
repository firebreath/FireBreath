/**********************************************************\
Original Author: Richard Bateman (taxilian)

Created:    Mar 10, 2015
License:    Dual license model; choose one of two:
            New BSD License
            http://www.opensource.org/licenses/bsd-license.php
            - or -
            GNU Lesser General Public License, version 2.1
            http://www.gnu.org/licenses/lgpl-2.1.html

Copyright 2015 Richard Bateman, Firebreath development team
\**********************************************************/

#include "LocalWyrmling.h"



using namespace FB::FireWyrm;

FB::FireWyrm::LocalWyrmling::LocalWyrmling(const WyrmBrowserHostPtr& host, const FB::JSAPIWeakPtr& api, FW_INST id, bool autoRelease /*= false*/)
    : m_api(api), m_browser(host), m_autoRelease(autoRelease), m_id(id) {
    if (m_autoRelease) {
        // If we're auto-releasing it we need to auto-retain it
        FB::JSAPIPtr api_strong(api.lock());
        if (api_strong) {
            host->retainJSAPIPtr(api_strong);
        }
    }
}

FB::FireWyrm::LocalWyrmling::LocalWyrmling(LocalWyrmling &&rh) : m_api(rh.m_api), m_browser(rh.m_browser), m_valid(rh.m_valid), m_autoRelease(rh.m_autoRelease), m_id(rh.m_id) {
    if (m_autoRelease) {
        // When moving we don't need the overhead of retain / release
        rh.m_autoRelease = false;
    }
}

FB::FireWyrm::LocalWyrmling::LocalWyrmling(const LocalWyrmling &rh) : m_api(rh.m_api), m_browser(rh.m_browser), m_valid(rh.m_valid), m_autoRelease(rh.m_autoRelease), m_id(rh.m_id) {
    if (m_autoRelease) {
        // If we're auto-releasing it we need to auto-retain it
        FB::JSAPIPtr api_strong(m_api.lock());
        WyrmBrowserHostPtr host_strong(m_browser.lock());
        if (api_strong && host_strong) {
            host_strong->retainJSAPIPtr(api_strong);
        }
    }
}
void LocalWyrmling::swap(LocalWyrmling& s) NOEXCEPT {
    std::swap(s.m_api, this->m_api);
    std::swap(s.m_browser, this->m_browser);
    std::swap(s.m_valid, this->m_valid);
    std::swap(s.m_autoRelease, this->m_autoRelease);
    std::swap(s.m_id, this->m_id);
    std::swap(s.m_api, this->m_api);
    std::swap(s.m_api, this->m_api);
}
LocalWyrmling& LocalWyrmling::operator=(const LocalWyrmling& rh) {
    // By using this idiom the constructors and destructors all work
    LocalWyrmling tmp(rh);
    tmp.swap(*this);
    return *this;
}

FB::FireWyrm::LocalWyrmling::~LocalWyrmling(void) {
    if (m_autoRelease) {
        // If we're auto-releasing it we need to auto-retain it
        FB::JSAPIPtr api_strong(m_api.lock());
        WyrmBrowserHostPtr host_strong(m_browser.lock());
        if (api_strong && host_strong) {
            host_strong->releaseJSAPIPtr(api_strong);
        }
    }
}


FB::VariantListPromise FB::FireWyrm::LocalWyrmling::Enum() {
    std::vector<std::string> memberList;
    getAPI()->getMemberNames(memberList);
    return FB::make_variant_list(memberList);
}

FB::variantPromise FB::FireWyrm::LocalWyrmling::Invoke(std::string name, FB::VariantList args) {
    return getAPI()->Invoke(name, args);
}

FB::variantPromise FB::FireWyrm::LocalWyrmling::GetP(std::string name) {
    return getAPI()->GetProperty(name);
}

bool LocalWyrmling::HasMethod(std::string propName) {
    return getAPI()->HasMethod(propName);
}

FB::Promise<void> FB::FireWyrm::LocalWyrmling::SetP(std::string name, FB::variant value) {
    getAPI()->SetProperty(name, value);
    return FB::Promise<void>(true);
}
