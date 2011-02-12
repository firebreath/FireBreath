/**********************************************************\ 
Original Author: Richard Bateman and Georg Fritzsche 

Created:    December 3, 2009
License:    Dual license model; choose one of two:
            New BSD License
            http://www.opensource.org/licenses/bsd-license.php
            - or -
            GNU Lesser General Public License, version 2.1
            http://www.gnu.org/licenses/lgpl-2.1.html

Copyright 2009 Georg Fritzsche,
               Firebreath development team
\**********************************************************/

#define _WIN32_DCOM
#pragma warning(disable : 4995)

#include <list>
#include <boost/assign/list_of.hpp>
#include <atlbase.h>
#include <atlcom.h>
#include <atlstr.h>
#include <dshow.h>
#include "JSAPI.h"
#include "Win/PluginWindowWin.h"
#include "../MediaPlayer.h"
#include "error_mapping.h"


struct PlayerContext 
{
    CComPtr<IGraphBuilder> spGraph;
    CComPtr<IMediaControl> spMediaControl;
    CComPtr<IMediaEvent>   spMediaEvent;
    CComPtr<IVideoWindow>  spVideoWindow;
    
    HWND hwnd;
    std::string error;

    PlayerContext() : hwnd(0) {}
};

namespace 
{
    std::string vfwErrorString(const std::string& what, HRESULT hr)
    {
        std::ostringstream os;
        os << what << ": " << mapVfwError(hr);
        return os.str();
    }

    PlayerContextPtr make_context(HWND hwnd)
    {
        PlayerContextPtr context(new PlayerContext);
        if(!context) throw MediaPlayer::InitializationException("failed to create context");

        context->hwnd = hwnd;

        HRESULT hr =  context->spGraph.CoCreateInstance(CLSID_FilterGraph, 0, CLSCTX_INPROC_SERVER);
        if(FAILED(hr)) throw MediaPlayer::InitializationException("failed to create player");
        
        CComQIPtr<IMediaControl> spMediaControl = context->spGraph;
        if(!spMediaControl) 
            throw MediaPlayer::InitializationException("failed to QI for IMediaControl");
        context->spMediaControl = spMediaControl;

        CComQIPtr<IMediaEvent> spMediaEvent = context->spGraph;
        if(!spMediaEvent) 
            throw MediaPlayer::InitializationException("failed to QI for IMediaEvent");
        context->spMediaEvent = spMediaEvent;

        CComQIPtr<IVideoWindow> spVideoWindow = context->spGraph;
        if(!spVideoWindow) 
            throw MediaPlayer::InitializationException("failed to QI for IVideoWindow");
        context->spVideoWindow = spVideoWindow;

        return context;
    }

    bool activateVideo(PlayerContextPtr context)
    {
        if(!context->hwnd)
            return true;

        static const std::list<HRESULT> ignore = 
            boost::assign::list_of(E_PROP_ID_UNSUPPORTED)(E_NOINTERFACE);
        HRESULT hr;

        hr = context->spVideoWindow->put_Owner((OAHWND)context->hwnd);
        
        if(std::find(ignore.begin(), ignore.end(), hr) != ignore.end())
            return true;

    
        if(FAILED(hr)) {
            context->error = vfwErrorString("IVideoWindow::put_Owner() failed", hr);
            return false;
        }

        const long windowStyle = WS_CHILD | WS_CLIPCHILDREN | WS_CLIPSIBLINGS;
        hr = context->spVideoWindow->put_WindowStyle(windowStyle);
        if(FAILED(hr)) {
            context->error = vfwErrorString("IVideoWindow::put_WindowStyle() failed", hr);
            return false;
        }

        RECT rect;
        ::GetClientRect(context->hwnd, &rect);
        hr = context->spVideoWindow->SetWindowPosition
                (rect.left, rect.top, rect.right, rect.bottom);
        if(FAILED(hr)) {
            context->error = vfwErrorString("IVideoWindow::SetWindowPosition() failed", hr);
            return false;
        }

        return true;
    }
};

MediaPlayer::MediaPlayer()
  : m_context()
  , m_version("")
  , m_type("DirectShow")
{
    ::CoInitializeEx(0, COINIT_MULTITHREADED);

    try
    {
        m_context = make_context(0);
    }
    catch(const InitializationException& e)
    {
        if(!m_context)
            m_context = PlayerContextPtr(new PlayerContext);
        m_context->error = e.what();
        throw;
    }
}

MediaPlayer::~MediaPlayer()
{
    HRESULT hr;
    
    stop();
    hr = m_context->spVideoWindow->put_Owner(0);
}

void MediaPlayer::setWindow(FB::PluginWindow* pluginWindow)
{
    HRESULT hr;
    HWND hwnd = 0;

    if(pluginWindow) {
        FB::PluginWindowWin* wnd = reinterpret_cast<FB::PluginWindowWin*>(pluginWindow);
        hwnd = wnd->getHWND();
    }
    
    if(m_context->hwnd) {
        hr = m_context->spVideoWindow->put_Owner((OAHWND)hwnd);
        if(FAILED(hr)) {
            m_context->error = vfwErrorString("IVideoWindow::put_Owner() failed", hr);
        }
    }
    
    m_context->hwnd = hwnd;
}

const std::string& MediaPlayer::version() const
{
    return m_version;
}

const std::string& MediaPlayer::type() const
{
    return m_type;
}

const std::string& MediaPlayer::lastError() const
{
    return m_context->error;
}

bool MediaPlayer::play(const std::string& file_)
{
    HRESULT hr;

    CA2W fileConversion(file_.c_str());
    CComBSTR file(fileConversion);
    PlayerContextPtr context = make_context(m_context->hwnd);

    hr = context->spGraph->RenderFile(file, 0);
    if(FAILED(hr)) {
        m_context->error = vfwErrorString("IGraphBuilder::RenderFile() failed", hr);
        return false;
    }

    OAFilterState state;
    hr = m_context->spMediaControl->GetState(50, &state);
    if(FAILED(hr)) {
        m_context->error = vfwErrorString("IMediaControl::GetState() failed", hr);
        return false;
    }

    if(state == State_Running) {
        hr = m_context->spMediaControl->Stop();
        if(FAILED(hr)) {
            m_context->error = vfwErrorString("IMediaControl::Stop() failed: ", hr);
            return false;
        }
    }

    std::swap(m_context, context);

    activateVideo(m_context);

    hr = m_context->spMediaControl->Run();
    if(FAILED(hr)) {
        m_context->error = vfwErrorString("IMediaControl::Run() failed", hr);
        return false;
    }
    
    return true;
}

bool MediaPlayer::stop()
{
    HRESULT hr;
    bool success = true;
    
    hr = m_context->spMediaControl->Stop();
    if(FAILED(hr)) {
        m_context->error = vfwErrorString("IMediaControl::Stop() failed", hr);
        success = false;
    }

    m_context = make_context(m_context->hwnd);

    return success;
}    

