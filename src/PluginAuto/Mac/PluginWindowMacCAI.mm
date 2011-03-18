/**********************************************************\
Original Author: Anson MacKeracher
 
Created:    May 5, 2010
License:    Dual license model; choose one of two:
            New BSD License
            http://www.opensource.org/licenses/bsd-license.php
            - or -
            GNU Lesser General Public License, version 2.1
            http://www.gnu.org/licenses/lgpl-2.1.html
 
Copyright 2010 Georg Fritzsche, Firebreath development team
\**********************************************************/

#include "ConstructDefaultPluginWindows.h"

#include "PluginWindowMacCAI.h"

using namespace FB;

@interface PluginWindowMacICA_helper : NSObject
{
    PluginWindowMacICA *m_ica;
}

@property (assign) PluginWindowMacICA* ica;

- (void)fired:(NSTimer*)timer;

@end

@implementation PluginWindowMacICA_helper

@synthesize ica=m_ica;

- (void)fired:(NSTimer*)timer {
    m_ica->Invalidate();
}

@end

FB::PluginWindowMacICA* FB::createPluginWindowMacICA()
{
    return new PluginWindowMacICA();
}

PluginWindowMacICA::PluginWindowMacICA()
    : PluginWindowMacCA(), m_timer(NULL), m_displayOnInvalidate(true)
{
    PluginWindowMacICA_helper *mhelper = [PluginWindowMacICA_helper new];
    mhelper.ica = this;
    m_helper = mhelper;
}

PluginWindowMacICA::~PluginWindowMacICA()
{
    StopAutoInvalidate();
    
    PluginWindowMacICA_helper *mhelper = (PluginWindowMacICA_helper*) m_helper;
    if (mhelper) {
        [mhelper release];
        mhelper = NULL;
    }
}

void PluginWindowMacICA::StartAutoInvalidate(double rate)
{
    StopAutoInvalidate();
    
    PluginWindowMacICA_helper *mhelper = (PluginWindowMacICA_helper*) m_helper;
    m_timer = [[NSTimer scheduledTimerWithTimeInterval:rate target:mhelper selector:@selector(fired:) userInfo:NULL repeats:YES] retain];
}
void PluginWindowMacICA::StopAutoInvalidate()
{
    NSTimer *mtimer = (NSTimer*) m_timer;
    if (mtimer) {
        [mtimer invalidate];
        [mtimer release];
        mtimer = NULL;
    }
}

void PluginWindowMacICA::Invalidate() {
    if (m_displayOnInvalidate) {
        CALayer *mlayer = (CALayer*) m_layer;
        // setNeedsDisplay will draw sometime in the future.
        [[mlayer sublayers] makeObjectsPerformSelector:@selector(setNeedsDisplay)];
        // display will draw now, immediately.
//      [[mlayer sublayers] makeObjectsPerformSelector:@selector(display)];
    }
    InvalidateWindow();
}
