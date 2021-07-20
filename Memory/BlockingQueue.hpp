#pragma once

namespace std::execution
{
    class blocking_queue
    {
        using lock_t = unique_lock<mutex>;

    public:
        blocking_queue() = default;
        ~blocking_queue() = default;
        blocking_queue(blocking_queue const&) = delete;
        blocking_queue& operator=(blocking_queue const&) = delete;
        blocking_queue(blocking_queue&&) = delete;
        blocking_queue& operator=(blocking_queue&&) = delete;

    public:
        void enqueue(task_t&& task)
        {
            lock_t lock{ mutex_ };
            bool const toNotify = tasks_.empty();
            tasks_.push_back(move(task));
            if(toNotify)
            {
                cond_var_.notify_one();
            }
        }

        bool try_enqueue(task_t&& task)
        {
            lock_t lock{ mutex_, try_to_lock };
            if(!lock)
            {
                return false;
            }
            bool const toNotify = tasks_.empty();
            tasks_.push_back(move(task));
            if(toNotify)
            {
                cond_var_.notify_one();
            }
            return true;
        }

        void dequeue(task_t& task)
        {
            lock_t lock{ mutex_ };
            while(tasks_.empty())
            {
                cond_var_.wait(lock);
            }
            task = move(tasks_.front());
            tasks_.pop_front();
        }

        bool try_dequeue(task_t& t)
        {
            lock_t lock{ mutex_, try_to_lock };
            if(!lock)
            {
                return false;
            }
            if(tasks_.empty())
            {
                return false;
            }
            t = move(tasks_.front());
            tasks_.pop_front();
            return true;
        }

        template <typename Pred>
        void dequeue(task_t& task, Pred pred)
        {
            lock_t lock{ mutex_ };
            while(tasks_.empty())
            {
                if(!pred())
                {
                    return;
                }
                cond_var_.wait(lock);
            }

            task = move(tasks_.front());
            tasks_.pop_front();
        }

        void notify()
        {
            cond_var_.notify_all();
        }

        mutex              mutex_;
        condition_variable cond_var_;
        deque<task_t>      tasks_;
    };
}