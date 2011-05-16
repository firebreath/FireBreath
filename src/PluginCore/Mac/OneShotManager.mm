
/**********************************************************\
 Original Author: Georg Fritzsche
 
 Created:    Nov 6, 2010
 License:    Dual license model; choose one of two:
 New BSD License
 http://www.opensource.org/licenses/bsd-license.php
 - or -
 GNU Lesser General Public License, version 2.1
 http://www.gnu.org/licenses/lgpl-2.1.html
 
 Copyright 2010 Georg Fritzsche and the Firebreath development team
 \**********************************************************/

#include "OneShotManager.h"
#import <Cocoa/Cocoa.h>
#include <iostream>
#include <fstream>
#include <cassert>

@interface OneShotManagerHelper : NSObject {
    FB::OneShotManager* m_manager;
}

- (void)shoot:(id)object;

@end

@implementation OneShotManagerHelper

- (void)setManager:(FB::OneShotManager*) manager
{
	m_manager = manager;
}
- (FB::OneShotManager*)manager
{
	return m_manager;
}

- (void)shoot:(id)object {
    if (m_manager)
        m_manager->npp_asyncCallback();
}

@end

FB::OneShotManager::OneShotManager() : m_helper(NULL), m_shots(0)
{
    OneShotManagerHelper* mHelper = [OneShotManagerHelper new];
    [mHelper setManager:this];
    m_helper = mHelper;
}

FB::OneShotManager::~OneShotManager()
{
    OneShotManagerHelper* mHelper = (OneShotManagerHelper*) m_helper;
    [mHelper release];
    mHelper = NULL;
}

void FB::OneShotManager::npp_register(void* instance) {
    // If there isn't a deque for the instance, create it.
    boost::mutex::scoped_lock lock(m_mutex);
    std::map<void*,SinkQueue*>::iterator sink = m_sinks.find(instance);
    if (m_sinks.end() == sink)
        m_sinks[instance] = new SinkQueue();
}
void FB::OneShotManager::npp_unregister(void* instance) {
    // If there is a deque for the instance, destroy it and all callbacks.
    boost::mutex::scoped_lock lock(m_mutex);
    std::map<void*,SinkQueue*>::iterator sink = m_sinks.find(instance);
    if (m_sinks.end() != sink) {
        m_shots -= sink->second->size();
        delete sink->second;
        m_sinks.erase(sink);
    }
}

void FB::OneShotManager::npp_scheduleAsyncCallback(void* instance, OneShotCallback func, void *userData) {
    // If there is a deque for the instance, add the callback.
    boost::mutex::scoped_lock lock(m_mutex);
    std::map<void*,SinkQueue*>::iterator sink = m_sinks.find(instance);
    if (m_sinks.end() != sink) {
        sink->second->push_back(std::make_pair(userData, func));
        if (!m_shots) {
            OneShotManagerHelper* mHelper = (OneShotManagerHelper*) m_helper;
            [mHelper performSelectorOnMainThread:@selector(shoot:) withObject:NULL waitUntilDone:NO];
        }
        m_shots++;
    }
}

bool FB::OneShotManager::npp_nextCallback(SinkPair& callback) {
    // return the next callback.
    boost::mutex::scoped_lock lock(m_mutex);
    std::map<void*,SinkQueue*>::iterator sink = m_sinks.begin();
    while (m_sinks.end() != sink) {
        SinkQueue::iterator call = sink->second->begin();
        if (sink->second->end() != call) {
            callback = *call;
            sink->second->erase(call);
            m_shots--;
            return true;
        }
        sink++;
    }
    return false;
}

void FB::OneShotManager::npp_asyncCallback() {
    // Must be on main thread.
    // This is done this way because Safari may call NPP_Destroy during the callback.
    SinkPair callback;
    while (npp_nextCallback(callback))
        callback.second(callback.first);
}

FB::OneShotManager& FB::OneShotManager::getInstance()
{
    static OneShotManager manager;
    return manager;
}