/**********************************************************\
 Original Author: Eric Herrmann

 Created:    2011
 License:    Dual license model; choose one of two:
             New BSD License
             http://www.opensource.org/licenses/bsd-license.php
             - or -
             GNU Lesser General Public License, version 2.1
             http://www.gnu.org/licenses/lgpl-2.1.html

 Copyright 2010 Eric Herrmann, Firebreath development team
\**********************************************************/

#import <AppKit/AppKit.h>

#include "ConstructDefaultPluginWindows.h"
#include "KeyCodesCarbon.h"
#include "PluginEvents/MacEventCarbon.h"
#include "PluginEvents/GeneralEvents.h"
#include "PluginEvents/DrawingEvents.h"
#include "PluginEvents/MouseEvents.h"
#include "PluginEvents/KeyboardEvents.h"

#include "PluginWindowMac.h"
#include "PluginEventMacCarbon.h"

using namespace FB;

#ifndef NP_NO_CARBON

static uint32_t modifiersForEvent(const EventRecord *event)
{
    uint32_t rval = 0;
    if (event->modifiers & cmdKey)
        rval |= NSCommandKeyMask;
    if (event->modifiers & shiftKey)
        rval |= NSShiftKeyMask;
    if (event->modifiers & alphaLock)
        rval |= NSAlphaShiftKeyMask;
    if (event->modifiers & optionKey)
        rval |= NSAlternateKeyMask;
    if (event->modifiers & controlKey)
        rval |= NSControlKeyMask;
    return rval;
}

/*
typedef enum {
√ NPCocoaEventDrawRect = 1,                // updateEvt
√ NPCocoaEventMouseDown,                   // mouseDown
√ NPCocoaEventMouseUp,                     // mouseUp
√ NPCocoaEventMouseMoved,                  // NPEventType_AdjustCursorEvent
√ NPCocoaEventMouseEntered,                // NPEventType_AdjustCursorEvent
√ NPCocoaEventMouseExited,                 // NPEventType_AdjustCursorEvent
√ NPCocoaEventMouseDragged,                // NPEventType_AdjustCursorEvent
  NPCocoaEventKeyDown,                     // NPCocoaEventKeyDown
  NPCocoaEventKeyUp,                       // NPCocoaEventKeyUp
√ NPCocoaEventFlagsChanged,                // nullEvent
√ NPCocoaEventFocusChanged,                // NPEventType_GetFocusEvent/NPEventType_LoseFocusEvent
√ NPCocoaEventWindowFocusChanged,          // activateEvt
  NPCocoaEventScrollWheel,                 // Not Supported
  NPCocoaEventTextInput                    // Not Supported
} NPCocoaEventType;

enum NPEventType {
√ nullEvent                     = 0,       // NPCocoaEventFlagsChanged
√ mouseDown                     = 1,       // NPCocoaEventMouseDown
√ mouseUp                       = 2,       // NPCocoaEventMouseUp
  keyDown                       = 3,       // NPCocoaEventKeyDown
  keyUp                         = 4,       // NPCocoaEventKeyUp
√ autoKey                       = 5,       // NPCocoaEventKeyUp
√ updateEvt                     = 6,       // NPCocoaEventDrawRect
  diskEvt                       = 7,       // Not Supported
√ activateEvt                   = 8,       // NPCocoaEventWindowFocusChanged
  osEvt                         = 15,      // Not Supported
  kHighLevelEvent               = 23       // Not Supported
√ NPEventType_GetFocusEvent = (osEvt + 16),// NPCocoaEventFocusChanged
√ NPEventType_LoseFocusEvent,              // NPCocoaEventFocusChanged
√ NPEventType_AdjustCursorEvent,           // NPCocoaEventMouseEntered/NPCocoaEventMouseExited, and NPCocoaEventMouseMoved/NPCocoaEventMouseDragged
  NPEventType_MenuCommandEvent,            // No Implementation in Mozilla or WebKit
  NPEventType_ClippingChangedEvent,        // No Implementation in Mozilla or WebKit
  NPEventType_ScrollingBeginsEvent = 1000, // No Implementation in Mozilla or WebKit
  NPEventType_ScrollingEndsEvent           // No Implementation in Mozilla or WebKit
};
*/

void PluginEventMacCarbon::CarbonToNPCocoaEvent(const EventRecord* evt, NPCocoaEvent& rval)
{
    memset(&rval, 0, sizeof(NPCocoaEvent));
    switch (evt->what) {
        case nullEvent:
        {
            rval.type = NPCocoaEventFlagsChanged;
            rval.data.key.modifierFlags = modifiersForEvent(evt);
            rval.data.key.characters = NULL;
            rval.data.key.charactersIgnoringModifiers = NULL;
            rval.data.key.isARepeat = false;
            rval.data.key.keyCode = 0;
        }   break;
        case mouseDown:
        {
            Point local = GlobalToLocal(evt->where);
            rval.type = NPCocoaEventMouseDown;
            rval.data.mouse.modifierFlags = modifiersForEvent(evt);
            rval.data.mouse.pluginX = local.h;
            rval.data.mouse.pluginY = local.v;
            rval.data.mouse.buttonNumber = (evt->modifiers & controlKey) ? kCGMouseButtonRight : kCGMouseButtonLeft;
            rval.data.mouse.clickCount = 1;
            rval.data.mouse.deltaX = 0;
            rval.data.mouse.deltaY = 0;
            rval.data.mouse.deltaZ = 0;
        }   break;
        case mouseUp:
        {
            Point local = GlobalToLocal(evt->where);
            rval.type = NPCocoaEventMouseUp;
            rval.data.mouse.modifierFlags = modifiersForEvent(evt);
            rval.data.mouse.pluginX = local.h;
            rval.data.mouse.pluginY = local.v;
            rval.data.mouse.buttonNumber = (evt->modifiers & controlKey) ? kCGMouseButtonRight : kCGMouseButtonLeft;
            rval.data.mouse.clickCount = 1;
            rval.data.mouse.deltaX = 0;
            rval.data.mouse.deltaY = 0;
            rval.data.mouse.deltaZ = 0;
        }   break;
        case keyDown:
        {
            NPNSString* characters = TranslateKeyEventRecord(evt);
            if (characters) {
                rval.type = NPCocoaEventKeyDown;
                rval.data.key.modifierFlags = modifiersForEvent(evt);
                rval.data.key.characters = characters;
                rval.data.key.charactersIgnoringModifiers = rval.data.key.characters;
                rval.data.key.isARepeat = false;
                rval.data.key.keyCode = (evt->message & keyCodeMask) >> 8;
            }
        }   break;
        case keyUp:
        case autoKey:
        {
            NPNSString* characters = TranslateKeyEventRecord(evt);
            if (characters) {
                rval.type = NPCocoaEventKeyUp;
                rval.data.key.modifierFlags = modifiersForEvent(evt);
                rval.data.key.characters = characters;
                rval.data.key.charactersIgnoringModifiers = rval.data.key.characters;
                rval.data.key.isARepeat = (autoKey == evt->what);
                rval.data.key.keyCode = (evt->message & keyCodeMask) >> 8;
            }
        }   break;
        case updateEvt:
        {
            PluginWindowMacPtr pluginWindow = m_PluginWindow.lock();
            if (pluginWindow && PluginWindowMac::DrawingModelCoreGraphics == pluginWindow->getDrawingModel()) {
                FB::Rect bounds = pluginWindow->getWindowPosition();
                rval.type = NPCocoaEventDrawRect;
                rval.data.draw.context = (CGContextRef)pluginWindow->getDrawingPrimitive();
                rval.data.draw.x = bounds.left;
                rval.data.draw.y = bounds.top;
                rval.data.draw.width = bounds.right - bounds.left;
                rval.data.draw.height = bounds.bottom -  bounds.top;
            }
        }   break;
        case activateEvt:
        {
            rval.type = NPCocoaEventWindowFocusChanged;
            rval.data.focus.hasFocus = (evt->modifiers & activeFlag) ? true : false;
        }   break;
        case NPEventType_GetFocusEvent:
        {
            rval.type = NPCocoaEventFocusChanged;
            rval.data.focus.hasFocus = true;
        }   break;
        case NPEventType_LoseFocusEvent:
        {
            rval.type = NPCocoaEventFocusChanged;
            rval.data.focus.hasFocus = false;
        }   break;
        case NPEventType_AdjustCursorEvent:
        {
            Point local = GlobalToLocal(evt->where);
            bool mouseOver = isMouseOver(local);
            if (mouseOver && !m_mouseEntered) {
                rval.type = NPCocoaEventMouseEntered;
                m_mouseEntered = true;
            } else if (!mouseOver && m_mouseEntered) {
                rval.type = NPCocoaEventMouseExited;
                m_mouseEntered = false;
            } else
                rval.type = (evt->modifiers & btnState) ? NPCocoaEventMouseDragged : NPCocoaEventMouseMoved;
            rval.data.mouse.modifierFlags = modifiersForEvent(evt);
            rval.data.mouse.pluginX = local.h;
            rval.data.mouse.pluginY = local.v;
            rval.data.mouse.buttonNumber = (evt->modifiers & controlKey) ? kCGMouseButtonRight : kCGMouseButtonLeft;
            rval.data.mouse.clickCount = 0;
            rval.data.mouse.deltaX = 0;
            rval.data.mouse.deltaY = 0;
            rval.data.mouse.deltaZ = 0;
        }   break;
        default: {
        }   break;
    }
}

/*
    // This might work better. Firefox/WebKit probably already ran through this process to generate a UnicodeString and 
    // now is feeding us a keycode at a time and then we're recreating it again. Oh well.
    // This will generate NPCocoaEventTextInput events.
    // Since we seem to be getting modifier keys as events, we can use CGEventCreateKeyboardEvent and feed those through
    // NPCocoaEventKeyDown and NPCocoaEventKeyUp
    From Firefox nsChildView.cpp

    static const EventTypeSpec sTSMEvents[] = { { kEventClassTextInput, kEventTextInputUnicodeForKeyEvent } };
    ::InstallEventHandler(::GetEventDispatcherTarget(),
                        ::NewEventHandlerUPP(PluginKeyEventsHandler),
                        GetEventTypeCount(sTSMEvents),
                        sTSMEvents,
                        NULL,
                        &gPluginKeyEventsHandler);

    InterfaceTypeList supportedServices;
    supportedServices[0] = kTextServiceDocumentInterfaceType;
    supportedServices[1] = kUnicodeDocumentInterfaceType;
    ::NewTSMDocument(2, supportedServices, &mPluginTSMDoc, 0);
    // We'll need to use the "input window".
    ::UseInputWindow(mPluginTSMDoc, YES);
    ::ActivateTSMDocument(mPluginTSMDoc);

    ::TSMSetDocumentProperty(mPluginTSMDoc, kFocusedChildViewTSMDocPropertyTag, sizeof(ChildView *), &self);
    ::TSMProcessRawKeyEvent([theEvent _eventRef]);
    ::TSMRemoveDocumentProperty(mPluginTSMDoc, kFocusedChildViewTSMDocPropertyTag);

    ::DeleteTSMDocument(mPluginTSMDoc);

    ::RemoveEventHandler(gPluginKeyEventsHandler);
*/

NPNSString* PluginEventMacCarbon::TranslateKeyEventRecord(const EventRecord *event) {
    NPNSString* rval = NULL;
#if 1
    // Nothing fancy, just covert the character to a unicode string.
    UniChar unicodeString = (event->message & charCodeMask);
    rval = (NPNSString*) [NSString stringWithCharacters:&unicodeString length:1];
#else
    // Is the TISInputSource still selected? If not update the layout data.
    if (!m_tisInputSource || (kCFBooleanFalse == (CFBooleanRef) TISGetInputSourceProperty(m_tisInputSource, kTISPropertyInputSourceIsSelected))) {
        if (m_tisInputSource) CFRelease(m_tisInputSource);
        m_tisInputSource = TISCopyCurrentKeyboardInputSource();
        m_tisKeyLayoutData = (CFDataRef) TISGetInputSourceProperty(m_tisInputSource, kTISPropertyUnicodeKeyLayoutData);
    }
    
    OSStatus status = paramErr;
    if (m_tisKeyLayoutData) {
        const UCKeyboardLayout *keyLayoutPtr = (const UCKeyboardLayout*) CFDataGetBytePtr(m_tisKeyLayoutData);
        UInt16 virtualKeyCode = (event->message & keyCodeMask) >> 8;
        UInt16 keyAction = (keyDown == event->what) ? kUCKeyActionDown : (keyUp == event->what) ? kUCKeyActionUp : kUCKeyActionAutoKey;
        UInt32 modifierKeyState = (event->modifiers & keyCodeMask) >> 8;
        UInt32 keyboardType = LMGetKbdType();
        OptionBits keyTranslateOptions = 0;
        UniCharCount maxStringLength = 255;
        UniCharCount actualStringLength = 0;
        UniChar unicodeString[255];

        status = UCKeyTranslate(keyLayoutPtr, virtualKeyCode, keyAction, modifierKeyState, keyboardType, keyTranslateOptions, &m_deadKeyState,
            maxStringLength, &actualStringLength, unicodeString);
        if (noErr == status) {
            NSString* temp = [NSString stringWithCharacters:unicodeString length:actualStringLength];
            NSLog(@"%s action=%d keyCode=%d modifiers=%d \"%@\"", __func__, keyAction, virtualKeyCode, modifierKeyState, temp);
            rval = (NPNSString*) temp;
        } else {
            NSLog(@"%s action=%d keyCode=%d modifiers=%d err=%d", __func__, keyAction, virtualKeyCode, modifierKeyState, status);
        }

    } else
        NSLog(@"%s No KeyLayoutData", __func__);
#endif
    return rval;
}

#endif

