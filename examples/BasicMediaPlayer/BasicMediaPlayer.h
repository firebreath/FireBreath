/**********************************************************\ 
Original Author: Richard Bateman and Georg Fritzsche 

Created:    December 3, 2009
License:    Dual license model; choose one of two:
            Eclipse Public License - Version 1.0
            http://www.eclipse.org/legal/epl-v10.html
            - or -
            GNU Lesser General Public License, version 2.1
            http://www.gnu.org/licenses/lgpl-2.1.html

Copyright 2009 PacketPass Inc, Georg Fritzsche,
               Firebreath development team
\**********************************************************/

#include <boost/shared_ptr.hpp>
#include "JSAPIAuto.h"
#include "BrowserHostWrapper.h"
#include "BrowserObjectAPI.h"

class MediaPlayer;

class BasicMediaPlayer : public FB::JSAPIAuto
{
    typedef std::vector<std::string> PlayList;
public:
    virtual ~BasicMediaPlayer();
    BasicMediaPlayer(FB::BrowserHostWrapper *host);

    // methods exposed to script

    bool Play();
    bool Stop();

    // properties exposed to script

    std::string Version();
    std::string Type();

    JSAPI* Playlist();
    void SetPlaylist(PlayList&);

private:
    typedef boost::shared_ptr<MediaPlayer> MediaPlayerPtr;

    FB::AutoPtr<FB::BrowserHostWrapper> m_host;
    MediaPlayerPtr m_player;
    bool m_valid;

    PlayList m_playlist;
};
