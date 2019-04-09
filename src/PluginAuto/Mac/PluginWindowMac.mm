/**********************************************************\
Original Author: Eric Herrmann 

Created:    Feb 8, 2011
License:    Dual license model; choose one of two:
            New BSD License
            http://www.opensource.org/licenses/bsd-license.php
            - or -
            GNU Lesser General Public License, version 2.1
            http://www.gnu.org/licenses/lgpl-2.1.html

Copyright 2011 Eric Herrmann, Firebreath development team
\**********************************************************/

#include "global/config.h"
#include "FactoryBase.h"

#include "PluginWindowMacICA.h"
#include "PluginWindowMacCA.h"
#include "PluginWindowMacCG.h"
#include "PluginWindowMacQD.h"
#include "PluginEventMacCocoa.h"
#include "PluginEventMacCarbon.h"
#include "PluginEvents/GeneralEvents.h"
#include "PluginEvents/DrawingEvents.h"
#include "Mac/NpapiPluginMac.h"
#include "NpapiPluginModule.h"
#include "PluginEventMac.h"

#include "PluginWindowMac.h"

using namespace FB;

@interface PluginWindowMac_helper : NSObject
{
    PluginWindowMac *m_ica;
	
}

- (void)setIca:(PluginWindowMac *)ica;
- (PluginWindowMac *)ica;

- (void)fired:(NSTimer*)timer;

@end

@implementation PluginWindowMac_helper

- (void)setIca:(PluginWindowMac *)ica {
	m_ica = ica;
}
- (PluginWindowMac *)ica {
	return m_ica;
}

- (void)fired:(NSTimer*)timer {
    m_ica->InvalidateWindow();
}

@end

#if FBMAC_USE_INVALIDATINGCOREANIMATION || FBMAC_USE_COREANIMATION || FBMAC_USE_COREGRAPHICS || FBMAC_USE_QUICKDRAW
static bool supports(const FB::Npapi::NpapiBrowserHostPtr &host, NPNVariable what) {
    NPBool value =  FALSE;        
    // err tells us if the browser supports model negotiation
    NPError err = host->GetValue(what, &value);
    return (NPERR_NO_ERROR == err && value) ? true : false;
}

static bool set(const FB::Npapi::NpapiBrowserHostPtr &host, NPPVariable what, void* value) {
    NPError err = host->SetValue(what, value);
    return (NPERR_NO_ERROR == err) ? true : false;
}
#endif

#if FBMAC_USE_INVALIDATINGCOREANIMATION
static bool initPluginWindowMac_ICA(const FB::Npapi::NpapiBrowserHostPtr &host, NPDrawingModel& drawingModel) {
    drawingModel = (NPDrawingModel) -1; 
    if (supports(host, NPNVsupportsInvalidatingCoreAnimationBool) && set(host, NPPVpluginDrawingModel, (void*)NPDrawingModelInvalidatingCoreAnimation)) {
        FBLOG_INFO("PluginCore", "NPDrawingModel=NPDrawingModelInvalidatingCoreAnimation");
        drawingModel = NPDrawingModelInvalidatingCoreAnimation;
        return true;
    }
    return false;
}
#endif
#if FBMAC_USE_COREANIMATION
static bool initPluginWindowMac_CA(const FB::Npapi::NpapiBrowserHostPtr &host, NPDrawingModel& drawingModel) {
    drawingModel = (NPDrawingModel) -1; 
    if (supports(host, NPNVsupportsCoreAnimationBool) && set(host, NPPVpluginDrawingModel, (void*)NPDrawingModelCoreAnimation)) {
        FBLOG_INFO("PluginCore", "NPDrawingModel=NPDrawingModelCoreAnimation");
        drawingModel = NPDrawingModelCoreAnimation;
        return true;
    }
    return false;
}
#endif
#if FBMAC_USE_COREGRAPHICS
static bool initPluginWindowMac_CG(const FB::Npapi::NpapiBrowserHostPtr &host, NPDrawingModel& drawingModel) {
    drawingModel = (NPDrawingModel) -1; 
    if (supports(host, NPNVsupportsCoreGraphicsBool) && set(host, NPPVpluginDrawingModel, (void*)NPDrawingModelCoreGraphics)) {
        FBLOG_INFO("PluginCore", "NPDrawingModel=NPDrawingModelCoreGraphics");
        drawingModel = NPDrawingModelCoreGraphics;
        return true;
    }
    return false;
}
#endif
#if FBMAC_USE_QUICKDRAW
#ifndef NP_NO_QUICKDRAW
static bool initPluginWindowMac_QD(const FB::Npapi::NpapiBrowserHostPtr &host, NPDrawingModel& drawingModel) {
    drawingModel = (NPDrawingModel) -1; 
    if (supports(host, NPNVsupportsQuickDrawBool) && set(host, NPPVpluginDrawingModel, (void*)NPDrawingModelQuickDraw)) {
        FBLOG_INFO("PluginCore", "NPDrawingModel=NPDrawingModelQuickDraw");
        drawingModel = NPDrawingModelQuickDraw;
        return true;
    }
    return false;
}
#endif
#endif

NPDrawingModel PluginWindowMac::initPluginWindowMac(const FB::Npapi::NpapiBrowserHostPtr &host, const std::string& drawingModel) {
    NPDrawingModel rval = (NPDrawingModel) -1; 
#if FBMAC_USE_COREANIMATION
    if (0 == strcmp(drawingModel.c_str(), "NPDrawingModelCoreAnimation"))
        (void) initPluginWindowMac_CA(host, rval);
    else
#endif
#if FBMAC_USE_INVALIDATINGCOREANIMATION
    if (0 == strcmp(drawingModel.c_str(), "NPDrawingModelInvalidatingCoreAnimation"))
        (void) initPluginWindowMac_ICA(host, rval);
    else 
#endif
#if FBMAC_USE_COREGRAPHICS
    if (0 == strcmp(drawingModel.c_str(), "NPDrawingModelCoreGraphics"))
        (void) initPluginWindowMac_CG(host, rval);
    else
#endif
#if FBMAC_USE_QUICKDRAW
#ifndef NP_NO_QUICKDRAW
    if (0 == strcmp(drawingModel.c_str(), "NPDrawingModelQuickDraw"))
        (void) initPluginWindowMac_QD(host, rval);
    else
#endif
#endif
        FBLOG_INFO("PluginCore", "NPDrawingModel=NONE");
    return rval;
}

NPDrawingModel PluginWindowMac::initPluginWindowMac(const FB::Npapi::NpapiBrowserHostPtr &host) {
    NPDrawingModel rval = (NPDrawingModel) -1; 
#if FBMAC_USE_INVALIDATINGCOREANIMATION
    if (initPluginWindowMac_ICA(host, rval))
        return rval;
    else
#endif
#if FBMAC_USE_COREANIMATION
    if (initPluginWindowMac_CA(host, rval))
        return rval;
    else
#endif
#if FBMAC_USE_COREGRAPHICS
    if (initPluginWindowMac_CG(host, rval))
        return rval;
    else
#endif
#if FBMAC_USE_QUICKDRAW
#ifndef NP_NO_QUICKDRAW
    if (initPluginWindowMac_QD(host, rval))
        return rval;
    else
#endif
#endif
        FBLOG_INFO("PluginCore", "NPDrawingModel=NONE");
    return rval;
}

FB::PluginWindowMac* PluginWindowMac::createPluginWindowMac(NPDrawingModel drawingModel) {
    FB::PluginWindowMac* rval = NULL;
    switch (drawingModel) {
#if FBMAC_USE_INVALIDATINGCOREANIMATION
        case NPDrawingModelInvalidatingCoreAnimation:
            rval = getFactoryInstance()->createPluginWindowMacICA();
            break;
#endif
#if FBMAC_USE_COREANIMATION
        case NPDrawingModelCoreAnimation:
            rval = getFactoryInstance()->createPluginWindowMacCA();
            break;
#endif
#if FBMAC_USE_COREGRAPHICS
        case NPDrawingModelCoreGraphics:
            rval = getFactoryInstance()->createPluginWindowMacCG();
            break;
#endif
#if FBMAC_USE_QUICKDRAW
#ifndef NP_NO_QUICKDRAW
        case NPDrawingModelQuickDraw:
            rval = getFactoryInstance()->createPluginWindowMacQD();
            break;
#endif
#endif
        case NPDrawingModelOpenGL:
        default:
            break;
    }
    return rval;
}

PluginWindowMac::PluginWindowMac()
    : PluginWindow(), m_npHost()
    , m_x(0), m_y(0), m_width(0), m_height(0)
    , m_clipTop(0), m_clipLeft(0), m_clipBottom(0), m_clipRight(0)
    , m_timer(NULL), m_helper(NULL), m_drawLabel(false)
{
    PluginWindowMac_helper *mhelper = [PluginWindowMac_helper new];
    [mhelper setIca:this];
    m_helper = mhelper;
}

PluginWindowMac::~PluginWindowMac()
{
    StopAutoInvalidate();
    
    PluginWindowMac_helper *mhelper = (PluginWindowMac_helper*) m_helper;
    if (mhelper) {
        [mhelper release];
        mhelper = NULL;
    }
}

NPError PluginWindowMac::SetWindow(NPWindow* window) {
    if ((m_x != window->x)
        || (m_y != window->y)
        || (m_width != window->width)
        || (m_height != window->height)) {
        m_x = window->x;
        m_y = window->y;
        m_width = window->width;
        m_height = window->height;
        
        ResizedEvent evt;
        SendEvent(&evt);
    }
    if ((m_clipTop != window->clipRect.top)
        || (m_clipLeft != window->clipRect.left)
        || (m_clipBottom != window->clipRect.bottom)
        || (m_clipRight != window->clipRect.right)) {
        m_clipTop = window->clipRect.top;
        m_clipLeft = window->clipRect.left;
        m_clipBottom = window->clipRect.bottom;
        m_clipRight = window->clipRect.right;
        
        ClipChangedEvent evt;
        SendEvent(&evt);
    }
    return NPERR_NO_ERROR;
}

FB::Rect PluginWindowMac::getWindowPosition() const {
    FB::Rect r = { static_cast<int32_t>(m_y), static_cast<int32_t>(m_x),
                   static_cast<int32_t>(m_y + m_height), static_cast<int32_t>(m_x + m_width) };
    return r;
}

FB::Rect PluginWindowMac::getWindowClipping() const {
    FB::Rect r = { m_clipTop, m_clipLeft, m_clipBottom, m_clipRight };
    return r;
}

uint32_t PluginWindowMac::getWindowHeight() const {
    return m_height;
}

uint32_t PluginWindowMac::getWindowWidth() const {
    return m_width;
}

static void timerCallback(NPP npp, uint32_t timerID) {
    boost::shared_ptr<FB::Npapi::NpapiPluginMac> plugin = FB::ptr_cast<FB::Npapi::NpapiPluginMac>(FB::Npapi::getPlugin(npp));
    if (plugin)
        plugin->HandleTimerEvent();
}

int PluginWindowMac::scheduleTimer(int interval, bool repeat) {
    return m_npHost->ScheduleTimer(interval, repeat, timerCallback);
}

void PluginWindowMac::unscheduleTimer(int timerId) {
    m_npHost->UnscheduleTimer(timerId);
}

void PluginWindowMac::handleTimerEvent() {
    TimerEvent ev(0, NULL);
    SendEvent(&ev);
}

void PluginWindowMac::StartAutoInvalidate(double rate) {
    StopAutoInvalidate();
    
    FBLOG_INFO("PluginCore", "AutoInvalidate STARTED " << 1/rate << "fps");
    PluginWindowMac_helper *mhelper = (PluginWindowMac_helper*) m_helper;
    m_timer = [[NSTimer scheduledTimerWithTimeInterval:rate target:mhelper selector:@selector(fired:) userInfo:NULL repeats:YES] retain];
}

void PluginWindowMac::StopAutoInvalidate() {
    FBLOG_INFO("PluginCore", "AutoInvalidate STOPPED");
    NSTimer *mtimer = (NSTimer*) m_timer;
    if (mtimer) {
        [mtimer invalidate];
        [mtimer release];
        mtimer = NULL;
        m_timer = NULL;
    }
}

void PluginWindowMac::InvalidateWindow() const {
    FBLOG_TRACE("PluginCore", "InvalidateWindow");
    NPRect r = { 0, 0, static_cast<uint16_t>(m_height), static_cast<uint16_t>(m_width) };
    if (!m_npHost->isMainThread())
        m_npHost->ScheduleOnMainThread(m_npHost, boost::bind(&Npapi::NpapiBrowserHost::InvalidateRect2, m_npHost, r));
    else
        m_npHost->InvalidateRect(&r);
}

const char* PluginWindowMac::getUserAgent() const
{
    return m_npHost->UserAgent();
}
