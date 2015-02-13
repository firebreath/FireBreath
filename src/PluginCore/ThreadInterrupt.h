/**********************************************************\
Original Author: Richard Bateman
Modified from: http://stackoverflow.com/a/7105779/229998

Created:    Feb 10, 2015
License:    Dual license model; choose one of two:
            New BSD License
            http://www.opensource.org/licenses/bsd-license.php
            - or -
            GNU Lesser General Public License, version 2.1
            http://www.gnu.org/licenses/lgpl-2.1.html
\**********************************************************/

#ifndef H_FB_THREADINTERRUPT
#define H_FB_THREADINTERRUPT

#include <thread>
#include <mutex>
#include <condition_variable>
#include <boost/noncopyable.hpp>

namespace FB {
    struct thread_interrupted {};
    class ThreadInterrupt : public boost::noncopyable
    {
    public:
        ThreadInterrupt() : m_stop(false) {}

        void cancel() {
            std::unique_lock<std::mutex> lock(m_mutex);
            m_stop = true;
            m_cond.notify_all();
        }

        template <typename P>
        void wait(const P& period) {
            std::unique_lock<std::mutex> lock(m_mutex);
            if (m_stop || m_cond.wait_for(lock, period) == std::cv_status::no_timeout) {
                m_stop = false;
                throw thread_interrupted();
            }
        }

        bool cancelled() { return m_stop; }
    private:
        bool m_stop;
        std::mutex m_mutex;
        std::condition_variable m_cond;
    };
}

#endif // H_FB_THREADINTERRUPT