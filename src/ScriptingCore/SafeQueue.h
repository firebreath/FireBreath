#/**********************************************************\
#Original Author: Anthony Williams
#Imported and Adapted By: Richard Bateman
#Original Source: http://www.justsoftwaresolutions.co.uk/threading/implementing-a-thread-safe-queue-using-condition-variables.html
# 
#License:    Dual license model; choose one of two:
#            New BSD License
#            http://www.opensource.org/licenses/bsd-license.php
#            - or -
#            GNU Lesser General Public License, version 2.1
#            http://www.gnu.org/licenses/lgpl-2.1.html
#            
#Copyright 2008
#\**********************************************************/

#ifndef H_FB_SafeQueue
#define H_FB_SafeQueue

#include <boost/thread/mutex.hpp>
#include <boost/thread/condition_variable.hpp>
#include <queue>

namespace FB {
    template<typename Data>
    class SafeQueue
    {
    private:
        std::queue<Data> the_queue;
        mutable boost::mutex the_mutex;
        boost::condition_variable the_condition_variable;
    public:
        void push(Data const& data)
        {
            boost::mutex::scoped_lock lock(the_mutex);
            the_queue.push(data);
            lock.unlock();
            the_condition_variable.notify_one();
        }

        bool empty() const
        {
            boost::mutex::scoped_lock lock(the_mutex);
            return the_queue.empty();
        }

        bool try_pop(Data& popped_value)
        {
            boost::mutex::scoped_lock lock(the_mutex);
            if(the_queue.empty())
            {
                return false;
            }
            
            popped_value=the_queue.front();
            the_queue.pop();
            return true;
        }

        void wait_and_pop(Data& popped_value)
        {
            boost::mutex::scoped_lock lock(the_mutex);
            while(the_queue.empty())
            {
                the_condition_variable.wait(lock);
            }
            
            popped_value=the_queue.front();
            the_queue.pop();
        }

    };
};

#endif //H_FB_SafeQueue
