
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

@property (assign) FB::OneShotManager* manager;

- (void)shoot:(id)object;

@end

@implementation OneShotManagerHelper

@synthesize manager=m_manager;

- (void)shoot:(id)object {
    if (m_manager)
        m_manager->npp_asyncCallback();
}

@end

FB::OneShotManager::OneShotManager() : m_helper(NULL), m_shot(false)
{
    OneShotManagerHelper* mHelper = [OneShotManagerHelper new];
    mHelper.manager = this;
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
        if (!m_shot) {
            OneShotManagerHelper* mHelper = (OneShotManagerHelper*) m_helper;
            [mHelper performSelectorOnMainThread:@selector(shoot:) withObject:NULL waitUntilDone:NO];
             m_shot = true;
       }
    }
}

void FB::OneShotManager::npp_asyncCallback() {
    // Must be on main thread.
    // Copy all callbacks to a tmp deque and clear the callbacks.
    SinkQueue calls;
    {
        boost::mutex::scoped_lock lock(m_mutex);
        std::map<void*,SinkQueue*>::iterator sink = m_sinks.begin();
        while (m_sinks.end() != sink) {
            calls.insert(calls.end(), sink->second->begin(), sink->second->end());
            sink->second->clear();
            sink++;
        }
        m_shot = false;
    }
    // Call all the callbacks.
    SinkQueue::iterator call = calls.begin();
    while (calls.end() != call) {
        (*call).second((*call).first);
        call++;
    }
}

FB::OneShotManager& FB::OneShotManager::getInstance()
{
    static OneShotManager manager;
    return manager;
}