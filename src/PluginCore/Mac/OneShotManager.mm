
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
    FB::OneShotManager* manager_;
}

- (id)initWithManager:(FB::OneShotManager *)manager;
- (void)dealloc;

- (void)disconnect;
- (void)doStuff:(id)obj;
@end

@implementation OneShotManagerHelper

- (id)initWithManager:(FB::OneShotManager *)manager
{
    if ((self = [super init])) 
    {
        manager_ = manager;
    }
    
    return self;
}

- (void)dealloc
{
    manager_ = 0;
    [super dealloc];
}

- (void)disconnect
{
    manager_ = 0;
}

- (void)doStuff:(id)obj
{
    if (manager_) {
        manager_->shoot();
    }
}

@end

namespace 
{
    OneShotManagerHelper *getHelper(FB::OneShotManager *manager)
    {
        static OneShotManagerHelper *helper = [[OneShotManagerHelper alloc] initWithManager:manager];
        return helper;
    }
}

FB::OneShotManager::OneShotManager()
{
    
}

FB::OneShotManager::~OneShotManager()
{
    [getHelper(this) disconnect];
}

void FB::OneShotManager::push(void* npp, OneShotCallback sink)
{
    boost::mutex::scoped_lock lock(m_mutex);
    m_sinks.push(std::make_pair(npp, sink));
    [getHelper(this) performSelectorOnMainThread:@selector(doStuff:) withObject:nil waitUntilDone:NO];
}

void FB::OneShotManager::clear(void* npp)
{
    // This and push must never be called at the same time; thus, the mutex prevents that
    // from happening.
    boost::mutex::scoped_lock lock(m_mutex);
    SinkQueue tmp;
    SinkPair cur;
    // Go through all items in the queue; add all but those
    // associated with npp to a temporary queue
    while (m_sinks.try_pop(cur)) {
        if (cur.first != npp) {
            tmp.push(cur);
        }
    }
    // Add the items we kept back into the queue
    while (tmp.try_pop(cur)) {
        m_sinks.push(cur);
    }
}

FB::OneShotManager& FB::OneShotManager::getInstance()
{
    static OneShotManager manager;
    return manager;
}

void FB::OneShotManager::shoot()
{
    SinkPair val;
    while (m_sinks.try_pop(val))
    {
        val.second(val.first, 0);
    }
}
