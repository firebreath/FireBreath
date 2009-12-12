
#include <atlbase.h>
#include <atlcom.h>
#include <atlstr.h>
#include <wmp.h>
#include "../MediaPlayer.h"

struct PlayerContext 
{
	CComPtr<IWMPPlayer> spPlayer;

	PlayerContext() : spPlayer() {}
};

MediaPlayer::MediaPlayer()
  : m_context()
  , m_version("")
  , m_type("Windows Media Player")
{
	m_context = PlayerContextPtr(new PlayerContext);
	if(!m_context) {
		throw InitializationException("failed to create context");
	}

	HRESULT hr =  m_context->spPlayer.CoCreateInstance(__uuidof(WindowsMediaPlayer), 0, CLSCTX_INPROC_SERVER);
	if(FAILED(hr)) {
		throw InitializationException("failed to create player");
	}

	CComBSTR version;
	hr = m_context->spPlayer->get_versionInfo(&version);
	if(SUCCEEDED(hr)) {
		const CW2A conv(version);
		m_version = conv;
	}
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