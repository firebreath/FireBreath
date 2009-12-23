
#include <atlbase.h>
#include <atlcom.h>
#include <atlstr.h>
#include <dshow.h>
#include "JSAPI.h"
#include "../MediaPlayer.h"

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

std::string vfwError(HRESULT hr)
{
#define ENTRY(X) case X: return #X;
    switch(hr)
    {
        default: return "<unknown>";
        ENTRY(S_OK);
        ENTRY(VFW_S_AUDIO_NOT_RENDERED);
        ENTRY(VFW_S_DUPLICATE_NAME);
        ENTRY(VFW_S_PARTIAL_RENDER);    
        ENTRY(VFW_S_VIDEO_NOT_RENDERED);
        ENTRY(E_ABORT);    
        ENTRY(E_FAIL);    
        ENTRY(E_INVALIDARG);    
        ENTRY(E_OUTOFMEMORY);    
        ENTRY(E_POINTER);    
        ENTRY(VFW_E_CANNOT_CONNECT);    
        ENTRY(VFW_E_CANNOT_LOAD_SOURCE_FILTER);    
        ENTRY(VFW_E_CANNOT_RENDER);    
        ENTRY(VFW_E_INVALID_FILE_FORMAT);    
        ENTRY(VFW_E_NOT_FOUND);    
        ENTRY(VFW_E_UNKNOWN_FILE_TYPE);    
        ENTRY(VFW_E_UNSUPPORTED_STREAM);
    }
#undef ENTRY
}

std::string MediaPlayer::play(const std::string& file)
{
    HRESULT hr;

    hr = m_context->spGraph->RenderFile(L"c:\\tmp\\weare.wav", 0);
    if(FAILED(hr)) {
        std::ostringstream ss;
        ss << "failed to render file: " << vfwError(hr);
        return ss.str();
    }

    hr = m_context->spMediaControl->Run();
    if(FAILED(hr)) {
        std::ostringstream ss;
        ss << "failed to render file: " << vfwError(hr);
        return ss.str();
    }

    return "all good";
}