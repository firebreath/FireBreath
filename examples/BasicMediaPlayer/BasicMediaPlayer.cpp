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

#include <boost/assign.hpp>
#include "BasicMediaPlayer.h"
#include "MediaPlayer.h"
#include "DOM/JSAPI_DOMWindow.h"
#include "Util/JSArray.h"
#include "variant_list.h"
#include "util.h"

BasicMediaPlayer::BasicMediaPlayer(FB::BrowserHostWrapper *host) 
  : m_host(host)
  , m_player()
  , m_valid(false)
{
    using FB::make_method;
    using FB::make_property;

    registerMethod  ("play",      make_method  (this, &BasicMediaPlayer::play));
    registerMethod  ("stop",      make_method  (this, &BasicMediaPlayer::stop));
    registerMethod  ("addItem",   make_method  (this, &BasicMediaPlayer::addItem));

    registerProperty("version",   make_property(this, &BasicMediaPlayer::version));
    registerProperty("type",      make_property(this, &BasicMediaPlayer::type));
	registerProperty("lastError", make_property(this, &BasicMediaPlayer::lastError));
    registerProperty("playlist",  make_property(this, &BasicMediaPlayer::playlist,
                                                      &BasicMediaPlayer::setPlaylist));

    try 
    {
        m_player  = MediaPlayerPtr(new MediaPlayer);
        m_valid   = true;
    } 
    catch(MediaPlayer::InitializationException&) 
    {
        m_player.reset();
    }
}

BasicMediaPlayer::~BasicMediaPlayer()
{
    m_player.reset();
}

bool BasicMediaPlayer::play(const std::string& file)
{
    return m_player->play(file);
}

bool BasicMediaPlayer::stop()
{
	return m_player->stop();
}

bool BasicMediaPlayer::addItem(const FB::variant& v)
{
    m_playlist.push_back(v.convert_cast<std::string>());
    //return FB::variant();
    return true;
}

std::string BasicMediaPlayer::version() const
{
    return m_player->version();
}

std::string BasicMediaPlayer::type() const
{
    return m_player->type();
}

std::string BasicMediaPlayer::lastError() const
{
	return m_player->lastError();
}

FB::JSOutArray BasicMediaPlayer::playlist() const
{
    //FB::AutoPtr<FB::BrowserObjectAPI> arr = new ArrayHelper(FB::make_variant_list(m_playlist), m_host);
    //return arr;
    return FB::make_variant_list(m_playlist);
}

void BasicMediaPlayer::setPlaylist(PlayList& list)
{
    m_playlist = list;
}
