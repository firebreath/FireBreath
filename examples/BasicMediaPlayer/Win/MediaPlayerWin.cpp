
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

    PlayerContext() {}
};

MediaPlayer::MediaPlayer()
  : m_context()
  , m_version("")
  , m_type("DirectShow")
{
	CoInitializeEx(0, COINIT_MULTITHREADED);

    m_context = PlayerContextPtr(new PlayerContext);
    if(!m_context) throw InitializationException("failed to create context");

    HRESULT hr =  m_context->spGraph.CoCreateInstance(CLSID_FilterGraph, 0, CLSCTX_INPROC_SERVER);
    if(FAILED(hr)) throw InitializationException("failed to create player");
    
    CComQIPtr<IMediaControl> spMediaControl(m_context->spGraph);
    if(!spMediaControl) throw InitializationException("failed to QI for IMediaControl");

    CComQIPtr<IMediaEvent> spMediaEvent(m_context->spGraph);
    if(!spMediaEvent) throw InitializationException("failed to QI for IMediaEvent");

    m_context->spMediaControl = spMediaControl;
    m_context->spMediaEvent   = spMediaEvent;
}

MediaPlayer::~MediaPlayer()
{

}

const std::string& MediaPlayer::Version() const
{
    return m_version;
}

const std::string& MediaPlayer::Type() const
{
    return m_type;
}

std::string MediaPlayer::play(const std::string& file)
{
    HRESULT hr;

    hr = m_context->spGraph->RenderFile(L"c:\\tmp\\weare.wav", 0);
    if(FAILED(hr)) {
        std::ostringstream ss;
		ss << "IGraphBuilder::RenderFile() failed: " << mapVfwError(hr);
        return ss.str();
    }

    hr = m_context->spMediaControl->Run();
    if(FAILED(hr)) {
        std::ostringstream ss;
		ss << "IMediaControl::Run() failed: " << mapVfwError(hr);
        return ss.str();
    }

    return "all good";
}
