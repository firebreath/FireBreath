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

#ifndef H_MEDIAPLAYER
#define H_MEDIAPLAYER

#include <boost/shared_ptr.hpp>
#include <stdexcept>
#include <string>

struct PlayerContext;
typedef boost::shared_ptr<PlayerContext> PlayerContextPtr;

class MediaPlayer
{
public:
    struct InitializationException : std::runtime_error {
        InitializationException(const char* const what) : std::runtime_error(what) {}
    };

    MediaPlayer();
    ~MediaPlayer();

    const std::string& version() const;
    const std::string& type() const;
	const std::string& lastError() const;

    bool play(const std::string& file);
	bool stop();

private:
    boost::shared_ptr<PlayerContext> m_context;
    std::string m_version;
    const std::string m_type;
};

#endif
