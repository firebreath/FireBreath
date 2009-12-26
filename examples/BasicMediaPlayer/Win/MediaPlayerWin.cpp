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

#include <atlbase.h>
#include <atlcom.h>
#include <atlstr.h>
#include <dshow.h>
#include "JSAPI.h"
#include "../MediaPlayer.h"
#include "error_mapping.h"

#pragma warning(disable:4995)

struct PlayerContext 
{
    CComPtr<IGraphBuilder> spGraph;
    CComPtr<IMediaControl> spMediaControl;
    CComPtr<IMediaEvent>   spMediaEvent;

	std::string error;

    PlayerContext() {}
};

PlayerContextPtr make_context()
{
	PlayerContextPtr context(new PlayerContext);
	if(!context) throw MediaPlayer::InitializationException("failed to create context");

    HRESULT hr =  context->spGraph.CoCreateInstance(CLSID_FilterGraph, 0, CLSCTX_INPROC_SERVER);
    if(FAILED(hr)) throw MediaPlayer::InitializationException("failed to create player");
    
    context->spMediaControl = context->spGraph;
    if(!context->spMediaControl) 
		throw MediaPlayer::InitializationException("failed to QI for IMediaControl");

    context->spMediaEvent = context->spGraph;
    if(!context->spMediaEvent) 
		throw MediaPlayer::InitializationException("failed to QI for IMediaEvent");

	return context;
}

MediaPlayer::MediaPlayer()
  : m_context()
  , m_version("")
  , m_type("DirectShow")
{
	::CoInitializeEx(0, COINIT_MULTITHREADED);
	m_context = make_context();
}

MediaPlayer::~MediaPlayer()
{

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
