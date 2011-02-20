/**********************************************************\ 
Original Author: Georg Fritzsche 

Created:    September 20, 2010
License:    Dual license model; choose one of two:
            New BSD License
            http://www.opensource.org/licenses/bsd-license.php
            - or -
            GNU Lesser General Public License, version 2.1
            http://www.gnu.org/licenses/lgpl-2.1.html

Copyright 2010 Georg Fritzsche,
               Firebreath development team
\**********************************************************/

#include <list>
#include <boost/assign/list_of.hpp>
#include "JSAPI.h"
#include "Mac/PluginWindowMacCA.h"
#include "../MediaPlayer.h"


struct PlayerContext 
{
    std::string error;

    PlayerContext() {}
};

namespace 
{
    PlayerContextPtr make_context()
    {
        PlayerContextPtr context(new PlayerContext);
        return context;
    }
}

MediaPlayer::MediaPlayer()
  : m_context()
  , m_version("")
  , m_type("Mac")
{
    try 
    {
        m_context = make_context();
    }
    catch(const InitializationException& e)
    {
        m_context = PlayerContextPtr(new PlayerContext);
        m_context->error = e.what();
        throw;
    }
}

MediaPlayer::~MediaPlayer()
{
    stop();
}

void MediaPlayer::setWindow(FB::PluginWindow* pluginWindow)
{
    if(pluginWindow) {

    }
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
    return true;
}

bool MediaPlayer::stop()
{
    return true;
}    

