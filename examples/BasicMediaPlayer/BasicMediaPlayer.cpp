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


BasicMediaPlayer::BasicMediaPlayer(FB::BrowserHostWrapper *host) 
  : m_host(host)
  , m_player()
  , m_valid(false)
  , m_playlist()
  , m_currentIndex(0)
{
    using FB::make_method;
    using FB::make_property;

    registerMethod  ("play",         make_method  (this, &BasicMediaPlayer::play));
    registerMethod  ("stop",         make_method  (this, &BasicMediaPlayer::stop));
    registerMethod  ("next",         make_method  (this, &BasicMediaPlayer::next));
    registerMethod  ("previous",     make_method  (this, &BasicMediaPlayer::previous));
    registerMethod  ("addItem",      make_method  (this, &BasicMediaPlayer::addEntry));
    registerMethod  ("removeItem",   make_method  (this, &BasicMediaPlayer::removeEntry));

    registerProperty("version",      make_property(this, &BasicMediaPlayer::version));
    registerProperty("lastError",    make_property(this, &BasicMediaPlayer::lastError));
    registerProperty("currentIndex", make_property(this, &BasicMediaPlayer::currentIndex));
    registerProperty("playlist",     make_property(this, &BasicMediaPlayer::playlist,
                                                         &BasicMediaPlayer::setPlaylist));

	registerEvent("onplaylistChanged");
	registerEvent("oncurrentItemChanged");

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

bool BasicMediaPlayer::play(const FB::CatchAll& catchall)
{
    const FB::VariantList& args = catchall.value;

    if(args.size() == 0)
    {
        if(!m_playlist.size())
            return false;
        return m_player->play(m_playlist[m_currentIndex]);
    }
    else if(args.size() == 1)
    {
        const FB::variant& var = args.front();
        const std::type_info& ti = var.get_type();

        if((ti == typeid(std::string))) 
        {
            m_playlist.push_back(var.cast<std::string>());
            m_currentIndex = m_playlist.size() - 1;
            fireCurrentItemChanged();
            return m_player->play(m_playlist.back());
        } 
        else 
        {
            long idx = var.convert_cast<long>();
            if((idx < 0) || (idx >= m_playlist.size())) {
                return false;
            }

            if(m_currentIndex != idx) {
                m_currentIndex = idx;
                fireCurrentItemChanged();
            }
            
            return m_player->play(m_playlist[idx]);
        }
    }

    return false;
}

bool BasicMediaPlayer::stop()
{
    return m_player->stop();
}

bool BasicMediaPlayer::next()
{
    if(m_currentIndex >= (m_playlist.size()-1))
        return false;
    bool result = m_player->play(m_playlist[++m_currentIndex]);
    if(result)
        fireCurrentItemChanged();
    return result;
}

bool BasicMediaPlayer::previous()
{
    if(m_currentIndex < 1)
        return false;
    bool result = m_player->play(m_playlist[--m_currentIndex]);
    if(result)
        fireCurrentItemChanged();
    return result;
}

bool BasicMediaPlayer::addEntry(const std::string& entry)
{
    if(!entry.length())
        return false;
    m_playlist.push_back(entry);
    firePlaylistChanged();
    return true;
}

bool BasicMediaPlayer::removeEntry(const FB::variant& arg)
{
    const std::type_info& ti = arg.get_type();

    if(ti == typeid(std::string)) {
        PlayList::iterator it =    std::find(m_playlist.begin(), m_playlist.end(),
                                          arg.cast<std::string>());
        if(it == m_playlist.end())
            return false;
        m_playlist.erase(it);
        if(m_currentIndex > 0)
            --m_currentIndex;
        firePlaylistChanged();
        return true;
    } 

    long idx = arg.convert_cast<long>();
    if((idx < 0) || (idx >= m_playlist.size())) {
        return false;
    }

    m_playlist.erase(m_playlist.begin() + idx);
    if(m_currentIndex > 0)
        --m_currentIndex;
    firePlaylistChanged();
    return true;
}

std::string BasicMediaPlayer::version() const
{
    return m_player->version();
}

std::string BasicMediaPlayer::lastError() const
{
    return m_player->lastError();
}

BasicMediaPlayer::PlayList::size_type 
BasicMediaPlayer::currentIndex() const
{
    return m_currentIndex;
}

FB::JSOutArray BasicMediaPlayer::playlist() const
{
    return FB::make_variant_list(m_playlist);
}

void BasicMediaPlayer::setPlaylist(PlayList& list)
{
    m_playlist = list;
    m_currentIndex = 0;
    firePlaylistChanged();
}

void BasicMediaPlayer::setWindow(FB::PluginWindow* win)
{
    m_player->setWindow(win);
}

void BasicMediaPlayer::firePlaylistChanged()
{
    FireEvent
        ("onplaylistChanged", 
         FB::variant_list_of
           (FB::make_variant_list(m_playlist))
           (m_currentIndex));
}

void BasicMediaPlayer::fireCurrentItemChanged()
{
    FireEvent
        ("oncurrentItemChanged",
         FB::variant_list_of(m_currentIndex));
}
