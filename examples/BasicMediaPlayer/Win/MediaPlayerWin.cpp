/**********************************************************\ 
Original Author: Richard Bateman and Georg Fritzsche 

Created:    December 3, 2009
License:    Dual license model; choose one of two:
            Eclipse Public License - Version 1.0
            http://www.eclipse.org/legal/epl-v10.html
            - or -
            GNU Lesser General Public License, version 2.1
            http://www.gnu.org/licenses/lgpl-2.1.html

Copyright 2009 Georg Fritzsche,
               Firebreath development team
\**********************************************************/

#define _WIN32_DCOM
#pragma warning(disable : 4995)

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

PlayerContextPtr make_context()
{
	PlayerContextPtr context(new PlayerContext);
	if(!context) throw MediaPlayer::InitializationException("failed to create context");

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

MediaPlayer::MediaPlayer()
  : m_context()
  , m_version("")
  , m_type("DirectShow")
{
	::CoInitializeEx(0, COINIT_MULTITHREADED);

	try
	{
		m_context = make_context();
	}
	catch(InitializationException& e)
	{
		if(!m_context)
			m_context = PlayerContextPtr(new PlayerContext);
		m_context->error = e.what();
		throw;
	}
}

MediaPlayer::~MediaPlayer()
{

}

void MediaPlayer::setWindow(FB::PluginWindow* pluginWindow)
{
	FB::PluginWindowWin* wnd = reinterpret_cast<FB::PluginWindowWin*>(pluginWindow);
	m_context->hwnd = wnd->getHWND();
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
	PlayerContextPtr context = make_context();

    hr = context->spGraph->RenderFile(file, 0);
    if(FAILED(hr)) {
        std::ostringstream os;
		os << "IGraphBuilder::RenderFile() failed: " << mapVfwError(hr);
        m_context->error = os.str();
		return false;
    }

	OAFilterState state;
	hr = m_context->spMediaControl->GetState(50, &state);
	if(FAILED(hr)) {
		std::ostringstream os;
		os << "IMediaControl::GetState() failed: " << mapVfwError(hr);
		m_context->error = os.str();
		return false;
	}

	if(state == State_Running) {
		hr = m_context->spMediaControl->Stop();
		if(FAILED(hr)) {
			std::ostringstream os;
			os << "IMediaControl::Stop() failed: " << mapVfwError(hr);
			m_context->error = os.str();
			return false;
		}
	}

	std::swap(m_context, context);

#if 0
	m_context->spVideoWindow->put_Owner((OAHWND)m_context->hwnd);
	m_context->spVideoWindow->put_WindowStyle(WS_CHILD | WS_CLIPSIBLINGS);

	RECT rc;
	::GetClientRect(m_context->hwnd, &rc);
	m_context->spVideoWindow->SetWindowPosition(0, 0, rc.right, rc.bottom);
#endif

    hr = m_context->spMediaControl->Run();
    if(FAILED(hr)) {
        std::ostringstream os;
		os << "IMediaControl::Run() failed: " << mapVfwError(hr);
        m_context->error = os.str();
		return false;
    }
	
	return true;
}

bool MediaPlayer::stop()
{
	HRESULT hr;
	
	hr = m_context->spMediaControl->Stop();
	if(FAILED(hr)) {
		std::ostringstream os;
		os << "IMediaControl::Stop() failed: " << mapVfwError(hr);
		m_context->error = os.str();
	}

	return (hr == S_OK);
}	
