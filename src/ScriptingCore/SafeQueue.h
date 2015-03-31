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

#include <queue>
#include <condition_variable>
#include <mutex>

namespace FB {

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// @class  SafeQueue
    ///
    /// @brief  Basic thread-safe queue class
    ////////////////////////////////////////////////////////////////////////////////////////////////////
    template<typename Data>
    class SafeQueue
    {
    private:
        std::queue<Data> the_queue;
        mutable std::mutex the_mutex;
        std::condition_variable the_condition_variable;
    public:
        ////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @fn void SafeQueue::push(Data const& data)
        ///
        /// @brief  Pushes an object onto the end of the queue.
        ///
        /// @param  data    The data. 
        ////////////////////////////////////////////////////////////////////////////////////////////////////
        void push(Data const& data)
        {
            std::unique_lock<std::mutex> lock(the_mutex);
            the_queue.push(data);
            lock.unlock();
            the_condition_variable.notify_one();
        }

        ////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @fn bool SafeQueue::empty() const
        ///
        /// @brief  Queries if the queue is empty 
        ///
        /// @return true if the Queue is empty, false if not. 
        ////////////////////////////////////////////////////////////////////////////////////////////////////
        bool empty() const
        {
            std::unique_lock<std::mutex> lock(the_mutex);
            return the_queue.empty();
        }

        ////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @fn bool SafeQueue::try_pop(Data& popped_value)
        ///
        /// @brief  Try to pop a value off the front of the queue; if the queue is empty returns false
        ///
        /// @param [out] popped_value    The popped value. 
        ///
        /// @return true if a value is returned, false if the queue was empty
        ////////////////////////////////////////////////////////////////////////////////////////////////////
        bool try_pop(Data& popped_value)
        {
            std::unique_lock<std::mutex> lock(the_mutex);
            if(the_queue.empty())
            {
                return false;
            }
            
            popped_value=the_queue.front();
            the_queue.pop();
            return true;
        }

        ////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @fn bool SafeQueue::timed_wait_and_pop(Data& popped_value, const std::chrono::duration<Rep, Period>& rel_time)
        ///
        /// @brief  Tries to pop a value off the front of the queue; if the queue is empty it will wait
        ///         for the specified duration until something is pushed onto the back of the queue
        ///         by another thread or until the duration times out.
        ///
        /// @param [out] popped_value    The popped value. 
        /// @param duration              The duration of time for which to wait
        ///
        /// @return true if a value is returned, false if the queue was empty
        ////////////////////////////////////////////////////////////////////////////////////////////////////
        template< class Rep, class Period >
        bool timed_wait_and_pop(Data& popped_value, const std::chrono::duration<Rep, Period>& rel_time)
        {
            std::unique_lock<std::mutex> lock(the_mutex);
            if(the_queue.empty())
            {
                // Wait for the specified duration if no values are there
                the_condition_variable.wait_for(lock, rel_time);
            }
            
            // See if a value was added; if not, we just timed out
            if(the_queue.empty())
            {
                return false;
            }
            
            popped_value=the_queue.front();
            the_queue.pop();
            return true;
        }
        ////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @fn void SafeQueue::wait_and_pop(Data& popped_value)
        ///
        /// @brief  Tries to pop a value off the front of the queue; if the queue is empty it will wait
        ///         indefinitely until something is pushed onto the back of the queue by another thread.
        ///
        /// @param [out] popped_value    The popped value. 
        ////////////////////////////////////////////////////////////////////////////////////////////////////
        void wait_and_pop(Data& popped_value)
        {
            std::unique_lock<std::mutex> lock(the_mutex);
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

