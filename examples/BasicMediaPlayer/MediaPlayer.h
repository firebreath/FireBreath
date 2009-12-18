
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

    const std::string& Version() const;
    const std::string& Type() const;

private:
    boost::shared_ptr<PlayerContext> m_context;
    std::string m_version;
    const std::string m_type;
};

#endif
