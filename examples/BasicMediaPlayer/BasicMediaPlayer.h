/**********************************************************\ 
Original Author: Richard Bateman and Georg Fritzsche 

Created:    December 3, 2009
License:    Dual license model; choose one of two:
            New BSD License
            http://www.opensource.org/licenses/bsd-license.php
            - or -
            GNU Lesser General Public License, version 2.1
            http://www.gnu.org/licenses/lgpl-2.1.html

Copyright 2009 PacketPass Inc, Georg Fritzsche,
               Firebreath development team
\**********************************************************/

#include <boost/shared_ptr.hpp>
#include "JSAPIAuto.h"
#include "BrowserHost.h"
#include "JSObject.h"

namespace FB { class PluginWindow; };
class MediaPlayer;

class BasicMediaPlayer : public FB::JSAPIAuto
{
    typedef std::vector<std::string> PlayList;
public:
    BasicMediaPlayer(FB::BrowserHostPtr host);
    virtual ~BasicMediaPlayer();

    // methods exposed to script

    bool play(const FB::CatchAll&);
    bool stop();

    bool next();
    bool previous();

    bool addEntry(const std::string&);
    bool removeEntry(const FB::variant&);

    // properties exposed to script

    std::string version() const;
    std::string lastError() const;

    FB::VariantList playlist() const;
    void setPlaylist(const PlayList&);
    void foo() const;

    PlayList::size_type currentIndex() const;

    // helpers

    void setWindow(FB::PluginWindow*);

private:
    typedef boost::shared_ptr<MediaPlayer> MediaPlayerPtr;
    
    bool play();
    bool play(const std::string&);
    bool play(long);

    void firePlaylistChanged();
    void fireCurrentItemChanged();

    FB::BrowserHostPtr m_host;
    MediaPlayerPtr m_player;
    bool m_valid;

    PlayList m_playlist;
    PlayList::size_type m_currentIndex;
};

